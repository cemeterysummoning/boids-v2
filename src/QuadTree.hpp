#ifndef QUADTREE_HPP_
#define QUADTREE_HPP_

#include <vector>
#include <memory>
#include <random>
#include <string>
#include "BoidNode.hpp"
#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/utils.hpp"

namespace GLOO{
class QuadTree {
public:
    // 3-argument constructor convenience overload
    QuadTree(glm::vec3 lower_bound, glm::vec3 upper_bound, int capacity)
        : QuadTree(lower_bound, upper_bound, capacity, std::vector<BoidNode*>{}) {}

    QuadTree(glm::vec3 lower_bound, glm::vec3 upper_bound, int capacity, std::vector<BoidNode*> boids) 
        : lower_bound_(lower_bound), upper_bound_(upper_bound), capacity_(capacity) {        
            for (BoidNode* boid : boids) {
                insert(boid);
            }
    }

    // Add member functions and variables here
    void subdivide() {

        if (max_depth-- <= 0) return;
        // Implement subdivision logic here
        if (divided_) return;
        divided_ = true;
        // Create child QuadTrees (8 octants) and store them in children_
        glm::vec3 mid = (lower_bound_ + upper_bound_) / 2.0f;
        children_.clear();
        children_.reserve(8);

        // Lower Z half
        // 0: (low,low,low) -> (mid,mid,mid)
        children_.push_back(make_unique<QuadTree>(
            lower_bound_, mid, capacity_));
        // 1: (mid.x, low.y, low.z) -> (high.x, mid.y, mid.z)
        children_.push_back(make_unique<QuadTree>(
            glm::vec3(mid.x, lower_bound_.y, lower_bound_.z),
            glm::vec3(upper_bound_.x, mid.y, mid.z), capacity_));
        // 2: (low.x, mid.y, low.z) -> (mid.x, high.y, mid.z)
        children_.push_back(make_unique<QuadTree>(
            glm::vec3(lower_bound_.x, mid.y, lower_bound_.z),
            glm::vec3(mid.x, upper_bound_.y, mid.z), capacity_));
        // 3: (mid.x, mid.y, low.z) -> (high.x, high.y, mid.z)
        children_.push_back(make_unique<QuadTree>(
            glm::vec3(mid.x, mid.y, lower_bound_.z),
            glm::vec3(upper_bound_.x, upper_bound_.y, mid.z), capacity_));

        // Upper Z half
        // 4: (low.x, low.y, mid.z) -> (mid.x, mid.y, high.z)
        children_.push_back(make_unique<QuadTree>(
            glm::vec3(lower_bound_.x, lower_bound_.y, mid.z),
            glm::vec3(mid.x, mid.y, upper_bound_.z), capacity_));
        // 5: (mid.x, low.y, mid.z) -> (high.x, mid.y, high.z)
        children_.push_back(make_unique<QuadTree>(
            glm::vec3(mid.x, lower_bound_.y, mid.z),
            glm::vec3(upper_bound_.x, mid.y, upper_bound_.z), capacity_));
        // 6: (low.x, mid.y, mid.z) -> (mid.x, high.y, high.z)
        children_.push_back(make_unique<QuadTree>(
            glm::vec3(lower_bound_.x, mid.y, mid.z),
            glm::vec3(mid.x, upper_bound_.y, upper_bound_.z), capacity_));
        // 7: (mid.x, mid.y, mid.z) -> (high.x, high.y, high.z)
        children_.push_back(make_unique<QuadTree>(
            glm::vec3(mid.x, mid.y, mid.z),
            upper_bound_, capacity_));

    }

    void insert(BoidNode* boid) {
        // Implement insertion logic here
        glm::vec3 pos = boid->get_position();
        if (pos.x < lower_bound_.x || pos.x > upper_bound_.x ||
            pos.y < lower_bound_.y || pos.y > upper_bound_.y ||
            pos.z < lower_bound_.z || pos.z > upper_bound_.z) {
            return; // Out of bounds
        }
        if (boids_.size() < capacity_) {
            boids_.push_back(boid);
        } else {
            if (!divided_) {
                subdivide();
            }
            for (auto& child : children_) {
                child->insert(boid);
            }

            // move existing boids to children

            for (auto it = boids_.begin(); it != boids_.end(); ) {
                bool inserted = false;
                for (auto& child : children_) {
                    glm::vec3 bpos = (*it)->get_position();
                    if (bpos.x >= child->lower_bound_.x && bpos.x <= child->upper_bound_.x &&
                        bpos.y >= child->lower_bound_.y && bpos.y <= child->upper_bound_.y &&
                        bpos.z >= child->lower_bound_.z && bpos.z <= child->upper_bound_.z) {
                        child->insert(*it);
                        it = boids_.erase(it);
                        inserted = true;
                        break;
                    }
                }
                if (!inserted) {
                    ++it;
                }
            }
        }
    }

    void query(const BoidNode* boid, float radius, float view_angle, std::vector<BoidNode*>& found) {
        // If node is out of sphere range, return
        glm::vec3 closest_point = glm::clamp(boid->get_position(), lower_bound_, upper_bound_);
        float distance_sq = glm::dot((closest_point - boid->get_position()), (closest_point - boid->get_position()));
        if (distance_sq > radius * radius) {
            return;
        }


        for (BoidNode* other_boid : boids_) {
            glm::vec3 bpos = other_boid->get_position();
            float dist_sq = glm::dot(bpos - boid->get_position(), bpos - boid->get_position());

            // check angle

             glm::vec3 to_other = glm::normalize(bpos - boid->get_position());
             glm::vec3 boid_dir = glm::normalize(boid->get_velocity());
             float angle = glm::acos(glm::dot(boid_dir, to_other));
             if (angle > view_angle / 2.0f) {
                 continue;
             }
            
             if (dist_sq <= radius * radius) {
                found.push_back(other_boid);
            }
        }
        if (divided_) {
            for (auto& child : children_) {
                child->query(boid, radius, view_angle, found);
            }
        }
    }


private:
    // Add private member variables here
    std::vector<BoidNode*> boids_;
    std::vector<std::unique_ptr<QuadTree>> children_;
    glm::vec3 lower_bound_;
    glm::vec3 upper_bound_;
    u_int64_t capacity_;
    bool divided_ = false;
    int max_depth = 8;
};
} // namespace GLOO

#endif // QUADTREE_HPP_
