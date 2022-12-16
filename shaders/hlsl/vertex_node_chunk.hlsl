{% if is_vertex_shader %}
    {% if type == 0 %}
        result.vertex_position{{swizzle}}
    {% else if type == 1 %}
        result.normal{{swizzle}}
    {% else if type == 2 %}
        result.tex_coord{{swizzle}}
    {% endif %}
{% else %}
    {% if type == 0 %}
        input.vertex_position{{swizzle}}
    {% else if type == 1 %}
        input.normal{{swizzle}}
    {% else if type == 2 %}
        input.tex_coord{{swizzle}}
    {% endif %}
{% endif %}