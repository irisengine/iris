{% if uv_source == 0 %}
    texture_table[{{texture_index}}].Sample(sampler_table[{{sampler_index}}], input.tex_coord)
{% else if uv_source == 1 %}
    texture_table[{{texture_index}}].Sample(sampler_table[{{sampler_index}}], input.position.xy * float2({{reciprocal_width}}, {{reciprocal_height}}))
{% else if uv_source == 2 %}
    texture_table[{{texture_index}}].Sample(sampler_table[{{sampler_index}}], {{tex_coord}})
{% endif %}
