set(IRIS_SHADER_NAME "sample_vert")
file(READ "sample.hlsl" IRIS_SHADER_SOURCE)
configure_file("sample.in" "sample.h")
