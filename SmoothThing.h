//
// Created by Adam Korytowski on 16/12/2021.
//

#ifndef RAYTRACER_SMOOTHTHING_H
#define RAYTRACER_SMOOTHTHING_H

#include "SmoothTriangle.h"
#include "Mat3.h"
class SmoothThing : public Intersectable
{
  public:
    ~SmoothThing() {
        delete [] triangles;
    }

    SmoothThing(const float* vertices, const float* normals, unsigned int vertexCount, const Mat3 &transform, const swVec3 &transpose, const swMaterial &material );

    bool intersect(const swRay &r, swIntersection &isect) final;

  public:
    unsigned int triangleCount;
    SmoothTriangle* triangles;
    Triangle box[12]{};
};

#endif // RAYTRACER_SMOOTHTHING_H
