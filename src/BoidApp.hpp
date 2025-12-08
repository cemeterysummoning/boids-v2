#ifndef BOID_VIEWER_APP_H_
#define BOID_VIEWER_APP_H_

#include "gloo/Application.hpp"
#include "FlockNode.hpp"

namespace GLOO {
class BoidApp : public Application {
    public:
        BoidApp(const std::string& app_name, glm::ivec2 window_size);
        void SetupScene() override;
        void SetupBoundaries(glm::vec3 lower_bounds, glm::vec3 upper_bounds);
    protected:
        void DrawGUI() override;

    private:
        std::vector<float> slider_values_;
        std::vector<float>* slider_values_ptr_;
        FlockNode* flock_ptr_;

        std::vector<float> min_values_ = {
            0.0f, // 0: close range
            0.0f, // 1: visible range 
            0.0f, // 2: visible angle
            0.05f, // 3: alignment strength
            0.05f, // 4: cohesion strength
            0.05f, // 5: separation strength
            0.05f, // 6: max speed
            0.0f, // 7: max force
            0.0f // 8: predator avoidance
        };
        std::vector<float> max_values_ = {
            10.0f, // 0: close range
            20.0f, // 1: visible range 
            3.14f, // 2: visible angle
            10.0f, // 3: alignment strength
            10.0f, // 4: cohesion strength
            10.0f, // 5: separation strength
            30.0f, // 6: max speed
            30.0f, // 7: max force,
            10.0f // 8: predator avoidance
        };
};
}  // namespace GLOO

#endif