layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec4 colour;
layout(location = 3) in vec4 tex;
layout(location = 4) in vec4 tangent;
layout(location = 5) in vec4 bitangent;
layout(location = 6) in ivec4 bone_ids;
layout(location = 7) in vec4 bone_weights;

out vec4 vertex_pos;
out vec4 frag_pos;
out vec2 tex_coord;
out vec4 col;
out vec4 norm;
out vec4 frag_pos_light_space;
out vec3 tangent_light_pos;
out vec3 tangent_view_pos;
out vec3 tangent_frag_pos;

layout (std140, binding = 0) uniform CameraData
{
    mat4 projection;
    mat4 view;
    vec3 camera;
};

layout (std140, binding = 1) uniform BoneData
{
    mat4 bones[100];
};

layout (std140, binding = 2) uniform LightData
{
    mat4 light_projection;
    mat4 light_view;
    vec4 light_colour;
    vec4 light_position;
    vec4 light_attenuation;
};

struct Model
{
    mat4 model;
    mat4 normal_matrix;
};
layout (std430, binding = 5) buffer ModelData
{
    Model models[];
};


void main()
{
    vertex_pos = position;
    tex_coord = tex.xy;
    gl_Position = position;
}
