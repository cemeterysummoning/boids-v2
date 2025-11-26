#ifndef BOID_VIEWER_APP_H_
#define BOID_VIEWER_APP_H_

#include "gloo/Application.hpp"

namespace GLOO {
class BoidApp : public Application {
    public:
        BoidApp(const std::string& app_name, glm::ivec2 window_size);
        void SetupScene() override;

    protected:
        void DrawGUI() override;

    private:
        std::vector<float> slider_values_;
};
}  // namespace GLOO

#endif