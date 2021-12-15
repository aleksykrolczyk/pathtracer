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

void createCoordinateSystem(const swVec3 &N, swVec3 &Nt, swVec3 &Nb)
{
    if (std::fabs(N.x()) > std::fabs(N.y())) {
        Nt = swVec3(N.z(), 0, -N.x()) / sqrtf(N.x() * N.x() + N.z() * N.z());
    }
    else {
        Nt = swVec3(0, -N.z(), N.y()) / sqrtf(N.y() * N.y() + N.z() * N.z());
    }
    Nb = N % Nt;
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

swVec3 getRandomRayDir2(swVec3 vec)
{
    float u1 = uniform();
    float u2 = uniform();
    const double r = sqrt(1.0-u1*u1);
    const double phi = 2 * PI * u2;
    float x=cos(phi)*r, y=sin(phi)*r ,z = u1;

     swVec3 Nt, Nb;
     createCoordinateSystem(vec, Nt, Nb);

     return {
       x * Nb.x() + y * vec.x() + z * Nt.x(),
       x * Nb.y() + y * vec.y() + z * Nt.y(),
       x * Nb.z() + y* vec.z() + z * Nt.z()
     };
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
    return {mPosition, R, 0.0f, 0.01f, FLT_MAX};
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
    if(c < 0) {
        return getReflectedRay();
    }

    O = (eta * D) + (eta * r - sqrt(c)) * N;
    // -------------------

    return swRay(mPosition, O, 0.0f, 0.01f, FLT_MAX);
}
