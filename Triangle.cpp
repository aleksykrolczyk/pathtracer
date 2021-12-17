//
// Created by Adam Korytowski on 11/12/2021.
//

#include <iostream>
#include "Triangle.h"

#define EPSILON 0.001

bool Triangle::intersect(const swRay &r, swIntersection &isect) {

    // TODO: cleanup, optimize

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
    isect.mNormal = n;
    isect.mNormal.normalize();
    isect.mFrontFacing = (-r.dir * isect.mNormal) > 0.0f;
    if (!isect.mFrontFacing) isect.mNormal = -isect.mNormal;
    isect.mPosition = q;
    isect.mMaterial = mMaterial;
    isect.mRay = r;
    return true;
}

bool Triangle::intersect(const swRay &r) const {
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

    return true;
}

Triangle::Triangle(const swVec3 &vtx1, const swVec3 &vtx2, const swVec3 &vtx3,
                   const swMaterial &material) {

    this->v0 = vtx1;
    this->v1 = vtx2;
    this->v2 = vtx3;
    this->mMaterial = material;
    this->e1 = v1 - v0;
    this->e2 = v2 - v0;
    this->n = e1 % e2;
}
