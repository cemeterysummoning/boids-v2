#include "FlockNode.hpp"
#include <vector>
#include "BoidNode.hpp"
#include <glm/glm.hpp>
#include <random>


namespace GLOO{
FlockNode::FlockNode(){
    // Default constructor
    // 50 boids with default parameters and time step size 0.1, normally distributed around (0,0) 

    boids_ = std::vector<BoidNode>();
    time_step_size_ = 0.1f;
    for (int i = 0; i < 50; ++i) {
        BoidNode b = BoidNode(dist(rng), dist(rng), dist(rng), dist(rng), 0.0f, 0.0f, 1.5f, 5.0f, 3.14f);
        boids_.push_back(b);
    }
}

std::vector<BoidNode> FlockNode::get_close_boids(const BoidNode& boid) {
    std::vector<BoidNode> close_boids;
    for (const BoidNode& other : boids_) {
        if (&other != &boid) {
            float distance = glm::length(other.get_position() - boid.get_position());
            if (distance < boid.get_close_range()) {
                close_boids.push_back(other);
            }
        }
    }
    return close_boids;
}

std::vector<BoidNode> FlockNode::get_visible_boids(const BoidNode& boid) {
    std::vector<BoidNode> visible_boids;
    for (const BoidNode& other : boids_) {
        if (&other != &boid) {
            float distance = glm::length(other.get_position() - boid.get_position());
            float angle = glm::acos(glm::dot(glm::normalize(boid.get_velocity()), glm::normalize(other.get_position() - boid.get_position())));
            if (distance < boid.get_visible_range() && angle < boid.get_visible_angle() / 2.0f) {
                visible_boids.push_back(other); 
            }
        }
    }
    return visible_boids;
}

void FlockNode::update_flock() {
    // Constants for each behavior strength
    float alignment_strength = 1.0f;
    float cohesion_strength = 1.0f;
    float separation_strength = 1.5f;
    float max_speed = 2.0f;
    float max_force = 0.05f;

    for (BoidNode& boid : boids_) {
        std::vector<BoidNode> visible_boids = get_visible_boids(boid);
        std::vector<BoidNode> close_boids = get_close_boids(boid);

        glm::vec3 steer_separation = glm::vec3(0.f);
        glm::vec3 steer_alignment = glm::vec3(0.f);
        glm::vec3 steer_cohesion = glm::vec3(0.f);

        if (!close_boids.empty()) {
            for (const BoidNode& other : close_boids) {
                steer_separation += boid.get_position() - other.get_position();
            }
        }

        if (!visible_boids.empty()) {
            for (const BoidNode& other : visible_boids) {
                steer_alignment += other.get_velocity();
                steer_cohesion += other.get_position();
            }
            steer_alignment /= static_cast<float>(visible_boids.size());
            steer_cohesion /= static_cast<float>(visible_boids.size());
        }
        steer_alignment -= boid.get_velocity();
        steer_cohesion -= boid.get_position();

        glm::vec3 new_acc = steer_separation * separation_strength + steer_alignment * alignment_strength + steer_cohesion * cohesion_strength;
        glm::vec3 new_vel = boid.get_velocity() + new_acc * time_step_size_;
        glm::vec3 new_pos = boid.get_position() + new_vel * time_step_size_;


        if (glm::length(new_vel) > 0.001f) {
    
            // 2. Normalize the velocity to get the direction
            glm::vec3 direction = glm::normalize(new_vel);
            
            // 3. Define the 'Up' vector (usually World Y)
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

            // 4. Calculate target rotation
            // Note: glm::quatLookAt creates a rotation looking in the 'direction'
            // This assumes your model's forward face is -Z (Standard OpenGL). 
            // If your model faces +Z, negate the direction: glm::quatLookAt(-direction, up)
            glm::quat target_rotation = glm::quatLookAt(direction, up);

            // 5. OPTIONAL: Smooth the rotation (Slerp)
            // Instant rotation looks robotic. Spherical Linear Interpolation (Slerp) 
            // makes it turn naturally over time.
            float turn_speed = 5.0f * time_step_size_;
            glm::quat current_rotation = boid.get_rotation(); // Assuming you have this getter
            glm::quat new_rotation = glm::slerp(current_rotation, target_rotation, turn_speed);

            boid.set_rotation(new_rotation);
        }

        boid.set_acceleration(new_acc);
        boid.set_velocity(new_vel);
        boid.set_position(new_pos);
    }
}
} // namespace GLOO