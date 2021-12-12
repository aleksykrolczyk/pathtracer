//
// Created by Adam Korytowski on 11/12/2021.
//

#ifndef RAYTRACER_TRIANGLE_H
#define RAYTRACER_TRIANGLE_H

#include "swMaterial.h"
#include "swRay.h"
#include "swIntersection.h"
#include "Intersectable.h"

class Triangle : public Intersectable
{
  public:

    Triangle() = default;
    ~Triangle() = default;
    Triangle(const swVec3 &vtx1, const swVec3 &vtx2, const swVec3 &vtx3, const swMaterial &material);

    bool intersect(const swRay &r, swIntersection &isect) final;

  public:
    swVec3 v0;
    swVec3 v1;
    swVec3 v2;
    swMaterial mMaterial;

    swVec3 e1;
    swVec3 e2;
    swVec3 n;
    float d{};
};

#endif // RAYTRACER_TRIANGLE_H
