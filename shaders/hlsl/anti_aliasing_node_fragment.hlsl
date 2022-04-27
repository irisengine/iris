SamplerState sampler_table[] : register(s0);
Texture2D texture_table[] : register(t0, space1);

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

float4 main(PSInput input) : SV_TARGET
{
    const float4 fragment_colour = {{ fragment_colour }};

    const int up_offset = -1;
    const int down_offset = 1;
    const int left_offset = -1;
    const int right_offset = 1;

    const float2 uv = input.tex_coord.xy;
    const Texture2D tex = texture_table[{{input_texture_index}}];
    const SamplerState smplr = sampler_table[{{input_sampler_index}}];
    const float2 inverse_size = float2({{inverse_width}}f, {{inverse_height}}f);

    // get luminance for current sample
    const float3 colour_centre = tex.Sample(smplr, uv).rgb;
    const float luma_centre = rgb_to_luma(colour_centre);
    
    // get luminance for surrounding samples
    const float luma_down = rgb_to_luma(tex.Sample(smplr, uv, int2(0, down_offset)).rgb);
    const float luma_up = rgb_to_luma(tex.Sample(smplr, uv, int2(0, up_offset)).rgb);
    const float luma_left = rgb_to_luma(tex.Sample(smplr, uv, int2(left_offset, 0)).rgb);
    const float luma_right = rgb_to_luma(tex.Sample(smplr, uv, int2(right_offset, 0)).rgb);
    
    // get min and max luminance
    const float luma_min = min(luma_centre, min(min(luma_down, luma_up), min(luma_left, luma_right)));
    const float luma_max = max(luma_centre, max(max(luma_down, luma_up), max(luma_left, luma_right)));
    
    const float luma_range = luma_max - luma_min;
    
    // filter anything outside of the accepted range
    if (luma_range < max(0.0312f, 0.125f * luma_max))
    {
        return float4(colour_centre, 1.0);
    }
    
    // get corner luminance
    const float luma_down_left = rgb_to_luma(tex.Sample(smplr, uv, int2(left_offset, down_offset)).rgb);
    const float luma_up_right = rgb_to_luma(tex.Sample(smplr, uv, int2(right_offset, up_offset)).rgb);
    const float luma_up_left = rgb_to_luma(tex.Sample(smplr, uv, int2(left_offset, up_offset)).rgb);
    const float luma_down_right = rgb_to_luma(tex.Sample(smplr, uv, int2(right_offset, down_offset)).rgb);
    
    const float luma_down_up = luma_down + luma_up;
    const float luma_left_right = luma_left + luma_right;
    
    // get side luminance
    const float luma_left_corners = luma_down_left + luma_up_left;
    const float luma_down_corners = luma_down_left + luma_down_right;
    const float luma_right_corners = luma_down_right + luma_up_right;
    const float luma_up_corners = luma_up_right + luma_up_left;
    
    // calculate string of luminance along the horizontal
    const float edge_h =  abs(-2.0f * luma_left + luma_left_corners) +
                    abs(-2.0f * luma_centre + luma_down_up) * 2.0f +
                    abs(-2.0f * luma_right + luma_right_corners);
    
    // calculate string of luminance along the vertical
    const float edge_v =  abs(-2.0f * luma_up + luma_up_corners) +
                    abs(-2.0f * luma_centre + luma_left_right) * 2.0f +
                    abs(-2.0f * luma_down + luma_down_corners);
    
    // check if current sample lies among a mostly horizontal (or vertical) line
    const bool is_horiz = (edge_h >= edge_v);

    // get directions to test if we are on the positive or negative side of the line
    const float luma_neg = is_horiz ? luma_up : luma_left;
    const float luma_pos = is_horiz ? luma_down : luma_right;
    
    // get the gradient of the luminance in both positive and negative directions
    const float gradient_neg = abs(luma_neg - luma_centre);
    const float gradient_pos = abs(luma_pos - luma_centre);
    
    float step_length = is_horiz ? -inverse_size.y : -inverse_size.x;
    
    // calculate if we are on the negative or positive side of the line and adjust step length if needed
    float luma_local_avg = 0.0f;
    float gradient_scaled = 0.0f;
    if (gradient_neg < gradient_pos)
    {
        luma_local_avg = 0.5f * (luma_pos + luma_centre);
        gradient_scaled = gradient_pos;
        step_length = -step_length;
    }
    else
    {
        luma_local_avg = 0.5f * (luma_neg + luma_centre);
        gradient_scaled = gradient_neg;
    }

    // get blend direction
    float2 current_uv = uv;
    if (is_horiz)
    {
        current_uv.y += step_length * 0.5f;
    }
    else
    {
        current_uv.x += step_length * 0.5f;
    }

    gradient_scaled *= 0.25f;
    
    const float2 offset = is_horiz ? float2(inverse_size.x, 0.0f) : float2(0.0f, inverse_size.y);
    
    float2 uv1 = current_uv - offset;
    float2 uv2 = current_uv + offset;

    float luma_end1 = 0.0f;
    float luma_end2 = 0.0f;

    bool reached1 = false;
    bool reached2 = false;

    // try to find the length of the line by walking both ends of the line until we get a drop in luminance
    // we use increasing step sizes for performance
    for (int i = 0; i < 24; ++i)
    {
        float steps[] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5f, 2.0f, 2.0f, 2.0f, 2.0f, 4.0f, 8.0f,
                          8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f };
        if (!reached1)
        {
            luma_end1 = rgb_to_luma(tex.SampleLevel(smplr, uv1, 0).rgb) - luma_local_avg;
        }

        if (!reached2)
        {
            luma_end2 = rgb_to_luma(tex.SampleLevel(smplr, uv2, 0).rgb) - luma_local_avg;
        }
        
        // check if we have reached the end of either line
        reached1 = reached1 || abs(luma_end1) >= gradient_scaled;
        reached2 = reached2 || abs(luma_end2) >= gradient_scaled;

        if (reached1 && reached2)
        {
            break;
        }

        if (!reached1)
        {
            uv1 -= offset * steps[0];
        }

        if (!reached2)
        {
            uv2 += offset * steps[0];
        }
    }

    
    // get distance we travelled in both directions
    const float distance1 = is_horiz ? (uv.x - uv1.x) : (uv.y - uv1.y);
    const float distance2 = is_horiz ? (uv2.x - uv.x) : (uv2.y - uv.y);
    
    const bool is_direction1 = distance1 < distance2;
    const float distance_final = min(distance1, distance2);
    
    const float edge_thickness = (distance1 + distance2);
    
    const float pixel_offset = -distance_final / edge_thickness + 0.5f;
    
    const bool is_luma_centre_smaller = luma_centre < luma_local_avg;
    const bool correct_variation = ((is_direction1 ? luma_end1 : luma_end2) < 0.0f) != is_luma_centre_smaller;
    float final_offset = correct_variation ? pixel_offset : 0.0f;
    
    // perform subpixel blending
    const float luma_average = (1.0f / 12.0f) * (2.0f * (luma_down_up + luma_left_right) + luma_left_corners + luma_right_corners);
    const float sub_pixel_offset1 = clamp(abs(luma_average - luma_centre) / luma_range, 0.0f, 1.0f);
    const float sub_pixel_offset2 = (-2.0f * sub_pixel_offset1 + 3.0f) * sub_pixel_offset1 * sub_pixel_offset1;
    const float sub_pixel_offset_final = sub_pixel_offset2 * sub_pixel_offset2 * 0.75f;
    
    final_offset = max(final_offset, sub_pixel_offset_final);
    
    float2 final_uv = uv;
    if (is_horiz)
    {
        final_uv.y += final_offset * step_length;
    }
    else
    {
        final_uv.x += final_offset * step_length;
    }
    
    // use the bilinear sampling of the original image to blur and soften the hard edges
    return tex.SampleLevel(smplr, float2(final_uv.x, final_uv.y), 0);
}