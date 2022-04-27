{% if vertex_data %}
    texture_table[{{texture_index}}].Sample(sampler_table[{{sampler_index}}], input.tex_coord)
{% else %}
    texture_table[{{texture_index}}].Sample(sampler_table[{{sampler_index}}], input.position.xy * float2({{reciprocal_width}}, {{reciprocal_height}}))
{% endif %}
