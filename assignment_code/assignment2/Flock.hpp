#include <glm/glm.hpp>
#include "Boid.hpp"
#include <vector>
#include <random>

// std::default_random_engine rng(std::random_device{}());
// std::normal_distribution<float> dist(0.0f, 1.0f);  // mean=0, stddev=1

// float x = dist(rng);  // single sample
#ifndef FLOCK_H_
#define FLOCK_H_
class Flock {
    public: 
        Flock();
        Flock(std::vector<Boid> boids, float time_step_size) : boids_{boids}, time_step_size_{time_step_size} {};
        std::vector<Boid> get_boids() const {
            return boids_;
        };
        void add_boid(const Boid& boid) {
            boids_.push_back(boid);
        };
        void update_flock();
        
    private:
        std::default_random_engine rng{42};  // fixed seed
        std::vector<Boid> boids_;
        float time_step_size_ = 0.1f;
        std::vector<Boid> get_visible_boids(const Boid& boid);
        std::vector<Boid> get_close_boids(const Boid& boid);
        std::normal_distribution<float> dist{0.0f, 10.0f};
};
#endif