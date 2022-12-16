layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec4 colour;
layout(location = 3) in vec4 tex;
layout(location = 4) in vec4 tangent;
layout(location = 5) in vec4 bitangent;
layout(location = 6) in ivec4 bone_ids;
layout(location = 7) in vec4 bone_weights;

out vec2 tex_coord;
out vec3 tangent_frag_pos;
out vec3 tangent_light_pos;
out vec3 tangent_view_pos;
out vec4 col;
out vec4 frag_pos;
out vec4 frag_pos_light_space;
out vec4 norm;
out vec4 vertex_pos;
out vec4 view_norm;
out vec4 view_position;

layout (std140, binding = 0) uniform CameraData
{
    mat4 projection;
    mat4 view;
    mat4 normal_view;
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
    mat4 bone_transform = bones[bone_ids[0]] * bone_weights[0];
    bone_transform += bones[bone_ids[1]] * bone_weights[1];
    bone_transform += bones[bone_ids[2]] * bone_weights[2];
    bone_transform += bones[bone_ids[3]] * bone_weights[3];
    bone_transform = transpose(bone_transform);

    mat4 normal_matrix = transpose(models[gl_InstanceID].normal_matrix);
    vec3 T = normalize(vec3(normal_matrix * bone_transform * tangent));
    vec3 B = normalize(vec3(normal_matrix * bone_transform * bitangent));
    vec3 N = normalize(vec3(normal_matrix * bone_transform * normal));
    mat3 tbn = transpose(mat3(T, B, N));

    vertex_pos = position;

    {% if position %}
        vertex_pos = {{position}};
    {% endif %}

    frag_pos = transpose(models[gl_InstanceID].model) * bone_transform * position;
    view_position = transpose(view) * frag_pos;
    gl_Position = transpose(projection) * view_position;
    norm = transpose(models[gl_InstanceID].normal_matrix) * bone_transform * normal;
    col = colour;
    tex_coord = vec2(tex.x, tex.y);

    {% if is_directional_light %}
        frag_pos_light_space = transpose(light_projection) * transpose(light_view) * frag_pos;
    {% endif %}

    tangent_light_pos = tbn * light_position.xyz;
    tangent_view_pos = tbn * camera.xyz;
    tangent_frag_pos = tbn * frag_pos.xyz;

    view_norm = transpose(normal_view) * norm;
    
}
