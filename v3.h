#ifndef V3_H
#define V3_H

class V3 {

    public:

    float x;
    float y;
    float z;

    float magnitude();

    float dot(V3 v);

    V3 reflect(V3 normal);

    V3 normalized();

    V3 operator*(float i);

    V3 operator*(V3 v);

    V3 operator+(float i);

    V3 operator+(V3 v);

    V3 operator-(float i);

    V3 operator-(V3 v);

    V3 operator-();

    V3 operator/(float i);

    V3 operator/(V3 v);
};


#endif
