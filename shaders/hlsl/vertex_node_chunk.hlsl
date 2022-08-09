{% if type == 0 %}
    input.vertex_position{{swizzle}}
{% else if type == 1 %}
    input.normal{{swizzle}}
{% else if type == 2 %}
    input.tex_coord{{swizzle}}
{% endif %}