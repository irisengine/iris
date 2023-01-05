
/* DEBUG: {{type}} */

{% if type == 0 %}
    position{{swizzle}}
{% else if type == 1 %}
    normal{{swizzle}}
{% else if type == 2 %}
    tex_coord{{swizzle}}
{% endif %}