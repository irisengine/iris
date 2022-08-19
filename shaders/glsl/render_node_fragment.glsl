in vec2 tex_coord;
in vec3 tangent_frag_pos;
in vec3 tangent_light_pos;
in vec3 tangent_view_pos;
in vec4 col;
in vec4 frag_pos;
in vec4 frag_pos_light_space;
in vec4 norm;
in vec4 vertex_pos;
in vec4 view_norm;
in vec4 view_position;

layout (location = 0) out vec4 out_colour;

{%if render_normal %}
layout (location = 1) out vec4 out_normal;
{% endif %}

{%if render_position %}
layout (location = 2) out vec4 out_position;
{% endif %}

layout (std140, binding = 2) uniform LightData
{
    mat4 light_projection;
    mat4 light_view;
    vec4 light_colour;
    vec4 light_position;
    vec4 light_attenuation;
};

layout (std430, binding = 3) buffer TextureData
{
    uvec2 texture_table[];
};

layout (std430, binding = 4) buffer CubeMapData
{
    uvec2 cube_map_table[];
};

uniform int shadow_map_index;

void main()
{
    {% if exists("fragment_colour") %}
        vec4 fragment_colour = {{ fragment_colour }};
    {% else %}
        vec4 fragment_colour = col;
    {% endif %}

    {% if exists("normal") %}
        vec3 normal = {{ normal }}.xyz;
        normal = normalize(normal * 2.0 - 1.0); 
    {% else %}
        vec3 normal = normalize(norm.xyz);
    {% endif %}

    {% if light_type == 0 %}
        {% if exists("ambient_input") %}
            out_colour = {{ambient_input}};
        {% else %}
            out_colour = light_colour * fragment_colour;
        {% endif %}
    {% endif %}
    {% if light_type == 1 %}
        {% if exists("normal") %}
            vec3 light_dir = normalize(-tangent_light_pos.xyz);
        {% else %}
            vec3 light_dir = normalize(-light_position.xyz);
        {% endif %}

        float shadow = 0.0;
        shadow = calculate_shadow(
            normal,
            frag_pos_light_space,
            light_position.xyz,
            sampler2D(texture_table[shadow_map_index]));

        float diff = (1.0 - shadow) * max(dot(normal, light_dir), 0.0);
        vec3 diffuse = vec3(diff);
        out_colour = vec4(diffuse * fragment_colour.xyz, 1.0);
    {% endif %}
    {% if light_type == 2 %}
        {% if exists("normal") %}
            vec3 light_dir = normalize(tangent_light_pos.xyz - tangent_frag_pos.xyz);
        {% else %}
            vec3 light_dir = normalize(light_position.xyz - frag_pos.xyz);
        {% endif %}
    
        float distance  = length(light_position.xyz - frag_pos.xyz);
        float constant = light_attenuation.x;
        float linear_term = light_attenuation.y;
        float quadratic = light_attenuation.z;
        float attenuation = 1.0 / (constant + linear_term * distance + quadratic * (distance * distance));    
        vec3 att = {attenuation, attenuation, attenuation};
    
        float diff = max(dot(normal, light_dir), 0.0);
        vec3 diffuse = {diff, diff, diff};
        
        out_colour = vec4(diffuse * light_colour.xyz * fragment_colour.xyz * att, 1.0);
    {% endif %}

    {%if render_normal %}
        out_normal = vec4(normalize(view_norm.xyz), 1.0);
    {% endif %}

    {%if render_position %}
        out_position = view_position;
    {% endif %}

    if (fragment_colour.a < 0.01)
    {
        discard;
    }
}
