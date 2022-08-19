float rgb_to_luma(float3 rgb)
{
    // calculate luminance based on how sensitive the human light is to each channel
    return sqrt(dot(rgb, float3(0.299f, 0.587f, 0.114f)));
}
