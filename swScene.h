#pragma once
#include "swIntersection.h"
#include "swSphere.h"
#include <vector>

class swScene {
  public:
    // TODO: this takes much longer when it's pointer instead of const ref ://
    void push(Intersectable* s) { mObjs.push_back(s); }
    bool intersect(const swRay &r, swIntersection &isect, bool any = false);

  private:
    std::vector<Intersectable*> mObjs;
};
