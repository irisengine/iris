cbuffer BoneData : register(b0)
{
    matrix bones[100];
};

cbuffer Light : register(b1)
{
    matrix light_projection;
    matrix light_view;
    float4 light_colour;
    float4 light_position;
    float4 light_attenuation;
};

cbuffer CameraData : register(b2)
{
    matrix projection;
    matrix view;
    matrix normal_view;
    float4 camera;
};

struct ShadowMapIndex
{
    int index;
};

ConstantBuffer<ShadowMapIndex> shadow_map_index : register(b3);

ConstantBuffer<ShadowMapIndex> shadow_map_sampler_index : register(b4);

struct ModelData
{
    matrix model;
    matrix normal_matrix;
};

StructuredBuffer<ModelData> model_data : register(t0);
SamplerState sampler_table[] : register(s0);
Texture2D texture_table[] : register(t0, space1);
TextureCube cube_map_table[] : register(t0, space2);

struct PSInput
{
    precise float4 position : SV_POSITION;
    precise float2 tex_coord : TEXCOORD;
    precise float4 vertex_position : TEXCOORD1;
    precise float4 frag_position : TEXCOORD2;
    precise float4 view_position : COLOR1;
    precise float4 tangent_view_pos : TEXCOORD4;
    precise float4 tangent_frag_pos : TEXCOORD5;
    precise float4 tangent_light_pos : TEXCOORD6;
    precise float4 frag_pos_light_space : TEXCOORD7;
    precise float4 normal : NORMAL;
    precise float4 view_normal : NORMAL1;
    precise float4 colour : COLOR;
};

{% if not render_normal and not render_position %}
float4 main(PSInput input) : SV_TARGET
{% else %}
struct PS_OUTPUT
{
    float4 colour: SV_Target0;

    {% if render_normal %}
        float4 normal: SV_Target1;
    {% endif %}

    {% if render_position %}
        float4 position: SV_Target2;
    {% endif %}
};
PS_OUTPUT main(PSInput input)
{% endif %}
{
    {% if exists("fragment_colour") %}
        float4 fragment_colour = {{ fragment_colour }};
    {% else %}
        float4 fragment_colour = input.colour;
    {% endif %}

    {% if exists("normal") %}
        float3 normal = {{ normal }}.xyz;
        normal = normalize(normal * 2.0 - 1.0); 
    {% else %}
        float3 normal = normalize(input.normal.xyz);
    {% endif %}

    {% if light_type == 0 %}
        {% if exists("ambient_input") %}
            float4 out_colour = {{ambient_input}};
        {% else %}
            float4 out_colour = light_colour * fragment_colour;
        {% endif %}
    {% endif %}
    {% if light_type == 1 %}
        {% if exists("normal") %}
            float3 light_dir = normalize(-input.tangent_light_pos.xyz);
        {% else %}
            float3 light_dir = normalize(-light_position.xyz);
        {% endif %}

        float shadow = 0.0;
        shadow = calculate_shadow(
            normal,
            input.frag_pos_light_space,
            light_dir,
            texture_table[shadow_map_index.index],
            sampler_table[shadow_map_sampler_index.index]);

        float diff = (1.0 - shadow) * max(dot(normal, light_dir), 0.0);
        float3 diffuse = {diff, diff, diff};
        float4 out_colour = float4(diffuse * fragment_colour, 1.0);
    {% endif %}
    {% if light_type == 2 %}
        {% if exists("normal") %}
            float3 light_dir = normalize(input.tangent_light_pos.xyz - input.tangent_frag_pos.xyz);
        {% else %}
            float3 light_dir = normalize(light_position.xyz - input.frag_position.xyz);
        {% endif %}
    
        float distance  = length(light_position.xyz - input.frag_position.xyz);
        float constant = light_attenuation.x;
        float linear_term = light_attenuation.y;
        float quadratic = light_attenuation.z;
        float attenuation = 1.0 / (constant + linear_term * distance + quadratic * (distance * distance));    
        float3 att = {attenuation, attenuation, attenuation};
    
        float diff = max(dot(normal, light_dir), 0.0);
        float3 diffuse = {diff, diff, diff};
        
        float4 out_colour = float4(diffuse * light_colour.xyz * fragment_colour.xyz * att, 1.0);
    {% endif %}

    {% if not render_normal and not render_position %}
        return out_colour;
    {% else %}
        PS_OUTPUT output;
        output.colour = out_colour;
    
        {% if render_normal %}
            output.normal = float4(normalize(input.view_normal.xyz), 1.0f);
        {% endif %}
    
        {% if render_position %}
            output.position = input.view_position;
        {% endif %}

        return output;
    {% endif %}
}
