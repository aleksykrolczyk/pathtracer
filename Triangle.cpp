//
// Created by Adam Korytowski on 11/12/2021.
//

#include <iostream>
#include "Triangle.h"
bool Triangle::intersect(const swRay &r, swIntersection &isect) {
//    auto nr = n * r.dir;
//    auto t = (n * r.orig + d) / nr;
////    if (t < 0)
////        return false; // triangle is behind
//
//    auto q = r.orig + t * r.dir;
//
////    if (t < r.minT || t > r.maxT)
////        return false;
//
//    swVec3 C;
//
//    C = (v1 - v0) % (q - v0);
//    if (n * C < 0) return false;
//
//    C = (v2 - v1) % (q - v1);
//    if (n * C < 0) return false;
//
//    C = (v0 - v2) % (q - v2);
//    if (n * C < 0) return false;

//    auto rr = q - v0;
//    auto ln = n.length();
//    auto v = (e1 % rr).length()/ln;
//    auto w = (rr % e2).length()/ln;
//
//    if (v < 0 || w < 0 || v + w >= 1)
//        return false;

    // TODO: cleanup, optimize, remove artifacts etc.

    auto rr = r.orig - v0;
    auto s = -r.dir * n;
    auto t = rr * n / s;
    auto v = -r.dir * (rr % e2) / s;
    if (t < r.minT || t > r.maxT)
        return false;
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

Triangle::Triangle(const swVec3 &vtx1, const swVec3 &vtx2, const swVec3 &vtx3,
                   const swMaterial &material) {

    this->v0 = vtx1;
    this->v1 = vtx2;
    this->v2 = vtx3;
    this->mMaterial = material;
    this->e1 = v1 - v0;
    this->e2 = v2 - v0;
    this->n = e1 % e2;
    this->d = n * v0;
}
