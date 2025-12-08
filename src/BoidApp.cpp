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
#include "FlockNode.hpp"
#include "FlockNode.hpp"

namespace {
    const std::vector<std::string> parameterNames = {"close range", "visible range", "visible angle", "alignment strength", "cohesion strength", "separation strength", "max speed", "max force", "predator avoidance"};
}

namespace GLOO {
BoidApp::BoidApp(const std::string& app_name,
                                 glm::ivec2 window_size)
    : Application(app_name, window_size){
}

void BoidApp::SetupScene() {
    SceneNode& root = scene_->GetRootNode();

    auto camera_node = make_unique<ArcBallCameraNode>(50.0f, 0.75f, 50.0f);
    // camera_node->GetTransform().SetPosition(glm::vec3(0.f, 0.f, 50.f));
    camera_node->Calibrate();
    scene_->ActivateCamera(camera_node->GetComponentPtr<CameraComponent>());
    root.AddChild(std::move(camera_node));

    auto ambient_light = std::make_shared<AmbientLight>();
    ambient_light->SetAmbientColor(glm::vec3(1.0f));
    root.CreateComponent<LightComponent>(ambient_light);


    // auto point_light = std::make_shared<PointLight>();
    // point_light->SetDiffuseColor(glm::vec3(0.8f, 0.8f, 0.8f));
    // point_light->SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
    // point_light->SetAttenuation(glm::vec3(1.0f, 0.09f, 0.032f));
    // auto point_light_node = make_unique<SceneNode>();
    // point_light_node->CreateComponent<LightComponent>(point_light);
    // point_light_node->GetTransform().SetPosition(glm::vec3(0.f, 0.f, 10.f));
    // root.AddChild(std::move(point_light_node));

    std::shared_ptr<PhongShader> shader = std::make_shared<PhongShader>();

    auto flock_node = make_unique<FlockNode>();
    flock_ptr_ = flock_node.get();
    SetupBoundaries(flock_node->lower_bounds_ - glm::vec3(flock_node->margin_), flock_node->upper_bounds_ + glm::vec3(flock_node->margin_));
    root.AddChild(std::move(flock_node));
}

void BoidApp::SetupBoundaries(glm::vec3 lower_bounds, glm::vec3 upper_bounds) {
    // make a cube for bounds
    // maybe add mesh later

    // auto quad_mesh = PrimitiveFactory::CreateQuad();
    // // RenderingComponent expects a shared_ptr<VertexObject>, convert ownership
    // std::shared_ptr<VertexObject> quad_mesh_shared = std::move(quad_mesh);

    // auto boundary_node = make_unique<SceneNode>();
    // boundary_node->CreateComponent<RenderingComponent>(quad_mesh_shared);
    // boundary_node->CreateComponent<ShadingComponent>(std::make_shared<PhongShader>());
    // boundary_node->GetTransform().SetPosition(glm::vec3(lower_bounds.x/2,lower_bounds.y/2,lower_bounds.z));
    // boundary_node->GetTransform().SetScale((upper_bounds - lower_bounds));


    // std::shared_ptr<Material> mat_ = std::make_shared<Material>();
    // mat_->SetAmbientColor(glm::vec3(0.2f, 0.2f, 0.2f));
    // mat_->SetDiffuseColor(glm::vec3(0.5f, 0.5f, 0.5f));
    // mat_->SetSpecularColor(glm::vec3(0.1f, 0.1f, 0.1f));
    

    // boundary_node->CreateComponent<MaterialComponent>(mat_);
    // scene_->GetRootNode().AddChild(std::move(boundary_node));

    // top lines
    auto line_shader = std::make_shared<SimpleShader>();

    // Build a wireframe rectangular prism from the given bounds (12 edges).
    auto box = std::make_shared<VertexObject>();

    // 8 corners: top 0-3, bottom 4-7
    auto positions = make_unique<PositionArray>();
    positions->push_back(glm::vec3(lower_bounds.x, upper_bounds.y, lower_bounds.z)); // 0
    positions->push_back(glm::vec3(upper_bounds.x, upper_bounds.y, lower_bounds.z)); // 1
    positions->push_back(glm::vec3(upper_bounds.x, upper_bounds.y, upper_bounds.z)); // 2
    positions->push_back(glm::vec3(lower_bounds.x, upper_bounds.y, upper_bounds.z)); // 3
    positions->push_back(glm::vec3(lower_bounds.x, lower_bounds.y, lower_bounds.z)); // 4
    positions->push_back(glm::vec3(upper_bounds.x, lower_bounds.y, lower_bounds.z)); // 5
    positions->push_back(glm::vec3(upper_bounds.x, lower_bounds.y, upper_bounds.z)); // 6
    positions->push_back(glm::vec3(lower_bounds.x, lower_bounds.y, upper_bounds.z)); // 7

    // Indices for 12 edges (each edge is two indices)
    auto indices = IndexArray();
    // top face loop
    indices.push_back(0); indices.push_back(1);
    indices.push_back(1); indices.push_back(2);
    indices.push_back(2); indices.push_back(3);
    indices.push_back(3); indices.push_back(0);
    // bottom face loop
    indices.push_back(4); indices.push_back(5);
    indices.push_back(5); indices.push_back(6);
    indices.push_back(6); indices.push_back(7);
    indices.push_back(7); indices.push_back(4);
    // vertical connectors
    indices.push_back(0); indices.push_back(4);
    indices.push_back(1); indices.push_back(5);
    indices.push_back(2); indices.push_back(6);
    indices.push_back(3); indices.push_back(7);

    box->UpdatePositions(std::move(positions));
    box->UpdateIndices(make_unique<IndexArray>(indices));

    auto line_node = make_unique<SceneNode>();
    line_node->CreateComponent<ShadingComponent>(line_shader);
    auto& rc_box = line_node->CreateComponent<RenderingComponent>(box);
    rc_box.SetDrawMode(DrawMode::Lines);
    auto color = glm::vec3(0.8f, 0.8f, 0.8f);
    auto material = std::make_shared<Material>(color, color, color, 0.0f);
    line_node->CreateComponent<MaterialComponent>(material);
    scene_->GetRootNode().AddChild(std::move(line_node));

}

void BoidApp::DrawGUI() {
    bool modified = false;
    ImGui::Begin("Control Panel");
    for (size_t i = 0; i < parameterNames.size(); i++) {
        ImGui::Text("%s", parameterNames[i].c_str());
        ImGui::PushID((int)i);
        modified |= ImGui::SliderFloat("", &flock_ptr_->params_[i], min_values_[i], max_values_[i]);
        ImGui::PopID();
    }
    ImGui::End();

}

}  // namespace GLOO
