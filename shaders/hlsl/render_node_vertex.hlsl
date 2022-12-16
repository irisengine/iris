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

struct ModelData
{
    matrix model;
    matrix normal_matrix;
};

struct IntValue
{
    int value;
};

StructuredBuffer<ModelData> model_data : register(t0);
ConstantBuffer<IntValue> time : register(b5);

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

PSInput main(
    float4 position : TEXCOORD0,
    float4 normal : TEXCOORD1,
    float4 colour : TEXCOORD2,
    float4 tex_coord : TEXCOORD3,
    float4 tangent : TEXCOORD4,
    float4 bitangent : TEXCOORD5,
    uint4 bone_ids : TEXCOORD6,
    float4 bone_weights : TEXCOORD7,
    uint instance_id : SV_InstanceID)
{
    matrix bone_transform = mul(bones[bone_ids[0]], bone_weights[0]);
    bone_transform += mul(bones[bone_ids[1]], bone_weights[1]);
    bone_transform += mul(bones[bone_ids[2]], bone_weights[2]);
    bone_transform += mul(bones[bone_ids[3]], bone_weights[3]);

    float3 T = normalize(mul(mul(bone_transform, tangent), model_data[instance_id].normal_matrix).xyz);
    float3 B = normalize(mul(mul(bone_transform, bitangent), model_data[instance_id].normal_matrix).xyz);
    float3 N = normalize(mul(mul(bone_transform, normal), model_data[instance_id].normal_matrix).xyz);

    float3x3 tbn = transpose(float3x3(T, B, N));

    PSInput result;

    result.vertex_position = position;

    {% if exists("position") %}
        result.vertex_position = {{ position }};
    {% endif %}

    result.frag_position = mul(result.vertex_position, bone_transform);
    result.frag_position = mul(result.frag_position, model_data[instance_id].model);
    result.view_position = mul(result.frag_position, view);
    result.position = mul(result.view_position, projection);

    result.normal = mul(normal, bone_transform);
    result.normal = mul(result.normal, model_data[instance_id].normal_matrix);
    result.view_normal = mul(result.normal, normal_view);
    result.colour = colour;
    result.tex_coord = tex_coord;

    result.tangent_light_pos = float4(mul(light_position.xyz, tbn), 0.0);
    result.tangent_view_pos = float4(mul(camera.xyz, tbn), 0.0);
    result.tangent_frag_pos = float4(mul(result.frag_position, tbn), 0.0);
    
    {% if is_directional_light %}
        result.frag_pos_light_space = mul(result.frag_position, light_view);
        result.frag_pos_light_space = mul(result.frag_pos_light_space, light_projection);
    {% endif %}

    return result;
}
