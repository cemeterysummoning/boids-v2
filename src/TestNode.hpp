#ifndef TEST_H_
#define TEST_H_
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


namespace GLOO{
class TestNode : public SceneNode{
    public: 
        TestNode(){
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

            CreateComponent<ShadingComponent>(shader_);
            CreateComponent<RenderingComponent>(sphere_mesh_);
            CreateComponent<MaterialComponent>(mat_);

            this->GetTransform().SetPosition(position);
        }
    private:

        void time_step(float dt) {
           
        };

        std::shared_ptr<VertexObject> sphere_mesh_ = PrimitiveFactory::CreateSphere(0.015f, 25, 25);
        std::shared_ptr<ShaderProgram> shader_ = std::make_shared<PhongShader>();
        std::shared_ptr<Material> mat_ = std::make_shared<Material>(Material::GetDefault());
        // heading is just normalized velocity
        // do mesh and shit later
};
} // namespace GLOO
#endif