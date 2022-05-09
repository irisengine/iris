float rgb_to_luma(vec3 rgb)
{
    // calculate luminance based on how sensitive the human light is to each channel
    return sqrt(dot(rgb, vec3(0.299f, 0.587f, 0.114f)));
}
