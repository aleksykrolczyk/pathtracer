#include <random>
#include "swIntersection.h"
#include "CONSTS.h"

float uniform() {
    // Will be used to obtain a seed for the random number engine
    static std::random_device rd;
    // Standard mersenne_twister_engine seeded with rd()
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    return dis(gen);
}

swVec3 getRandomRayDir(swVec3 vec) {
    // randomly sampled from hemisphere directed as vec
    while(true) {
        float phi = uniform() * 2.0f * PI;
        float theta = uniform() * PI;
        auto cart_vec = swVec3(
          cos(phi) * sin(theta),
          sin(phi) * sin(theta),
          cos(theta)
        );

        if (vec * cart_vec > 0) {
            return cart_vec;
        }
    }
}

//swVec3 getRandomCosineWeightedRay(swVec3 vec) {
//    // randomly sampled from hemisphere directed as vec
//    while(true) {
//        float phi = uniform() * 2.0f * PI;
//        float theta = uniform() * PI;
//
//    }
//}

swRay swIntersection::getRandomRay() const {
    return {mPosition, getRandomRayDir(mNormal), 0.0f, 0.01f, FLT_MAX};
}

swRay swIntersection::getShadowRay(const swVec3 &L) const {
    return {mPosition, L, 0.0f, 0.01f, FLT_MAX};
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
