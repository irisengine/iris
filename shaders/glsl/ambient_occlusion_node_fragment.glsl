in vec2 tex_coord;
in vec3 tangent_frag_pos;
in vec3 tangent_light_pos;
in vec3 tangent_view_pos;
in vec4 col;
in vec4 frag_pos;
in vec4 frag_pos_light_space;
in vec4 norm;
in vec4 vertex_pos;
in vec4 view_norm;
in vec4 view_position;

layout (location = 0) out vec4 out_colour;

layout (std140, binding = 0) uniform CameraData
{
    mat4 projection;
    mat4 view;
    mat4 normal_view;
    vec3 camera;
};

layout (std140, binding = 2) uniform LightData
{
    mat4 light_projection;
    mat4 light_view;
    vec4 light_colour;
    vec4 light_position;
    vec4 light_attenuation;
};

layout (std430, binding = 3) buffer TextureData
{
    uvec2 texture_table[];
};

layout (std430, binding = 4) buffer CubeMapData
{
    uvec2 cube_map_table[];
};

uniform int shadow_map_index;

void main()
{
    const vec4 fragment_colour = {{ fragment_colour }};

    const vec3 samples[] = {
        vec3(-0.00886638, 0.00229353, 0.0231249), vec3(-0.000634795, 0.0456608, 0.0796033),
        vec3(0.00276396, 0.038391, 0.051785), vec3(0.000758422, 0.0368439, 0.0226108),
        vec3(-0.0210339, -0.0251614, 0.00471135), vec3(-0.0077157, 0.0156199, 0.0450549),
        vec3(-0.0539939, 0.0648841, 0.0591181), vec3(0.072815, -0.0666652, 0.00620315),
        vec3(-0.00919381, -0.00449068, 0.00473938), vec3(-0.104514, -0.0203993, 0.00417302),
        vec3(-0.109165, -0.0305201, 0.0141049), vec3(0.0424682, 0.0210869, 0.0302346),
        vec3(0.0159805, -0.00986693, 1.3074e-05), vec3(0.000889632, 0.00126402, 0.0648331),
        vec3(-0.0785, -0.0148912, 0.0617556), vec3(0.0810225, -0.0301, 0.0296346),
        vec3(0.0905632, -0.0689013, 0.0126851), vec3(0.0414047, 0.0489806, 0.0333673),
        vec3(-0.0111549, -0.0129772, 0.00831587), vec3(-0.0327198, 0.023108, 0.0101144),
        vec3(-0.0248668, -0.0307488, 0.0313254), vec3(-0.00400038, -0.0168738, 0.00877024),
        vec3(0.16404, -0.0637624, 0.0455338), vec3(0.0734847, 0.0766196, 0.0272843),
        vec3(0.0871256, 0.0227383, 0.00885513), vec3(0.0191078, -0.00586284, 0.0375503),
        vec3(-0.00139819, -0.00164079, 0.00227057), vec3(0.137097, -0.105593, 0.0516416),
        vec3(-0.0115462, -0.00991795, 0.0116921), vec3(0.0147203, -0.0119939, 0.00783621),
        vec3(0.0377106, 0.0794236, 0.158526), vec3(-0.205867, 0.102649, 0.00129098),
        vec3(0.0067215, -0.00561246, 0.00643123), vec3(0.0727398, -0.0334538, 0.163939),
        vec3(0.0804173, -0.0935365, 0.0692903), vec3(0.0519014, 0.276055, 0.172247),
        vec3(-0.350498, 0.154156, 0.0152109), vec3(-0.0765435, -0.184808, 0.0878499),
        vec3(0.0229437, -0.337212, 0.227193), vec3(0.245147, -0.0672405, 0.174766),
        vec3(-0.269699, 0.218419, 0.277733), vec3(-0.0226895, -0.0909222, 0.0906554),
        vec3(-0.138153, 0.0324949, 0.400103), vec3(0.0382081, -0.107126, 0.103821),
        vec3(0.172096, 0.0158331, 0.142202), vec3(-0.275107, -0.424418, 0.0449334),
        vec3(-0.00638451, 0.235465, 0.212274), vec3(0.249825, -0.34683, 0.367554),
        vec3(0.310481, 0.225706, 0.38594), vec3(0.00561876, 0.0713732, 0.16017),
        vec3(-0.0805308, -0.0039723, 0.00282647), vec3(-0.0165996, -0.000963676, 0.0139403),
        vec3(0.371335, 0.376646, 0.333897), vec3(-0.0242451, 0.0228724, 0.0762239),
        vec3(0.0868034, 0.0415974, 0.140887), vec3(-0.0342928, 0.0116279, 0.0615761),
        vec3(0.45093, -0.0762613, 0.16263), vec3(-0.0300036, 0.00125506, 0.0126724),
        vec3(0.0579732, -0.0307036, 0.0701422), vec3(0.474882, 0.536485, 0.364894),
        vec3(0.0534704, -0.0184046, 0.0217006), vec3(-0.626959, 0.290028, 0.17594),
        vec3(0.0670969, -0.0771161, 0.0286401), vec3(0.732737, -0.580259, 0.00029956)
    };
    const vec3 noise_data[] = {
        vec3(-0.595906, -0.156549, 0.0f), vec3(-0.631204, -0.049680, 0.0f),
        vec3(0.850768, -0.577690, 0.0f), vec3(-0.576086, 0.813602, 0.0f),
        vec3(0.585386, 0.520160, 0.0f), vec3(0.542889, -0.034259, 0.0f),
        vec3(-0.338602, -0.899637, 0.0f), vec3(0.298322, -0.040603, 0.0f),
        vec3(-0.345729, 0.861361, 0.0f), vec3(0.729817, -0.604520, 0.0f),
        vec3(0.258031, 0.804646, 0.0f), vec3(0.487633, -0.194988, 0.0f),
        vec3(-0.104914, -0.366919, 0.0f), vec3(-0.478404, 0.212103, 0.0f),
        vec3(-0.598677, -0.779258, 0.0f), vec3(0.837367, 0.120391, 0.0f)
    };
    
    const vec2 size = vec2({{width}}.f, {{height}}.f);
    const vec2 uv = vec2(tex_coord.x, 1.0 - tex_coord.y);
    const vec3 frag_pos = texture(sampler2D(texture_table[{{position_texture_index}}]), uv).xyz;
    const vec3 normal = normalize(texture(sampler2D(texture_table[{{normal_texture_index}}]), uv).rgb);

    const int sample_count = {{sample_count}};
    const float radius = {{radius}};
    const float bias = {{bias}};

    const int x = int(uv.x * size.x) % 4;
    const int y = int(uv.y * size.y) % 4;
    const int index = (y * 4) + x;
    const vec3 rand = normalize(noise_data[index]);

    const vec3 tangent = normalize(rand - normal * dot(rand, normal));
    const vec3 bitangent = cross(normal, tangent);
    const mat3 tbn = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;

    for (int i = 0; i < sample_count; ++i)
    {
        vec3 sample_pos = tbn * samples[i];
        sample_pos = frag_pos + sample_pos * radius; 

        vec4 offset = transpose(projection) * vec4(sample_pos, 1.0);
        offset.xy /= offset.w;
        offset.x = offset.x * 0.5f + 0.5f;
        offset.y = -offset.y * 0.5f + 0.5f;

        const vec3 sample_normal = normalize(texture(sampler2D(texture_table[{{normal_texture_index}}]), offset.xy).rgb);
        if (dot(normal, sample_normal) > 0.99f)
        {
            continue;
        }

        const float sample_depth =
            texture(sampler2D(texture_table[{{position_texture_index}}]), vec2(offset.x, 1.0 - offset.y)).z;

        const float range_check = smoothstep(0.0, 1.0, radius / abs(frag_pos.z - sample_depth));
        occlusion += (sample_depth >= sample_pos.z + bias ? 1.0 : 0.0) * range_check;           
    }

    occlusion = 1.0 - (occlusion / sample_count);
    
    out_colour = light_colour * fragment_colour * vec4(occlusion, occlusion, occlusion, 1.0f);
}