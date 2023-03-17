{% if type == 0 %}
    gl_Position{{swizzle}}
{% else if type == 1 %}
    view_position{{swizzle}}
{% else if type == 2 %}
    fragment_pos{{swizzle}}
{% else if type == 3 %}
    tex_coord{{swizzle}}
{% else if type == 4 %}
    normal{{swizzle}}
{% endif %}