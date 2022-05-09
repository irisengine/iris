in vec4 vertex_pos;
in vec2 tex_coord;
in vec4 col;
in vec4 norm;
in vec4 frag_pos;
in vec4 frag_pos_light_space;
in vec3 tangent_light_pos;
in vec3 tangent_view_pos;
in vec3 tangent_frag_pos;

out vec4 out_colour;

layout (std430, binding = 4) buffer CubeMapData
{
    uvec2 cube_map_table[];
};

void main()
{
    out_colour = texture(samplerCube(cube_map_table[{{cube_map_index}}]), vertex_pos.xyz);
}
