// #include <glm/glm.hpp>
#include "BoidNode.hpp"
#include <vector>
#include <memory>
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
class FlockNode : public SceneNode {
    public: 
        FlockNode();
        FlockNode(std::vector<BoidNode*>&& boids, float time_step_size) : boids_{boids}, time_step_size_{time_step_size} {};
        const std::vector<BoidNode*>& get_boids() const {
            return boids_;
        };
        void add_boid(std::unique_ptr<BoidNode> boid) {
            boids_.push_back(boid.get());
            AddChild(std::move(boid));
        };

        void Update(double delta_time) override;
        glm::vec3 lower_bounds_{-10.f, -10.f, -10.f};
        glm::vec3 upper_bounds_{10.f, 10.f, 10.f};
        float margin_ = 1.f;

    private:
        std::default_random_engine rng{42};  // fixed seed
        std::vector<BoidNode*> boids_;
        float time_step_size_ = 0.1f;
        std::vector<BoidNode*> get_visible_boids(const BoidNode& boid);
        std::vector<BoidNode*> get_close_boids(const BoidNode& boid);
        std::normal_distribution<float> dist{0.0f, 1.f};
        
};
} // namespace GLOO
#endif