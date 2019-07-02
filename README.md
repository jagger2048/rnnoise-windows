# rnnoise-windows
This is a RNNoise windows demo. It was modified and restructured so that it can be compiled with MSVC, VS2017.



## Usage:

1. Initialize the rnnoise module.

    ```c++
    DenoiseState *st = rnnoise_create();
    ```

2. Frame process

    ```c++
    ...
    for (size_t n = 0; n < totalSamples / FRAME_SIZE * FRAME_SIZE; n += FRAME_SIZE)
    {

        for (size_t i = 0; i < FRAME_SIZE; i++) processFrame[i] = inputS16[n + i];

        //  Apply the PCM data to the rnnoise and denoise the audio data
        flag = rnnoise_process_frame(st, processFrame, processFrame);

        for (size_t i = 0; i < FRAME_SIZE; i++) outputS16[n + i] = processFrame[i];

        //printf("%f \n", flag);
    }
    ...
    ```

3. Output and free resources

   ```c++
   // output to wav file
   ...
   // free
   rnnoise_destroy(st);
   ```


**Note:**

Although the input and output format of the |***rnnoise_process_frame()*** |function is |**float**|,  we should treat them as |***S16***|, *short format* PCM instead of |***F32***|, *float format* PCM.

If you feed the process function with |F32| PCM data, the function will result in a 0 return
and will not denoise the data.

For more details, you can check the project and test yourself.





# Reference:
- RNNoise-Project - https://github.com/xiph/rnnoise
