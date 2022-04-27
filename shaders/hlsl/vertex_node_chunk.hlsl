{% if type == 0 %}
    input.vertex_position{{swizzle}}
{% else %}
    input.normal{{swizzle}}
{% endif %}