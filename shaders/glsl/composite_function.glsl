vec4 composite(vec4 colour1, vec4 colour2, vec4 depth1, vec4 depth2, vec2 tex_coord)
{
    vec4 colour = colour2;

    if(depth1.r < depth2.r)
    {
        colour = colour1;
    }

    return colour;
}