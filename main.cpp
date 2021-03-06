/*
 *  main.cpp
 *  swTracer
 *
 *  Created by Michael Doggett on 2021-09-23.
 *  Copyright (c) 2021 Michael Doggett
 */
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <random>
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
#include "CONSTS.h"
#include "Thing.h"
#include "myTeapot.h"
#include "cube.h"
#include "SmoothThing.h"

const Color BLACK = Color(0, 0, 0);
const Color WHITE = Color(1.0f, 1.0f, 1.0f);

inline float clamp(float x, float min, float max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

float uniform2() {
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

Mat3 scale(float x, float y, float z) {
    swVec3 v0(x,0,0);
    swVec3 v1(0,y,0);
    swVec3 v2(0,0,z);
    return { v0, v1, v2 };
}

Mat3 rotateX(float alpha) {
    auto _sin = sin(alpha);
    auto _cos = cos(alpha);
    swVec3 v0(1,0,   0);
    swVec3 v1(0,_cos,-_sin);
    swVec3 v2(0,_sin,_cos);
    return { v0, v1, v2 };
}

Mat3 rotateY(float alpha) {
    auto _sin = sin(alpha);
    auto _cos = cos(alpha);
    swVec3 v0(_cos, 0,_sin);
    swVec3 v1(0,    1,0);
    swVec3 v2(-_sin,0,_cos);
    return { v0, v1, v2 };
}

Mat3 rotateZ(float alpha) {
    auto _sin = sin(alpha);
    auto _cos = cos(alpha);
    swVec3 v0(_cos, _sin,0);
    swVec3 v1(-_sin, _cos, 0);
    swVec3 v2(0,    0,    1);
    return { v0, v1, v2 };
}

//const swVec3 lightPos(275, 470, 250);
//(556.0, 548.8, 559.2)
const swVec3 lightPos(275.0, 670, 275);
#define ALPHA 0.1f
#define COEFF 0.9f
#define EMITTANCE 5.0f
Color traceRay(const swRay &r, swScene scene, int depth) {
    Color directColor, indirect;
    swIntersection hp, si;

    if (!scene.intersect(r, hp)) {
        return BLACK;
    }

    if (hp.mMaterial.emits_light()) {
        return hp.mMaterial.emittance;
    }

    float rul = 1.0f;
    if (depth > 7) {
        auto x = uniform2();
        if (x < ALPHA) {
            return BLACK;
        }
        rul = 1 / COEFF;
    }

    swVec3 lightDir = lightPos - hp.mPosition;
    float ldist = lightDir.length();
    float dist = ldist / (PI4 * 1.5); // ~ 264.785
    lightDir.normalize();

    directColor = hp.mMaterial.mColor;

    auto y = uniform2();
    auto refl = hp.mMaterial.reflectivity;
    auto trans = hp.mMaterial.transparency;

    if (y < refl) {
        return traceRay(hp.getReflectedRay(), scene, depth + 1) * rul;
    }
    if (y > refl && y < refl + trans) {
        return traceRay(hp.getRefractedRay(), scene, depth + 1) * rul;
    }


    auto newRay = hp.getRandomRay();
    auto cosTerm = newRay.dir * hp.mNormal;

    return directColor.elemMul(traceRay(newRay, scene, depth + 1)) * cosTerm * rul;
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

    const swMaterial WHITE_MAT =     swMaterial(swVec3(1.0f, 1.0f, 1.0f),      0.0f, 0.0f, 1.01f);
    const swMaterial GREEN_MAT =     swMaterial(swVec3(0.3125f, 0.86f, 0.39f), 0.0f, 0.0f, 1.01f);
    const swMaterial RED_MAT =       swMaterial(swVec3(0.95f, 0.30f, 0.30f),  0.0f, 0.0f, 1.01f);
    const swMaterial YELLOWISH_MAT = swMaterial(swVec3(0.95f, 0.865f, 0.49f), 0.0f, 0.0f, 1.01f);
    const swMaterial MIRROR =        swMaterial(swVec3(0.6f, 0.6f, 0.6f),     0.9f, 0.0f, 1.01f);
    const swMaterial GLASS =         swMaterial(swVec3(0.0f, 0.0f, 1.0f),     0.1f, 0.9f, 1.52f);
    const swMaterial PORCELAIN =     swMaterial(swVec3(0.93f,0.89f,0.81f),     0.1f, 0.0f, 1.01f);

    const swMaterial WHITE_LIGHT = swMaterial(swVec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f, 1.01f, swVec3(EMITTANCE, EMITTANCE, EMITTANCE));

    scene.push(new swSphere(swVec3(470,75,100), 75, MIRROR));
    scene.push(new swSphere(swVec3(120,75,100), 75, GLASS));

    scene.push(new swSphere(lightPos, 210, WHITE_LIGHT));

    auto transform = scale(200,200,200);
//    auto rotationX = rotateX(PI/4);
    auto rotationY = rotateY(PI/8);
    transform = transform * rotationY;
    auto translate = swVec3(175,100,320);
    auto czajnik = new SmoothThing(myTeapotVertices, myTeapotVertexNormals, myTeapotVertexCount, transform, translate, PORCELAIN);
    scene.push(czajnik);

    transform = scale(40,100,50);
    translate = swVec3(280,50,120);
    auto wesoly_kostek = new Thing(cubeVertices, cubeVertexCount, transform, translate, GLASS);
    scene.push(wesoly_kostek);


    transform = scale(100,30,30);
    translate = swVec3(330, 30,450);

    auto cokol = new Thing(cubeVertices, cubeVertexCount, transform, translate, YELLOWISH_MAT);
    scene.push(cokol);


    Triangle box[] = {
        /// floor
        Triangle(
            swVec3(552.8, 000.0, 000.0),
            swVec3(000.0, 000.0, 000.0),
            swVec3(000.0, 000.0, 559.2),
        WHITE_MAT
            ),
        Triangle(
            swVec3(552.8, 000.0, 000.0),
            swVec3(000.0, 000.0, 559.2),
            swVec3(549.6, 000.0, 559.2),
        WHITE_MAT
            ),
        // left wall
        Triangle(
            swVec3(552.8, 000.0, 000.0),
            swVec3(549.6, 000.0, 559.2),
            swVec3(556.0, 548.8, 559.2),
            RED_MAT
        ),
        Triangle(
            swVec3(552.8, 000.0, 000.0),
            swVec3(556.0, 548.8, 000.0),
            swVec3(556.0, 548.8, 559.2),
            RED_MAT
        ),

        // right wall
        Triangle(
            swVec3(000.0, 000.0, 559.2),
            swVec3(000.0, 000.0, 000.0),
            swVec3(000.0, 548.8, 000.0),
            GREEN_MAT
        ),
        Triangle(
            swVec3(000.0, 000.0, 559.2),
            swVec3(000.0, 548.8, 559.2),
            swVec3(000.0, 548.8, 000.0),
        GREEN_MAT
        ),
        // back wall
        Triangle(
            swVec3(549.6, 000.0, 559.2),
            swVec3(000.0, 000.0, 559.2),
            swVec3(000.0, 548.8, 559.2),
            WHITE_MAT
        ),
        Triangle(
            swVec3(549.6, 000.0, 559.2),
            swVec3(556.0, 548.8, 559.2),
            swVec3(000.0, 548.8, 559.2),
        WHITE_MAT
        ),
      // ceiling
      Triangle(
        swVec3(556.0, 548.0, 000.0),
        swVec3(556.0, 548.8, 559.2),
        swVec3(000.0, 548.8, 559.2),
        WHITE_MAT
        ),
      Triangle(
        swVec3(556.0, 548.0, 000.0),
        swVec3(000.0, 548.8, 000.0),
        swVec3(000.0, 548.8, 559.2),
        WHITE_MAT
        ),
    };

    for (auto& triangle : box) {
        scene.push(&triangle);
    }

    // Setup camera
    swVec3 eye(278,273,-530);
    swVec3 lookAt(278,273,1);
    swVec3 up(0, 1, 0);
    swCamera camera(eye, lookAt, up, 52.0f,
                    (float)imageWidth / (float)imageHeight);
    camera.setup(imageWidth, imageHeight);

    // Ray Trace pixels
    int depth = 0;
    int ss_size = 50;

    float subpixel_center = (1.0f / (float)ss_size) / 2;
    float subpixel_size = 1.0f / (float)ss_size;
    float inv_sqr_ss = 1.0f / (float)(ss_size * ss_size);

    std::cout << "Rendering\n";
    auto start = std::chrono::high_resolution_clock::now();
    int c = 0;

#pragma omp parallel for schedule(dynamic, 1) default(none) shared(imageHeight, imageWidth, ss_size, subpixel_size, subpixel_center, camera, scene, depth, inv_sqr_ss, pixels, std::cout, c)
    for (int j = 0; j < imageHeight; ++j) {
        for (int i = 0; i < imageWidth; ++i) {
            Color pixel_sum;

            for(int ss = 0; ss < ss_size; ss++) {
                float u1 = uniform2() - 0.5f, u2 = uniform2() - 0.5f;
                swRay r = camera.getRay((float)i + u1, (float)j + u2);
                pixel_sum += traceRay(r, scene, 0);
            }
            Color output_pixel = pixel_sum / ss_size;
#pragma omp critical
            {
                WriteColor((j * imageWidth + i) * numChannels, output_pixel,
                           pixels);
            }
        }
#pragma omp critical
        {
            c++;
            std::cout << (float)c / (float)imageHeight * 100 << "%\n";
        }
    }
    stbi_write_png("out.png", imageWidth, imageHeight, numChannels, pixels,
                   imageWidth * numChannels);
    delete[] pixels;
    std::cout << "rendering done in " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() << " ms\n";
}
