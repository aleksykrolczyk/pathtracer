#pragma once
#include "swVec3.h"

class swMaterial {
  public:
    swMaterial() = default;
    swMaterial(const swVec3 &c, float r = 0, float t = 0, float i = 1, const swVec3 &e = {0.0f, 0.0f, 0.0f})
      : mColor{c}, reflectivity(r), transparency(t), refractiveIndex(i), emittance(e) {}

  public:
    swVec3 mColor;
    float reflectivity{0.0f};
    float transparency{0.0f};
    float refractiveIndex{1.0f};
    swVec3 emittance{0.0f, 0.0f, 0.0f};

    bool emits_light() const {
        return this->emittance.x() > 0 ||
               this->emittance.y() > 0 ||
               this->emittance.z() > 0;
    }
};
