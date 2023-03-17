{% if operator == 0 %}
    ({{value1}} + {{value2}})
{% else if operator == 1 %}
    ({{value1}} - {{value2}})
{% else if operator == 2 %}
    ({{value1}} * {{value2}})
{% else if operator == 3 %}
    ({{value1}} / {{value2}})
{% else if operator == 4 %}
    dot({{value1}}, {{value2}})
{% else if operator == 5 %}
    cross({{value1}}, {{value2}})
{% else if operator == 6 %}
    distance({{value1}}, {{value2}})
{% endif %}