/*
 *  main.cpp
 *  swTracer
 *
 *  Created by Michael Doggett on 2021-09-23.
 *  Copyright (c) 2021 Michael Doggett
 */
#define _USE_MATH_DEFINES
#include <cfloat>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>
#include <omp.h>
#include <memory>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "swCamera.h"
#include "swIntersection.h"
#include "swMaterial.h"
#include "swRay.h"
#include "swScene.h"
#include "swSphere.h"
#include "swVec3.h"
#include "Triangle.h"

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

const Color BLACK = Color(0, 0, 0);

inline float clamp(float x, float min, float max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

float uniform() {
    // Will be used to obtain a seed for the random number engine
    static std::random_device rd;
    // Standard mersenne_twister_engine seeded with rd()
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    return dis(gen);
}

void WriteColor(int index, swVec3 p, uint8_t *pixels) {
    // gamma correct for gamma=2.2, x^(1/gamma), more see :
    // https://www.geeks3d.com/20101001/tutorial-gamma-correction-a-story-of-linearity/
    for (int n = 0; n < 3; n++) {
        p.m[n] = pow(p.m[n], 1.0f / 2.2f);
        pixels[index + n] = (uint8_t)(256 * clamp(p.m[n], 0.0f, 0.999f));
    }
}

Color traceRay(const swRay &r, swScene scene, int depth) {
    Color c, directColor;
    if (depth < 0) return c;

    const swVec3 gLightPos(20, 240, -7);

    swIntersection hp, si;
    if (!scene.intersect(r, hp)) return BLACK; // Background color

    swVec3 lightDir = gLightPos - hp.mPosition;
    lightDir.normalize();

    bool is_illuminated = !scene.intersect(hp.getShadowRay(lightDir), si, true);
    directColor = is_illuminated ? hp.mMaterial.mColor : BLACK;
    directColor = directColor * (hp.mNormal * lightDir);

    float refl = hp.mMaterial.reflectivity;
    float trans = hp.mMaterial.transparency;

    Color rc, tc;
    if (refl > 0) {
        rc = traceRay(hp.getReflectedRay(), scene, depth - 1);
    }

    if (trans > 0) {
        tc = traceRay(hp.getRefractedRay(), scene, depth - 1);
    }

    c = (1 - refl - trans) * directColor + refl * rc + trans * tc;

    return c;
}

int main() {
    int imageWidth = 512;
    int imageHeight = imageWidth;
    const int numChannels = 3;
    uint8_t *pixels = new uint8_t[imageWidth * imageHeight * numChannels];

    // Setup scene
    swScene scene;
    swMaterial mat[4];
    // swMaterial(swVec3 color, reflectivity, refractivity, refractiveIndex)
    mat[0] = swMaterial(swVec3(0.0f, 0.2f, 0.9f), 0.0f, 0.7f, 1.01f);
    mat[1] = swMaterial(swVec3(1.0f, 0.3f, 0.2f), 0.7f, 0.0f, 1.00f);
    mat[2] = swMaterial(swVec3(0.0f, 0.7f, 0.1f), 0.3f, 0.3f, 1.20f);
    mat[3] = swMaterial(swVec3(0.6f, 0.6f, 0.6f), 0.5f, 0.0f, 1.00f);
    std::vector<PIntersectable> qqq;
    for (int i = 0; i < 2 * 2 * 2; i++) {
        scene.push(new swSphere(swVec3(
                                      (float)(-6 + 12 * (i & 1)),
                                      (float)(-6 + 12 * ((i >> 1) & 1)),
                                      (float)(-6 + 12 * ((i >> 2) & 1))
                                        ),
                                 3.5, mat[i % 3]));
    }

    scene.push(new Triangle(
      swVec3(-20.0f, -10.0f, 20.0f),
      swVec3(20.0f, -10.0f, -20.0f),
      swVec3(-20.0f, -10.0f, -20.0f),
      mat[3]));
    scene.push(new Triangle(
      swVec3(-20.0f, -10.0f, 20.0f),
      swVec3(20.0f, -10.0f, 20.0f),
      swVec3(20.0f, -10.0f, -20.0f),
      mat[3]));

    // Setup camera
    swVec3 eye(27, 17, 21);
//    swVec3 eye(0, 30, 0);
    swVec3 lookAt(-1, -3, 0);
//    swVec3 lookAt(0, 0, 0);
    swVec3 up(0, 1, 0);
    swCamera camera(eye, lookAt, up, 52.0f,
                    (float)imageWidth / (float)imageHeight);
    camera.setup(imageWidth, imageHeight);

    // Ray Trace pixels
    int depth = 5;
    int ss_size = 3;

    float subpixel_center = (1.0f / (float)ss_size) / 2;
    float subpixel_size = 1.0f / (float)ss_size;
    float inv_sqr_ss = 1.0f / (float)(ss_size * ss_size);

    std::cout << "Rendering\n";
    auto start = std::chrono::high_resolution_clock::now();
#pragma omp parallel for schedule(static) default(none) shared(imageHeight, imageWidth, ss_size, subpixel_size, subpixel_center, camera, scene, depth, inv_sqr_ss, pixels)
    for (int j = 0; j < imageHeight; ++j) {
        for (int i = 0; i < imageWidth; ++i) {
            Color pixel_sum;
            for (int ssi = 0; ssi < ss_size; ssi++) {
                for (int ssj = 0; ssj < ss_size; ssj++) {

                    float cx = ((float)i) +
                               ((float)ssi * subpixel_size + subpixel_center);
                    float cy = ((float)j) +
                               ((float)ssj * subpixel_size + subpixel_center);

                    swRay r = camera.getRay(cx, cy);
                    pixel_sum += traceRay(r, scene, depth);
                }
            }
            Color output_pixel = pixel_sum * inv_sqr_ss;
            WriteColor((j * imageWidth + i) * numChannels, output_pixel,
                       pixels);
        }
    }
    stbi_write_png("out.png", imageWidth, imageHeight, numChannels, pixels,
                   imageWidth * numChannels);
    delete[] pixels;
    std::cout << "rendering done in " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() << " ms\n";
}
