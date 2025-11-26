#include "BoidApp.hpp"

#include <glm/gtx/string_cast.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "gloo/SceneNode.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/shaders/SimpleShader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/CameraComponent.hpp"
#include "gloo/components/LightComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/MeshLoader.hpp"
#include "gloo/lights/AmbientLight.hpp"
#include "gloo/lights/PointLight.hpp"
#include "gloo/cameras/BasicCameraNode.hpp"
#include "gloo/cameras/ArcBallCameraNode.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"

namespace {
    const std::vector<std::string> parameterNames = {"cohesion", "alignment", "visual range"};
}

namespace GLOO {
BoidApp::BoidApp(const std::string& app_name,
                                 glm::ivec2 window_size)
    : Application(app_name, window_size), slider_values_(parameterNames.size(), 0.f) {
}

void BoidApp::SetupScene() {
    SceneNode& root = scene_->GetRootNode();

    auto camera_node = make_unique<ArcBallCameraNode>(50.0f, 1.0f, 10.0f);
    camera_node->GetTransform().SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));
    camera_node->GetTransform().SetRotation(glm::vec3(0.0f, 1.0f, 0.0f), kPi / 2);
    camera_node->Calibrate();
    scene_->ActivateCamera(camera_node->GetComponentPtr<CameraComponent>());
    root.AddChild(std::move(camera_node));

    auto ambient_light = std::make_shared<AmbientLight>();
    ambient_light->SetAmbientColor(glm::vec3(0.15f));
    auto ambient_node = make_unique<SceneNode>();
    ambient_node->CreateComponent<LightComponent>(ambient_light);
    root.AddChild(std::move(ambient_node));

    std::shared_ptr<PhongShader> shader = std::make_shared<PhongShader>();

    auto flock_node = make_unique<FlockNode>();
    flock_ptr_ = flock_node.get();
    root.AddChild(std::move(flock_node));
}

void BoidApp::DrawGUI() {
    bool modified = false;
    ImGui::Begin("Control Panel");
    for (size_t i = 0; i < parameterNames.size(); i++) {
        ImGui::Text("%s", parameterNames[i].c_str());
        ImGui::PushID((int)i);
        modified |= ImGui::SliderFloat("", &slider_values_[i], -kPi, kPi);
        ImGui::PopID();
    }
    ImGui::End();

}

}  // namespace GLOO
