function(iris_generate_shaders SHADER_LANGUAGE)
    set(SHADER_INCLUDES "")

    file(GLOB FILES ${PROJECT_SOURCE_DIR}/shaders/${SHADER_LANGUAGE}/*.${SHADER_LANGUAGE})

    foreach(FILE ${FILES})
        string(REGEX REPLACE "[.]${SHADER_LANGUAGE}$" "" SHADER_NAME ${FILE})
        cmake_path(GET SHADER_NAME FILENAME SHADER_NAME)
        string(REGEX REPLACE "[.]${SHADER_LANGUAGE}$" ".h" SHADER_FILE_NAME ${FILE})
        cmake_path(GET SHADER_FILE_NAME FILENAME SHADER_FILE_NAME)

        SET(SHADER_INCLUDES "${SHADER_INCLUDES}\n#include \"${SHADER_LANGUAGE}/${SHADER_FILE_NAME}\"")

        file(READ ${FILE} SHADER_SOURCE)
        configure_file(../shader_string.in ${SHADER_FILE_NAME})
    endforeach()

    configure_file(../shader_include.in "${SHADER_LANGUAGE}.h")
endfunction()

add_subdirectory("glsl")
add_subdirectory("hlsl")
add_subdirectory("msl")