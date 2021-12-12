//
// Created by Adam Korytowski on 11/12/2021.
//

#ifndef RAYTRACER_INTERSECTABLE_H
#define RAYTRACER_INTERSECTABLE_H

#include <memory>
#include "swIntersection.h"
class Intersectable {
  public:
    virtual bool intersect(const swRay &r, swIntersection &isect) = 0;
};

typedef std::unique_ptr<Intersectable> PIntersectable;


#endif // RAYTRACER_INTERSECTABLE_H
