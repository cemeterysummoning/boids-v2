#include <glm/glm.hpp>
#include "BoidNode.hpp"
#include <vector>
#include <random>
#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"

#include <string>
#include <vector>

// std::default_random_engine rng(std::random_device{}());
// std::normal_distribution<float> dist(0.0f, 1.0f);  // mean=0, stddev=1

// float x = dist(rng);  // single sample
#ifndef FLOCK_H_
#define FLOCK_H_


namespace GLOO{
class FlockNode : SceneNode {
    public: 
        FlockNode();
        FlockNode(std::vector<BoidNode> boids, float time_step_size) : boids_{boids}, time_step_size_{time_step_size} {};
        std::vector<BoidNode> get_boids() const {
            return boids_;
        };
        void add_boid(const BoidNode& boid) {
            boids_.push_back(boid);
        };
        void update_flock();
        
    private:
        std::default_random_engine rng{42};  // fixed seed
        std::vector<BoidNode> boids_;
        float time_step_size_ = 0.1f;
        std::vector<BoidNode> get_visible_boids(const BoidNode& boid);
        std::vector<BoidNode> get_close_boids(const BoidNode& boid);
        std::normal_distribution<float> dist{0.0f, 10.0f};
};
} // namespace GLOO
#endif