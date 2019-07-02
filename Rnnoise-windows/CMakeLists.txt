set(RNNOISE_WINDOWS_SRC
    Rnnoise-windows.cpp)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
include_directories("./include")

link_directories("${PROJECT_BINARY_DIR}/libs")
add_executable(rnnoise-windows ${RNNOISE_WINDOWS_SRC})
add_dependencies(rnnoise-windows rnnoise)
target_link_libraries(rnnoise-windows rnnoise)