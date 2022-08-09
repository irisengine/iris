{% if uv_source == 0 %}
    texture(sampler2D(texture_table[{{texture_index}}]), vec2(tex_coord.s, 1.0 - tex_coord.t))
{% else if uv_source == 1 %}
    texture(sampler2D(texture_table[{{texture_index}}]), vec2(tex_coord.s * {{reciprocal_width}}, 1.0 - (tex_coord.t * {{reciprocal_height}})))
{% else if uv_source == 2 %}
    texture(sampler2D(texture_table[{{texture_index}}]), vec2({{tex_coord}}.x, 1.0 - {{tex_coord}}.y))
{% endif %}
