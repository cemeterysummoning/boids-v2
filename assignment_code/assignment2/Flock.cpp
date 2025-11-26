#include "Flock.hpp"
#include <vector>
#include "Boid.hpp"
#include <glm/glm.hpp>
#include <random>

Flock::Flock(){
    // Default constructor
    // 50 boids with default parameters and time step size 0.1, normally distributed around (0,0) 

    boids_ = std::vector<Boid>();
    time_step_size_ = 0.1f;
    for (int i = 0; i < 50; ++i) {
        Boid b = Boid(dist(rng), dist(rng), dist(rng), dist(rng), 0.0f, 0.0f, 1.5f, 5.0f);
        boids_.push_back(b);
    }
}

std::vector<Boid> Flock::get_close_boids(const Boid& boid) {
    std::vector<Boid> close_boids;
    for (const Boid& other : boids_) {
        if (&other != &boid) {
            float distance = glm::length(other.get_position() - boid.get_position());
            if (distance < boid.get_close_range()) {
                close_boids.push_back(other);
            }
        }
    }
    return close_boids;
}

std::vector<Boid> Flock::get_visible_boids(const Boid& boid) {
    std::vector<Boid> visible_boids;
    for (const Boid& other : boids_) {
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

void Flock::update_flock() {
    // Constants for each behavior strength
    float alignment_strength = 1.0f;
    float cohesion_strength = 1.0f;
    float separation_strength = 1.5f;
    float max_speed = 2.0f;
    float max_force = 0.05f;

    for (Boid& boid : boids_) {
        std::vector<Boid> visible_boids = get_visible_boids(boid);
        std::vector<Boid> close_boids = get_close_boids(boid);

        glm::vec3 steer_separation = glm::vec3(0.f);
        glm::vec3 steer_alignment = glm::vec3(0.f);
        glm::vec3 steer_cohesion = glm::vec3(0.f);

        if (!close_boids.empty()) {
            for (const Boid& other : close_boids) {
                steer_separation += boid.get_position() - other.get_position();
            }
        }

        if (!visible_boids.empty()) {
            for (const Boid& other : visible_boids) {
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

        boid.set_acceleration(new_acc);
        boid.set_velocity(new_vel);
        boid.set_position(new_pos);
    }
}