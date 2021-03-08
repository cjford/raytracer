#include "trace_ray.h"

const char* LIGHT_TYPE_AMBIENT     = "ambient";
const char* LIGHT_TYPE_POINT       = "point";
const char* LIGHT_TYPE_DIRECTIONAL = "directional";

// Scene data
static const Sphere s1 = {{0, -1, 3}, 1, 500.0, 0.3, {255, 0, 0}};
static const Sphere s2 = {{1, 2, 8}, 3, 500.0, 0.5, {0, 0, 255}};
static const Sphere s3 = {{-2, 0, 4}, 1, 10.0, 0.4, {0, 255, 0}};
static const Sphere s4 = {{0, -5001, 0}, 5000, 1000.0, 0.5, {255, 255, 0}};

static const Light ambient = {LIGHT_TYPE_AMBIENT, 0.2};
static const Light point = {LIGHT_TYPE_POINT, 0.6, {2, 1, 0}};
static const Light point2 = {LIGHT_TYPE_POINT, 0.3, {0, 1, 8}};
static const Light direct = {LIGHT_TYPE_DIRECTIONAL, 0.2, {1, 4, 4}};

static std::vector<Light> lights = {point, direct, ambient, point2};
static std::vector<Sphere> spheres = {s2, s1, s3, s4};

// Use quadratic formula to solve for intersection points of ray and sphere surface
// Can intersect when entering and exiting the sphere, thus a pair of possible solutions
std::pair<float, float> intersect_ray_sphere(V3 origin, V3 ray, Sphere s) {
    V3 CO = origin - s.center;
    std::pair<float, float> intersect_offsets;
    float a = ray.dot(ray);
    float b = 2.0 * CO.dot(ray);
    float c = CO.dot(CO) - (s.radius * s.radius);
    float discriminant = (b * b) - (4 * a * c);

    if (discriminant <= 0) {
        intersect_offsets.first = 0;
        intersect_offsets.second = 0;
    } else {
        intersect_offsets.first = (-b + sqrt(discriminant)) / (2*a);
        intersect_offsets.second = (-b - sqrt(discriminant)) / (2*a);
    }

    return intersect_offsets;
}

// Given intersections calculated by intersect_ray_sphere, we only care
// about the closest one, as the futher one is behind the sphere. We also
// return a reference to the sphere itself, so we can use its normals and color.
Intersection closest_intersection(V3 origin, V3 ray, float min_offset, float max_offset) {
    Sphere *closest_sphere = NULL;
    float closest_intersect = max_offset;

    for(int i = 0; i < spheres.size(); i++) {
        std::pair<float, float> intersects = intersect_ray_sphere(origin, ray, spheres[i]);

        if(intersects.first > min_offset && intersects.first < closest_intersect) {
            closest_intersect = intersects.first;
            closest_sphere = &spheres[i];
        }
            if(intersects.second > min_offset && intersects.second < closest_intersect) {
            closest_intersect = intersects.second;
            closest_sphere = &spheres[i];
        }

    }

    return {closest_intersect, closest_sphere};
}

// For a given point on sphere surface, calculate lighting hiting that point, which is used to modify local color
float lighting_intensity(V3 target_point, V3 normal, V3 view_ray, float specular_factor, float max_offset) {
    float intensity = 0.0;
    V3 light_ray;

    for(int i = 0; i < lights.size(); i++) {
        // Ambient light is applied evenly everywhere, flat increase to intensity value
        if (lights[i].type == LIGHT_TYPE_AMBIENT) {
            intensity += lights[i].intensity;
        } else {
            if (lights[i].type == LIGHT_TYPE_POINT) {
                light_ray = lights[i].position - target_point;
            } else { // type == LIGHT_TYPE_DIRECTIONAL
                light_ray = lights[i].position;
            }

            // If a vector from target_point to the light intersects another sphere,
            // ignore that light's intensity as the point is in shadow. Use 0.01
            // min_offset to avoid intersecting with self.
            Intersection closest_result = closest_intersection(target_point, light_ray, 0.01, max_offset);
            if (closest_result.closest_sphere != NULL) {
                continue;
            }

            float n_dot_l = normal.dot(light_ray);
            if (n_dot_l > 0) {
                intensity += lights[i].intensity * n_dot_l / (normal.magnitude() * light_ray.magnitude());
            }

            // Specular lighting can come from point or directional sources.
            // The greater the specular_factor (i.e. "shininess"), the larger amount of light is reflected
            // back to view_ray and the higher intensity we see.
            if (specular_factor != -1) {
                V3 reflect_ray = light_ray.reflect(normal);

                float dividend = reflect_ray.dot(view_ray);
                float divisor = reflect_ray.magnitude() * view_ray.magnitude();
                float quotient = dividend / divisor;

                if (dividend > 0.0) {
                    float in =  pow((dividend / divisor), specular_factor);
                    in = lights[i].intensity * in;
                    intensity += in;
                }
            }
        }
    }
    return intensity;
}


// Combine all lighting and reflections to give us a final color to render. The ray here will be
// constructed depending on which pixel of the canvas we're currently rendering, wrt a camera point (origin)
Color trace_ray(V3 origin, V3 ray, float min_offset, float max_offset, int reflect_depth) {
    Sphere *closest_sphere = NULL;
    float closest_intersect = max_offset;

    Intersection intersect = closest_intersection(origin, ray, min_offset, max_offset);
    closest_sphere = intersect.closest_sphere;
    closest_intersect = intersect.ray_offset;

    if (!closest_sphere) {
        // Nothing was hit, return background color
        return {0, 0, 0};
    } else {
        V3 intersect_point =  ray * closest_intersect + origin;
        V3 sphere_normal = (intersect_point - closest_sphere->center).normalized();

        float lighting = lighting_intensity(
            intersect_point,
            sphere_normal,
            -ray,
            closest_sphere->specular_factor,
            max_offset
        );

        Color local_color = {
            closest_sphere->color.red * lighting,
            closest_sphere->color.green * lighting,
            closest_sphere->color.blue * lighting,
        };

        float reflect_factor = closest_sphere->reflect_factor;
        if (reflect_factor <= 0.0 || reflect_depth <= 0) {
            return local_color;
        }

        V3 reflect_ray = -ray.reflect(sphere_normal);

        // Trace a ray back out from this point to see what it hits, thus what color is reflected.
        // Do this recursively up to finite reflect_depth, to avoid infinite mirror effect
        Color reflected_color = trace_ray(
            intersect_point,
            reflect_ray,
            0.01,
            10000,
            reflect_depth - 1
        );

        // Blend together local color (with lighting) and reflected color,
        // weighted by reflect_factor (i.e. "reflectiveness")
        return local_color.blend(reflected_color, reflect_factor);
    }
}
