{% if type == 0 %}
float {{name}} = {{value}};
{% else if type == 1 %}
vec3 {{name}} = {{value}};
{% else if type == 2 %}
vec4 {{name}} = {{value}};
{% endif %}