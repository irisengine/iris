float4 invert(float4 colour)
{
    return float4(1.0 - colour.r, 1.0 - colour.g, 1.0 - colour.b, colour.a);
}
