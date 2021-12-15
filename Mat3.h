//
// Created by Adam Korytowski on 15/12/2021.
//

#ifndef RAYTRACER_MAT3_H
#define RAYTRACER_MAT3_H

#include "swVec3.h"
class Mat3 {
  public:
    Mat3() = default;
    Mat3(swVec3 &v0, swVec3 &v1, swVec3 &v2)
      : m{v0,v1,v2}{}

    swVec3 operator*(swVec3 &v) const {
        return {v*m[0], v*m[1], v*m[2]};
    }

    Mat3 operator*(Mat3 &mat) const {
        swVec3 mm0(mat.m[0].x(), mat.m[1].x(), mat.m[2].x());
        swVec3 mm1(mat.m[0].y(), mat.m[1].y(), mat.m[2].y());
        swVec3 mm2(mat.m[0].z(), mat.m[1].z(), mat.m[2].z());
        swVec3 v0(m[0] * mm0, m[0] * mm1, m[0] * mm2);
        swVec3 v1(m[1] * mm0, m[1] * mm1, m[1] * mm2);
        swVec3 v2(m[2] * mm0, m[2] * mm1, m[2] * mm2);
        return { v0, v1, v2 };
    }

  public:
    swVec3 m[3];
};


#endif // RAYTRACER_MAT3_H
