vec4 blur(sampler2D tex, vec2 tex_coords)
{
    const float offset[5] = {0.0, 1.0, 2.0, 3.0, 4.0};
    const float weight[5] = {0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162};

    vec4 colour = texture(tex, tex_coords) * weight[0];

    for (int i = 1; i < 5; ++i)
    {
        colour += texture(tex, tex_coords + vec2(0.0f, offset[i] / 800.0f)) * weight[i];
        colour += texture(tex, tex_coords - vec2(0.0f, offset[i] / 800.0f)) * weight[i];
    }

    for (int i = 1; i < 5; ++i)
    {
        colour += texture(tex, tex_coords + vec2(offset[i] / 800.0f, 0.0f)) * weight[i];
        colour += texture(tex, tex_coords - vec2(offset[i] / 800.0f, 0.0f)) * weight[i];
    }

    return colour;
}