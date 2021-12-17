//
// Created by Adam Korytowski on 15/12/2021.
//

#ifndef RAYTRACER_THING_H
#define RAYTRACER_THING_H

#include "Intersectable.h"
#include "Mat3.h"
#include "Triangle.h"
class Thing : public Intersectable
{
  public:
    ~Thing() {
        delete [] triangles;
    }

    Thing(const float* vertices, unsigned int vertexCount, const Mat3 &transform, const swVec3 &transpose, const swMaterial &material );

    bool intersect(const swRay &r, swIntersection &isect) final;

  public:
    unsigned int triangleCount;
    Triangle* triangles;
    Triangle box[12]{};

};

#endif // RAYTRACER_THING_H
