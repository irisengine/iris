set(ARCH_ROOT "${PROJECT_SOURCE_DIR}/include/iris/jobs/arch/x86_64")

target_sources(iris PRIVATE
    fiber.cpp
    ${ARCH_ROOT}/context.h
    ${ARCH_ROOT}/functions.S)
