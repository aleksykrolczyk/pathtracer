//
// Created by Adam Korytowski on 16/12/2021.
//

#ifndef RAYTRACER_SMOOTHTRIANGLE_H
#define RAYTRACER_SMOOTHTRIANGLE_H

#include "Triangle.h"
class SmoothTriangle : public Triangle
{
  public:
    SmoothTriangle() = default;
    SmoothTriangle(const swVec3 &vtx1, const swVec3 &vtx2, const swVec3 &vtx3,
                   swVec3 &norm1, swVec3 &norm2, swVec3 &norm3, const swMaterial &material);

    bool intersect(const swRay &r, swIntersection &isect) final;


  public:
    swVec3 n0;
    swVec3 n1;
    swVec3 n2;

};

#endif // RAYTRACER_SMOOTHTRIANGLE_H
