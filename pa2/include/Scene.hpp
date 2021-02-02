#pragma once

#include "Camera.hpp"
#include "Ray.hpp"
#include "Triangle.hpp"

#include "tinyobjloader/tiny_obj_loader.h"

#include <algorithm>
#include <vector>

// Node for constructing BVH.
struct BVHNode : Node<2> {
  public:
    BBox bbox;
    flt  area;

    BVHNode *fa;

    // Do not subdivide a node when there are less than `threshold` primitives
    // inside it.
    std::size_t threshold;

    std::vector<Triangle const *> tris;

    std::array<BVHNode *, 2> children;

  public:
    BVHNode() : area{0}, threshold{16} {}
    BVHNode(std::vector<Triangle const *> const &tris)
        : area{0}, threshold{16}, tris{tris}, children{nullptr} {}
    BVHNode(std::vector<Triangle> const &tris)
        : area{0}, threshold{16}, children{nullptr} {
        for (Triangle const &t : tris) {
            this->tris.push_back(&t);
        }
    }

    void build(BVHNode *fa = nullptr) {
        if (fa) {
            this->tdep = fa->tdep + 1;
        }
        /* Naive */
        if (this->tris.size() <= this->threshold) {
            for (Triangle const *t : this->tris) {
                this->bbox |= t->boundingbox();
                // this->area += t->area();
                this->area += t->doublearea();
                this->isleaf = true;
            }
            return;
        }
        BBox centroid_box;
        for (Triangle const *t : this->tris) {
            centroid_box |= t->boundingbox().centroid();
        }
        // Split along the direction that has the maximum extent.
        auto begining = this->tris.begin();
        auto ending   = this->tris.end();
        auto middling = begining + this->tris.size() * 0.5;
        switch (centroid_box.max_dir()) {
        case 0:
            std::nth_element(begining, middling, ending,
                             [](Triangle const *t1, Triangle const *t2) {
                                 return t1->boundingbox().centroid().x <
                                        t2->boundingbox().centroid().x;
                             });
            break;
        case 1:
            std::nth_element(begining, middling, ending,
                             [](Triangle const *t1, Triangle const *t2) {
                                 return t1->boundingbox().centroid().y <
                                        t2->boundingbox().centroid().y;
                             });
            break;
        case 2:
            std::nth_element(begining, middling, ending,
                             [](Triangle const *t1, Triangle const *t2) {
                                 return t1->boundingbox().centroid().z <
                                        t2->boundingbox().centroid().z;
                             });
            break;
        default:
            errorm("?\n");
        }
        this->children[0] =
            new BVHNode(std::vector<Triangle const *>(begining, middling));
        this->children[1] =
            new BVHNode(std::vector<Triangle const *>(middling, ending));
        this->children[0]->build(this);
        this->children[1]->build(this);
        this->bbox = this->children[0]->bbox | this->children[1]->bbox;
        this->area = this->children[0]->area + this->children[1]->area;
    }
};

class Scene {
  private:
    std::vector<Triangle> orig_tris;
    std::vector<Triangle> tris;

    std::vector<tinyobj::material_t> mats;

    BVHNode *root;

  public:
    Scene();
    Scene(tinyobj::ObjReader const &loader);

    void to_camera_space(Camera const &cam);

    void build_BVH();

    Intersection intersect(Ray const &ray) const;

    std::vector<Triangle> const &           triangles() const;
    std::vector<tinyobj::material_t> const &materials() const;
};

// Author: Blurgy <gy@blurgy.xyz>
// Date:   Jan 31 2021, 21:11 [CST]
