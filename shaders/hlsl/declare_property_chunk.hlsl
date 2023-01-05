cbuffer Properties : register(b6)
{
{% for property in properties %}
    {% if property.1 == 0 %}
    float property_{{property.0}};
    {% endif %}
{% endfor %}
};
