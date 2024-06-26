/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: LicenseRef-NvidiaProprietary
 *
 * NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
 * property and proprietary rights in and to this material, related
 * documentation and any modifications thereto. Any use, reproduction,
 * disclosure or distribution of this material and related documentation
 * without an express license agreement from NVIDIA CORPORATION or
 * its affiliates is strictly prohibited.
 */


#ifndef DS3D_COMMON_IMPL_BASE_DATA_FILTER_H
#define DS3D_COMMON_IMPL_BASE_DATA_FILTER_H

#include "ds3d/common/hpp/datamap.hpp"
#include "ds3d/common/impl/impl_dataprocess.h"

namespace ds3d { namespace impl {

/**
 * @brief Any custom datafilter must derive from BaseImplDataBridge,
 *
 *   For custom lib implementation, user need to implement the following
 *   virtual functions:
 *     startImpl(...), // user also need setCaps(port) in startImpl
 *     stopImpl(), // stop all resources for datafilter
 *     prepollImpl(data), // prepoll on 1st coming data
 *     renderImpl(data, dataConsumedCb), // rendering data. Once data is done, invoke
 *         dataConsumedCb(datamap) callback to notify application
 *     flushImpl(), // flush data in process
 */
class BaseImplDataBridge : public BaseImplDataProcessor<abiDataBridge> {
public:
    BaseImplDataBridge() {}
    ~BaseImplDataBridge() override = default;

    ErrCode process_i(
        const struct VideoBridge2dInput* inputData, const abiOnDataCB* outputDataCb,
        const abiOnBridgeDataCB* inputDataConsumedCb) final
    {
        DS3D_FAILED_RETURN(
            getStateSafe() == State::kRunning, ErrCode::kState, "datafilter is not started");

        DS_ASSERT(inputData);
        GuardCB<abiOnDataCB> guardOutputCb(outputDataCb ? outputDataCb->refCopy() : nullptr);
        OnGuardDataCBImpl outputCbImpl = [gOutputCb = std::move(guardOutputCb), this](
                                             ErrCode code, GuardDataMap data) {
            gOutputCb(code, data.abiRef());
        };

        GuardCB<abiOnBridgeDataCB> guardConsumedCb(
            inputDataConsumedCb ? inputDataConsumedCb->refCopy() : nullptr);
        OnGuardBridgeDataCBImpl consumedCbImpl = [gConsumedCb = std::move(guardConsumedCb), this](
                                               ErrCode code, const struct VideoBridge2dInput* data) {
            gConsumedCb(code, data);
        };
        return processImpl(inputData, std::move(outputCbImpl), std::move(consumedCbImpl));
    }

protected:
    // cplusplus virtual interface, user need to derive from
    virtual ErrCode processImpl(
        const struct VideoBridge2dInput* inputData, OnGuardDataCBImpl outputDataCb,
        OnGuardBridgeDataCBImpl inputConsumedCb) = 0;
};

}}  // namespace ds3d::impl

#endif  // DS3D_COMMON_IMPL_BASE_DATA_FILTER_H
