#include "graphics/material_factory.h"

#include <memory>

#include "graphics/material.h"

namespace
{

// hard coded shaders

static const std::string vertex_source_mesh{R"(
#version 330 core
precision mediump float;
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec4 colour;
layout (location = 3) in vec4 tex;
layout (location = 4) in ivec4 bone_ids;
layout (location = 5) in vec4 bone_weights;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 normal_matrix;
uniform mat4 bones[100];

out vec4 fragPos;
out vec2 texCoord;
out vec4 col;
out vec4 norm;
void main()
{
    mat4 bone_transform = bones[bone_ids[0]] * bone_weights[0];
    bone_transform += bones[bone_ids[1]] * bone_weights[1];
    bone_transform += bones[bone_ids[2]] * bone_weights[2];
    bone_transform += bones[bone_ids[3]] * bone_weights[3];

    col = colour;
    norm = normal_matrix * bone_transform * normal;
    texCoord = vec2(tex.x, tex.y);
    fragPos = model * bone_transform * position;
    gl_Position = projection * view * fragPos;
}
)"};

static const std::string fragment_source_mesh{R"(
#version 330 core
precision mediump float;
in vec2 texCoord;
in vec4 col;
in vec4 norm;
in vec4 fragPos;
out vec4 outColor;
uniform sampler2D texture1;
uniform vec3 light;
void main()
{
    const vec3 amb = vec3(0.6, 0.6, 0.6);
    const vec3 lightColour = vec3(1.0, 1.0, 1.0);
    vec3 n = normalize(norm.xyz);
    vec3 light_dir = normalize(light - fragPos.xyz);
    float diff = max(dot(n, light_dir), 0.0);
    vec3 diffuse = diff * lightColour;
    vec3 l = amb + diffuse;
    outColor = vec4(l, 1.0) * col * texture(texture1, texCoord);
}
)"};

static const std::string vertex_source_sprite{R"(
    #version 330 core
    precision mediump float;
    layout (location = 0) in vec4 position;
    layout (location = 1) in vec4 normal;
    layout (location = 2) in vec4 colour;
    layout (location = 3) in vec4 tex;

    uniform mat4 projection;
    uniform mat4 view;
    uniform mat4 model;

    out vec4 fragPos;
    out vec2 texCoord;
    out vec4 col;
    void main()
    {
        col = colour;
        texCoord = vec2(tex.x, tex.y);
        fragPos = model * position;
        gl_Position = projection * view * fragPos;
    }
)"};

static const std::string fragment_source_sprite{R"(
    #version 330 core
    precision mediump float;
    in vec2 texCoord;
    in vec4 col;
    in vec4 fragPos;
    out vec4 outColor;
    uniform sampler2D texture1;
    void main()
    {
        outColor = texture(texture1, texCoord) * col;
        if(outColor.a < 0.01)
        {
            discard;
        }
    }
)"};

}

namespace iris::material_factory
{

Material *sprite()
{
    static auto mat = std::make_unique<Material>(
        vertex_source_sprite, fragment_source_sprite);
    return mat.get();
}

Material *mesh()
{
    static auto mat =
        std::make_unique<Material>(vertex_source_mesh, fragment_source_mesh);
    return mat.get();
}

}
