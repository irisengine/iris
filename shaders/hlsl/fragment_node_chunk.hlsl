{% if type == 0 %}
    input.position{{swizzle}}
{% else if type == 1 %}
    input.view_position{{swizzle}}
{% else if type == 2 %}
    input.fragment_position{{swizzle}}
{% else if type == 3 %}
    input.tex_coord{{swizzle}}
{% else if type == 4 %}
    input.normal{{swizzle}}
{% endif %}