##  Error details: gstnvinfer.cpp(2397): gst_nvinfer_output_loop (): /GstPipeline:dstest1-pipeline/GstNvInfer:primary-nvinference-engine: streaming stopped, reason not-negotiated (-4)

* Ref: https://forums.developer.nvidia.com/t/deepstream6-0-deepstream-test1-internal-data-stream-error/200320
* When I run in the host machine without the EGL/GLES (GUI) platform
* Solution: Replaced “nveglglessink” with “fakesink” in line 210 of deepstream_test1_app.c, the problem is now fixed!