include_directories("../include")
set(RNNOISE_SRC
    celt_lpc.c
    kiss_fft.c
    pitch.c
    rnn_data.c
    rnn.c)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/libs)
add_library(rnnoise SHARED ${RNNOISE_SRC})
add_library(rnnoise_static STATIC ${RNNOISE_SRC})
set_target_properties(rnnoise_static PROPERTIES OUTPUT_NAME "rnnoise")