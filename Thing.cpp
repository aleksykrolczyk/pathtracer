//
// Created by Adam Korytowski on 15/12/2021.
//

#include <algorithm>
#include <iostream>
#include "Thing.h"
Thing::Thing(const float *vertices, int vertexCount, const Mat3 &transform, const swVec3 &transpose, const swMaterial &material) {
    triangleCount = vertexCount / 3;
    triangles = new Triangle[triangleCount];
    float minx=9999, miny=9999, minz=9999, maxx=-99999, maxy=-99999, maxz=-9999;
    for(int i = 0; i < triangleCount; i++) {
        int ii = i * 12;
        swVec3 vert1(vertices[ii], vertices[ii+1], vertices[ii+2]);
        swVec3 vert2(vertices[ii+4], vertices[ii+5], vertices[ii+6]);
        swVec3 vert3(vertices[ii+8], vertices[ii+9], vertices[ii+10]);

        vert1 = transform * vert1 + transpose;
        vert2 = transform * vert2 + transpose;
        vert3 = transform * vert3 + transpose;

        minx = std::min({minx, vert1.x(), vert2.x(), vert3.x()});
        miny = std::min({miny, vert1.y(), vert2.y(), vert3.y()});
        minz = std::min({minz, vert1.z(), vert2.z(), vert3.z()});
        maxx = std::max({maxx, vert1.x(), vert2.x(), vert3.x()});
        maxy = std::max({maxy, vert1.y(), vert2.y(), vert3.y()});
        maxz = std::max({maxz, vert1.z(), vert2.z(), vert3.z()});

        triangles[i] = Triangle(vert1, vert2, vert3, material);
    }

    // bottom-top:left-right:down-up
    swVec3 bld(minx, miny, minz);
    swVec3 brd(maxx, miny, minz);
    swVec3 blu(minx, miny, maxz);
    swVec3 bru(maxx, miny, maxz);
    swVec3 tld(minx, maxy, minz);
    swVec3 trd(maxx, maxy, minz);
    swVec3 tlu(minx, maxy, maxz);
    swVec3 tru(maxx, maxy, maxz);

    // bottom
    box[0] = Triangle(brd, bld, blu, material);
    box[1] = Triangle(blu, bru, brd, material);
    // left
    box[2] = Triangle(bld, blu, tlu, material);
    box[3] = Triangle(bld, tld, tlu, material);
    // back
    box[4] = Triangle(blu, bru, tru, material);
    box[5] = Triangle(blu, tlu, tru, material);
    // right
    box[6] = Triangle(bru, brd, trd, material);
    box[7] = Triangle(bru, tru, trd, material);
    // front
    box[8] = Triangle(brd, bld, tld, material);
    box[9] = Triangle(brd, trd, tld, material);
    // top
    box[10] = Triangle(trd, tld, tlu, material);
    box[11] = Triangle(tlu, tru, trd, material);
}

bool Thing::intersect(const swRay &r, swIntersection &isect) {
    bool hit = false;
    for(auto & item : box) {
        if(item.intersect(r)) {
            hit = true;
            break;
        }
    }
    if (!hit) {
        return false;
    }

    hit = false;
    swIntersection curIsect;

    for(int i = 0; i < triangleCount; i++) {
        if(triangles[i].intersect(r, curIsect)) {
            hit = true;
            if(curIsect.mHitTime < isect.mHitTime) {
                isect = curIsect;
            }
        }
    }

    return hit;
}
