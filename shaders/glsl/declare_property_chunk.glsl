layout (std430, binding = 5) buffer Properties
{
{% for property in properties %}
    {% if property.1 == 0 %}
    float property_{{property.0}};
    {% endif %}
{% endfor %}
    float _dummy;
};
