#include <chrono>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include "core/exception.h"
#include "core/root.h"
#include "core/vector3.h"
#include "jobs/job_system.h"
#include "log/log.h"
#include "platform/start.h"

struct Sphere;

// helpful globals
std::random_device rd;
std::mt19937 generator(rd());
static std::vector<Sphere> scene;

/**
 * Simple ray class.
 */
struct Ray
{
    Ray(const iris::Vector3 &origin, const iris::Vector3 &direction)
        : origin(origin)
        , direction(direction)
    {
    }

    iris::Vector3 origin;
    iris::Vector3 direction;
};

/**
 * Simple sphere class.
 */
struct Sphere
{
    Sphere(
        const iris::Vector3 &origin,
        float radius,
        const iris::Vector3 &colour)
        : origin(origin)
        , radius(radius)
        , colour(colour)
    {
    }

    std::tuple<float, iris::Vector3, iris::Vector3> intersects(
        const Ray &ray) const
    {
        auto L = origin - ray.origin;
        auto tca = L.dot(ray.direction);
        auto d2 = L.dot(L) - tca * tca;
        if (d2 > radius * radius)
        {
            return {std::numeric_limits<float>::max(), {}, {}};
        }

        auto thc = sqrtf(radius * radius - d2);
        auto t0 = tca - thc;
        auto t1 = tca + thc;
        if (t0 < 0)
        {
            t0 = t1;
        }

        if (t0 < 0)
        {
            return {std::numeric_limits<float>::max(), {}, {}};
        }

        auto q = ray.origin + (ray.direction * t0);
        return {t0, q, iris::Vector3::normalise(q - origin)};
    }

    iris::Vector3 origin;
    float radius;
    iris::Vector3 colour;
    bool is_metal = false;

    float rougness = 0.0f;
};

iris::Vector3 random_unit_vector()
{
    std::uniform_real_distribution<float> dist1(0.0f, 1.0f);
    float z = dist1(generator) * 2.0f - 1.0f;
    float a = dist1(generator) * 2.0f * M_PI;
    float r = sqrtf(1.0f - z * z);
    float x = r * cosf(a);
    float y = r * sinf(a);
    return {x, y, z};
}

iris::Vector3 random_in_unit_sphere()
{
    std::uniform_real_distribution<float> dist1(0.0f, 1.0f);
    iris::Vector3 p;
    do
    {
        p =
            iris::Vector3{
                dist1(generator), dist1(generator), dist1(generator)} *
                2.0 -
            iris::Vector3(1, 1, 1);
    } while (p.dot(p) >= 1.0);
    return p;
}

/**
 * Recursively trace a ray through a scene, to a max depth.
 */
iris::Vector3 trace(const Ray &ray, int depth)
{
    const Sphere *hit = nullptr;
    auto distance = std::numeric_limits<float>::max();
    iris::Vector3 point;
    iris::Vector3 normal;

    for (const auto &shape : scene)
    {
        const auto &[d, p, n] = shape.intersects(ray);

        if (d < distance)
        {
            distance = d;
            point = p;
            normal = n;
            hit = &shape;
        }
    }

    // return sky colour if we don't hit anything
    if (hit == nullptr)
    {
        return {0.9, 0.9, 0.9};
    }

    const auto emittance = hit->colour;

    // return black if we hit max depth
    if (depth > 4)
    {
        return {0.0, 0.0, 0.0};
    }

    Ray newRay{{}, {}};

    // create another ray to trace, based on material
    if (hit->is_metal)
    {
        auto reflect = ray.direction - normal * ray.direction.dot(normal) * 2;
        newRay = {
            point,
            iris::Vector3::normalise(
                reflect + random_in_unit_sphere() * hit->rougness)};
        newRay.origin += newRay.direction * 0.01f;
    }
    else
    {
        auto target = point + normal + random_unit_vector();

        newRay = {point, iris::Vector3::normalise(target - point)};
        newRay.origin += newRay.direction * 0.001f;
    }

    // trace next ray and mix in its colour
    return emittance * trace(newRay, depth + 1);
}

void go(int, char **)
{
    scene.emplace_back(
        iris::Vector3{150.0f, 0.0f, -600.0f},
        100.0f,
        iris::Vector3{0.58, 0.49, 0.67});
    scene.emplace_back(
        iris::Vector3{-150.0f, 0.0f, -600.0f},
        100.0f,
        iris::Vector3{0.99, 0.78, 0.84});
    scene.emplace_back(
        iris::Vector3{00.0f, 0.0f, -750.0f},
        100.0f,
        iris::Vector3{1.0, 0.87, 0.82});
    scene.emplace_back(
        iris::Vector3{0.0f, -10100.0f, -600.0f},
        10000.0f,
        iris::Vector3{1, 1, 1});

    scene[2].is_metal = true;
    scene[2].rougness = 0.9;

    static const auto width = 600;
    static const auto height = 400;
    std::vector<std::uint8_t> pixels(width * height * 3);
    std::size_t counter = 0u;

    const float fov = M_PI / 3.;
    std::uniform_real_distribution<float> dist1(-0.5f, 0.5f);

    std::vector<iris::Job> jobs;

    auto start = std::chrono::high_resolution_clock::now();

    for (std::size_t j = 0; j < height; j++)
    {
        for (std::size_t i = 0; i < width; i++)
        {
            jobs.emplace_back([i, j, fov, counter, &pixels, &dist1]() {
                const auto dir_x = (i + 0.5f) - width / 2.0f;
                const auto dir_y = -(j + 0.5f) + height / 2.0f;
                const auto dir_z = -height / (2.0f * tan(fov / 2.0f));

                iris::Vector3 pixel;

                auto samples = 100;

                for (int i = 0; i < samples; i++)
                {
                    pixel += trace(
                        {{0, 0, 0},
                         iris::Vector3::normalise(
                             {dir_x + dist1(generator),
                              dir_y + dist1(generator),
                              dir_z})},
                        1);
                }

                pixel *= (1.0 / (float)samples);

                // clamp colours
                pixels[counter + 0u] = static_cast<std::uint8_t>(
                    (255.0f * std::max(0.0f, std::min(1.0f, (float)pixel.x))));
                pixels[counter + 1u] = static_cast<std::uint8_t>(
                    (255.0f * std::max(0.0f, std::min(1.0f, (float)pixel.y))));
                pixels[counter + 2u] = static_cast<std::uint8_t>(
                    (255.0f * std::max(0.0f, std::min(1.0f, (float)pixel.z))));
            });

            counter += 3u;
        }
    }

    iris::JobSystem::wait_for_jobs(jobs);

    auto end = std::chrono::high_resolution_clock::now();

    LOG_INFO(
        "job_sample",
        "render time: {}ms",
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count());

    stbi_write_png("render.png", width, height, 3, pixels.data(), 3 * width);

    LOG_INFO("job_sample", "done");
}

int main(int argc, char **argv)
{
    iris::start(argc, argv, go);

    return 0;
}
