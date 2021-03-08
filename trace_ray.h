#ifndef TRACE_RAY_H
#define TRACE_RAY_H

#include <vector>
#include <stdio.h>
#include <iostream>

#include "v3.h"
#include "color.h"

struct Sphere {
    V3 center;
    float radius;
    float specular_factor;
    float reflect_factor;
    Color color;
};

struct Light {
    std::string type;
    float intensity;
    V3 position;
};

struct Intersection {
    float ray_offset;
    Sphere *closest_sphere;
};

std::pair<float, float> intersect_ray_sphere(V3 origin, V3 ray, Sphere s);

Intersection closest_intersection(V3 origin, V3 ray, float min_offset, float max_offset);

float lighting_intensity(V3 target_point, V3 normal, V3 view_ray, float specular_factor, float max_offset);

Color trace_ray(V3 origin, V3 ray, float min_offset, float max_offset, int reflect_depth);

#endif
