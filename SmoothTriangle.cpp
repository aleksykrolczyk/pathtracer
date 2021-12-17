//
// Created by Adam Korytowski on 16/12/2021.
//

#include "SmoothTriangle.h"

#define EPSILON 0.001

SmoothTriangle::SmoothTriangle(const swVec3 &vtx1, const swVec3 &vtx2,
                               const swVec3 &vtx3, swVec3 &norm1, swVec3 &norm2,
                               swVec3 &norm3, const swMaterial &material)
  : Triangle(vtx1, vtx2, vtx3, material) {
    this->n0 = norm1;
    this->n1 = norm2;
    this->n2 = norm3;
}

bool SmoothTriangle::intersect(const swRay &r, swIntersection &isect) {
    auto rr = r.orig - v0;
    auto s = -r.dir * n;
    if (fabs(s) < EPSILON) {
        return false;
    }
    auto t = rr * n / s;
    if (t < r.minT || t > r.maxT)
        return false;
    auto v = -r.dir * (rr % e2) / s;
    auto w = -r.dir * (e1 % rr) / s;
    if (v < 0 || w < 0 || v + w >= 1)
        return false;
    auto q = r.orig + t * r.dir;

    isect.mHitTime = t;
    isect.mNormal = (1 - v - w) * n0 + v * n1 + w * n2;
    isect.mNormal.normalize();
    isect.mFrontFacing = (-r.dir * isect.mNormal) > 0.0f;
    if (!isect.mFrontFacing) isect.mNormal = -isect.mNormal;
    isect.mPosition = q;
    isect.mMaterial = mMaterial;
    isect.mRay = r;
    return true;
}
