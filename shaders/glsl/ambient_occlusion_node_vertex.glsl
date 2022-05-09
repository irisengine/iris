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

void main()
{
    tex_coord = vec2(tex.x, tex.y);
    gl_Position = position;
}

