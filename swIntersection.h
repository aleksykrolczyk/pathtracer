#pragma once
#include "swMaterial.h"
#include "swRay.h"

class swIntersection {
  public:
    swRay getShadowRay(const swVec3 &L, float d) const;
    swRay getReflectedRay(void) const;
    swRay getRefractedRay(void) const;
    swRay getRandomRay(void) const;
  public:
    float mHitTime{FLT_MAX};
    swVec3 mPosition;
    swVec3 mNormal;
    bool mFrontFacing{true};
    swMaterial mMaterial;
    swRay mRay; // incoming ray that creates intersection
};
