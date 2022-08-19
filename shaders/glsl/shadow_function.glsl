float calculate_shadow(vec3 n, vec4 frag_pos_light_space, vec3 light_dir, sampler2D tex)
{
    float shadow = 0.0;
    vec3 proj_coord = frag_pos_light_space.xyz / frag_pos_light_space.w;
    proj_coord = proj_coord * 0.5 + 0.5;

    float closest_depth = texture(tex, proj_coord.xy).r;
    float current_depth = proj_coord.z;
    float bias = 0.001;

    shadow = (current_depth - bias) > closest_depth ? 1.0 : 0.0;

    if (proj_coord.z > 1.0)
    {
        shadow = 0.0;
    }

    return shadow;
}
