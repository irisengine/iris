vec4 invert(vec4 colour)
{
    return vec4(1.0 - colour.r, 1.0 - colour.g, 1.0 - colour.b, colour.a);
}
