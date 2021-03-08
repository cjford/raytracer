#include "v3.h"

#include <math.h>

float V3::magnitude() {
    return sqrt(x * x + y * y + z * z);
}

float V3::dot(V3 v) {
    return (v.x * x) + (v.y * y) + (v.z * z);
}

V3 V3::reflect(V3 normal) {
    float n_dot = normal.dot(*this);

    return normal * 2.0f * n_dot - *this;
}

V3 V3::normalized() {
    return *this / this->magnitude();
}

V3 V3::operator*(float i) {
    return {x * i, y * i, z * i};
}

V3 V3::operator*(V3 v) {
    return {x * v.x, y * v.y, z * v.z};
}

V3 V3::operator+(float i) {
    return {x + i, y + i, z + i};
}

V3 V3::operator+(V3 v) {
    return {x + v.x, y + v.y, z + v.z};
}

V3 V3::operator-(float i) {
    return {x - i, y - i, z - i};
}

V3 V3::operator-(V3 v) {
    return {x - v.x, y - v.y, z - v.z};
}

V3 V3::operator-() {
    return {-x, -y, -z};
}

V3 V3::operator/(float i) {
    return {x / i, y / i, z / i};
}

V3 V3::operator/(V3 v) {
    return {x / v.x, y / v.y, z / v.z};
}
