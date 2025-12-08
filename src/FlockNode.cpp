#include "FlockNode.hpp"
#include <vector>
#include "BoidNode.hpp"
// #include <glm/glm.hpp>
#include <random>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include "TestNode.hpp"
#include <chrono>
#include <iostream>
#include <algorithm>
#include "gloo/InputManager.hpp"

namespace GLOO{

using clock = std::chrono::high_resolution_clock;
using time_point = std::chrono::time_point<clock>;

inline time_point now() {
    return clock::now();
}

inline double ms(time_point t0, time_point t1) {
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

FlockNode::FlockNode(){
    // Default constructor
    // 50 boids with default parameters and time step size 0.1, normally distributed around (0,0) 
    // boids_ is a vector of unique_ptrs; start empty

    // debugging rn so only 10
    time_step_size_ = 0.1f;
    for (int i = 0; i < 2000; ++i) {
        std::unique_ptr<BoidNode> temp_boid = make_unique<BoidNode>(dist(rng), dist(rng), dist(rng), 0.01f, 0.01f, 0.01f, 0.0f, 0.0f, 0.f, 1.5f, 5.0f, 3.14f);
        boids_.push_back(temp_boid.get());
        AddChild(std::move(temp_boid));
        // std::cout << "Added boid at" << glm::to_string(boids_.back()->get_position()) << std::endl;
    }
    quadtree_ = make_unique<QuadTree>(lower_bounds_, upper_bounds_, 4, boids_);

    std::unique_ptr<BoidNode> temp_predator = make_unique<BoidNode>(dist(rng), dist(rng), dist(rng), 0.01f, 0.01f, 0.01f, -0.5f, -0.5f, -0.5f, 1.5f, 5.0f, 3.14f, true);
    predator_ = temp_predator.get();
    boids_.push_back(temp_predator.get());
    AddChild(std::move(temp_predator));

}

std::vector<BoidNode*> FlockNode::get_close_boids(const BoidNode& boid) {
    std::vector<BoidNode*> close_boids;
    if (boid.is_predator()) {
        return close_boids;
    }

    auto boid_ptr = &boid;
    if (quadtree_) {
        quadtree_->query(boid_ptr, params_[0], 6.28, close_boids);
        return close_boids;
    }

    for (const auto& other : boids_) {
        if (other != &boid) {
            float distance = glm::length(other->get_position() - boid.get_position());
            if (distance < params_[0]) {
                close_boids.push_back(other);
            }
        }
    }
    return close_boids;
}

std::vector<BoidNode*> FlockNode::get_visible_boids(const BoidNode& boid) {
    std::vector<BoidNode*> visible_boids;
    if (boid.is_predator()) {
        return visible_boids;
    }

    if (quadtree_) {
        auto boid_ptr = &boid;
        quadtree_->query(boid_ptr, params_[1], params_[2], visible_boids);
        return visible_boids;
    }

    for (const auto& other : boids_) {
        if (other != &boid) {
            float distance = glm::length(other->get_position() - boid.get_position());
            float angle = glm::acos(glm::dot(glm::normalize(boid.get_velocity()), glm::normalize(other->get_position() - boid.get_position())));
            if (distance < params_[1] && angle < params_[2] / 2.0f) {
                visible_boids.push_back(other); 
            }
        }
    }
    return visible_boids;
}

void FlockNode::Update(double delta_time) {
    // Constants for each behavior strength

    //reconstruct quadtree each frame

    auto t0 = now();
    quadtree_ = make_unique<QuadTree>(lower_bounds_, upper_bounds_, 4, boids_);
    auto t1 = now();

    int totalNeighbors = 0;
    int maxNeighbors = 0;

    // Per-boid neighbor counting for debugging (counts boids within close range)
    for (auto& bptr : boids_) {
        std::vector<BoidNode*> neighbors;
        if (quadtree_) {
            // use quadtree query (radius = close range, full circle angle)
            quadtree_->query(bptr, params_[0], 6.28f, neighbors);
        } else {
            // fall back to linear scan
            for (auto& other : boids_) {
                if (other == bptr) continue;
                float distance = glm::length(other->get_position() - bptr->get_position());
                if (distance < params_[0]) neighbors.push_back(other);
            }
        }
        totalNeighbors += static_cast<int>(neighbors.size());
        maxNeighbors = std::max(maxNeighbors, static_cast<int>(neighbors.size()));
    }

    double avgNeighbors = boids_.empty() ? 0.0 : static_cast<double>(totalNeighbors) / static_cast<double>(boids_.size());
    


    for (auto& boid_ptr : boids_) {
        BoidNode& boid = *boid_ptr;
        std::vector<BoidNode*> visible_boids = get_visible_boids(boid);
        std::vector<BoidNode*> close_boids = get_close_boids(boid);

        glm::vec3 steer_separation = glm::vec3(0.f);
        glm::vec3 steer_alignment = glm::vec3(0.f);
        glm::vec3 steer_cohesion = glm::vec3(0.f);

        glm::vec3 predator_delta = glm::vec3(0.f);

        if (!close_boids.empty()) {
            for (const BoidNode* other : close_boids) {
                steer_separation += boid.get_position() - other->get_position();
                if (other->is_predator()) {
                    predator_delta = boid.get_position() - other->get_position();
                    steer_separation += params_[8] * predator_delta;
                }
            }
        }

        if (!visible_boids.empty()) {
            for (const BoidNode* other : visible_boids) {
                steer_alignment += other->get_velocity();
                steer_cohesion += other->get_position();
            }
            steer_alignment /= static_cast<float>(visible_boids.size());
            steer_cohesion /= static_cast<float>(visible_boids.size());
        }
        steer_alignment -= boid.get_velocity();
        steer_cohesion -= boid.get_position();

        // smooth turning at margins

        float turn_factor = 0.5f;
        glm::vec3 boundary_turn_acceleration = glm::vec3(0.f);

        if (boid.get_position().x < lower_bounds_.x + margin_) {
            boundary_turn_acceleration.x += (lower_bounds_.x + margin_ - boid.get_position().x) * turn_factor;
        } else if (boid.get_position().x > upper_bounds_.x - margin_) {
            boundary_turn_acceleration.x -= (boid.get_position().x - (upper_bounds_.x - margin_)) * turn_factor;
        }

        if (boid.get_position().y < lower_bounds_.y + margin_) {
            boundary_turn_acceleration.y += (lower_bounds_.y + margin_ - boid.get_position().y) * turn_factor;
        } else if (boid.get_position().y > upper_bounds_.y - margin_) {
            boundary_turn_acceleration.y -= (boid.get_position().y - (upper_bounds_.y - margin_)) * turn_factor;
        }

        if (boid.get_position().z < lower_bounds_.z + margin_) {
            boundary_turn_acceleration.z += (lower_bounds_.z + margin_ - boid.get_position().z) * turn_factor;
        } else if (boid.get_position().z > upper_bounds_.z - margin_) {
            boundary_turn_acceleration.z -= (boid.get_position().z - (upper_bounds_.z - margin_)) * turn_factor;
        }

        glm::vec3 new_acc = steer_separation * params_[5] + steer_alignment * params_[3] + steer_cohesion * params_[4] + boundary_turn_acceleration;

        if (boid.is_predator()) {
            new_acc = boid.get_acceleration();
        }

        if (glm::length(new_acc) > params_[7]) {
            new_acc = glm::normalize(new_acc) * params_[7];
        }

        glm::vec3 new_vel = boid.get_velocity() + new_acc * time_step_size_;

        if (glm::length(new_vel) > params_[6]) {
            new_vel = glm::normalize(new_vel) * params_[6];
        }

        glm::vec3 new_pos = boid.get_position() + new_vel * time_step_size_;

        if (glm::length(new_vel) > 0.001f) {

            // Align the model's local +Y axis to the velocity direction.
            glm::vec3 dir = glm::normalize(new_vel);
            const glm::vec3 model_y(0.0f, 1.0f, 0.0f);
            const float eps = 1e-6f;

            glm::quat target_rotation;

            // If dir is (almost) equal to model_y, use identity rotation.
            if (glm::length2(dir - model_y) < eps) {
                target_rotation = glm::quat(1.f, 0.f, 0.f, 0.f);
            } else if (glm::length2(dir + model_y) < eps) {
                // dir is opposite to model_y: 180 degree rotation around any perpendicular axis
                glm::vec3 axis = glm::cross(model_y, glm::vec3(1.f, 0.f, 0.f));
                if (glm::length2(axis) < eps) axis = glm::cross(model_y, glm::vec3(0.f, 0.f, 1.f));
                axis = glm::normalize(axis);
                target_rotation = glm::angleAxis(glm::pi<float>(), axis);
            } else {
                // General case: rotation that takes +Y to dir
                target_rotation = glm::rotation(model_y, dir);
            }

            // Smoothly slerp from current rotation toward target for natural turning.
            float turn_speed = 5.0f; // units: 1/second, tweakable
            float alpha = 1.0f - std::exp(-turn_speed * static_cast<float>(delta_time));
            glm::quat current = boid.GetTransform().GetRotation();
            glm::quat new_rotation = glm::slerp(current, target_rotation, glm::clamp(alpha, 0.0f, 1.0f));
            boid.set_rotation(new_rotation);

        }

        boid.set_acceleration(new_acc);
        boid.set_velocity(new_vel);
        boid.set_position(new_pos);
    }
    auto t2 = now();
    double buildMs = ms(t0, t1);
    double updateMs = ms(t1, t2);
    if (buildMs + updateMs > 16.67) {
        std::cout << "Warning: Slow frame! Quadtree build: " << buildMs << " ms, Boid update: " << updateMs << " ms, Total: " << (buildMs + updateMs) << " ms\n";
        std::cout << "avg neighbors: " << avgNeighbors
              << ", max neighbors: " << maxNeighbors << "\n";
    }

    if (InputManager::GetInstance().IsKeyPressed('W')) {
        predator_->set_velocity(glm::vec3(0.f, 0.5f, 0.f));
    } else if (InputManager::GetInstance().IsKeyPressed('A')) {
        predator_->set_velocity(glm::vec3(-0.5f, 0.f, 0.f));
    } else if (InputManager::GetInstance().IsKeyPressed('S')) {
        predator_->set_velocity(glm::vec3(0.f, -0.5f, 0.f));
    } else if (InputManager::GetInstance().IsKeyPressed('D')) {
        predator_->set_velocity(glm::vec3(0.5f, 0.f, 0.f));
    } else if (InputManager::GetInstance().IsKeyPressed(265)) {
        predator_->set_velocity(glm::vec3(0.f, 0.f, 0.5f));
    } else if (InputManager::GetInstance().IsKeyPressed(264)) {
        predator_->set_velocity(glm::vec3(0.f, 0.f, -0.5f));
    }
}
} // namespace GLOO