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

layout (std430, binding = 3) buffer TextureData
{
    uvec2 texture_table[];
};

void main()
{
    const vec4 fragment_colour = {{ fragment_colour }};
    const float gamma = {{gamma}};

    vec3 mapped = fragment_colour.rgb / (fragment_colour.rgb + vec3(1.0, 1.0, 1.0));
    mapped = pow(fragment_colour.rgb, vec3(gamma, gamma, gamma));

    out_colour = vec4(mapped.r, mapped.g, mapped.b, 1.0);
}
