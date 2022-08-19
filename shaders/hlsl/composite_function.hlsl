float4 composite(float4 colour1, float4 colour2, float4 depth1, float4 depth2, float2 tex_coord)
{
    float4 colour = colour2;

    if(depth1.r < depth2.r)
    {
        colour = colour1;
    }

    return colour;
}