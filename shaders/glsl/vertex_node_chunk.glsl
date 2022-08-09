{% if type == 0 %}
    vertex_pos{{swizzle}}
{% else if type == 1 %}
    norm{{swizzle}}
{% else if type == 2%}
    vec3(tex_coord{{swizzle}}, 0.0)
{% endif %}