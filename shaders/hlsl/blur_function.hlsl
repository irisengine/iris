float4 blur(Texture2D tex, float2 tex_coords, SamplerState smpler)
{
    float offset[5] = {0.0, 1.0, 2.0, 3.0, 4.0};
    float weight[5] = {0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162};

    float4 colour = tex.Sample(smpler, tex_coords) * weight[0];

    for (int i = 1; i < 5; ++i)
    {
        colour += tex.Sample(smpler, tex_coords + float2(0.0f, offset[i] / 800.0f)) * weight[i];
        colour += tex.Sample(smpler, tex_coords - float2(0.0f, offset[i] / 800.0f)) * weight[i];
    }

    for (int i = 1; i < 5; ++i)
    {
        colour += tex.Sample(smpler, tex_coords + float2(offset[i] / 800.0f, 0.0f)) * weight[i];
        colour += tex.Sample(smpler, tex_coords - float2(offset[i] / 800.0f, 0.0f)) * weight[i];
    }

    return colour;
}