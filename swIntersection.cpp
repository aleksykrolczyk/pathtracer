#include "swIntersection.h"

swRay swIntersection::getShadowRay(const swVec3 &L) {
    return swRay(mPosition, L, 0.0f, 0.01f, FLT_MAX);
}

swRay swIntersection::getReflectedRay(void) {
    swVec3 N = mNormal;
    const swVec3 D = mRay.dir;

    // TODONE: Implement reflection.
    // -------------------
    //    R = V - 2(N . V)N
    swVec3 R = D - 2 * (N * D) * N;
    // -------------------
    swRay rray = swRay(mPosition, R, 0.0f, 0.01f, FLT_MAX);
    return rray;
}

swRay swIntersection::getRefractedRay(void) {
    const swVec3 &D = mRay.dir;
    swVec3 N = mNormal;
    swVec3 O; // O : outgoing refracted vector
    float eta = 1.0f / mMaterial.refractiveIndex;
    if (!mFrontFacing) eta = 1.0f / eta; // Inside material.

    // TODONE: Implement reflection.
    // -------------------
    float r = (-1 * D) * N;
    float c = 1 - eta * eta * (1 - r * r);

    O = (eta * D) + (eta * r - sqrt(c)) * N;
    // -------------------

    return swRay(mPosition, O, 0.0f, 0.01f, FLT_MAX);
}
