#include "graphics/material_factory.h"

#include <memory>

#include "graphics/material.h"

namespace
{

// hard coded shaders

static const std::string vertex_source_sprite { R"(
    #version 330 core
    precision mediump float;
    layout (location = 0) in vec3 position;
    layout (location = 1) in vec3 normal;
    layout (location = 2) in vec3 colour;
    layout (location = 3) in vec3 tex;
    uniform mat4 projection;
    uniform mat4 view;
    uniform mat4 model;
    out vec3 fragPos;
    out vec2 texCoord;
    out vec3 col;
    void main()
    {
        col = colour;
        texCoord = vec2(tex.x, tex.y);
        fragPos = vec3(transpose(model) * vec4(position, 1.0));
        gl_Position = transpose(projection) * transpose(view) * vec4(fragPos, 1.0);
    }
)" };

static const std::string fragment_source_sprite { R"(
    #version 330 core
    precision mediump float;
    in vec2 texCoord;
    in vec3 col;
    in vec3 fragPos;
    out vec4 outColor;
    uniform sampler2D texture1;
    void main()
    {
        outColor = texture(texture1, texCoord) * vec4(col, 1.0);
        if(outColor.a < 0.01)
        {
            discard;
        }
    }
)"};

}

namespace eng::material_factory
{

Material* sprite()
{
    static auto mat = std::make_unique<Material>(vertex_source_sprite, fragment_source_sprite);
    return mat.get();
}

}
