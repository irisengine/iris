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

float4 main(PSInput input) : SV_TARGET
{
    return cube_map_table[{{cube_map_index}}].SampleLevel(
        sampler_table[{{sampler_index}}], 
        normalize(input.normal.xyz),
        0).rgba;
}
