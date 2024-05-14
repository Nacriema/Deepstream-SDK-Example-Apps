* Pipeline = plugin_1 (element_1) + plugin_2 ...
* First element: (source) contains just an output (source)
* Last element: (sink) contains just an input (sink)
* Other element between these two (filter) contains both sink and source (input and output)

Basic flow of a pipeline (read from file URI)

1. filesrc (reading from the file)
2. h264parse (since h264 stream, we need a h264 parser) (A parser)
3. nvv4l2decoder (hvdec_h264 to Accelerate decode n GPU)  (A decoder)
4. nvstreammux (perform batch collection from one or more sources) (Batch collector)
5. Use nvinfer or nvinferserver to run inferencing on the decoder output (PGIE - Primary inference engine)
6. nvvideoconvert Convert from NV12 to RGBA as required by nvosd
7. nvdsosd OSD to fraw on converted RGBA buffer
8. EGL based window: nv3dsink for Jetson / nveglglessink for dGPU 
    fakesink / encoder + muxer + filesink /

Put plugins together to form the pipelin
```c++

// Non rtsp out
gst_bin_add_many (GST_BIN (pipeline),
      source, h264parser, decoder, streammux, pgie,
      nvvidconv, nvosd, sink, NULL);

// RTSP out
gst_bin_add_many (GST_BIN (pipeline),
      source, h264parser, decoder, streammux, pgie,
      nvvidconv, nvosd, transform, cap_filter, encoder, rtppay, sink, NULL);
```

A sink pad (input) for the streammux is created and linked to decoder source pad (output)


Finnaly we need to link all elements together by gst_element_link_many