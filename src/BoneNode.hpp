#ifndef BONE_NODE_H_
#define BONE_NODE_H_

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

namespace GLOO {
class BoneNode : public SceneNode {
 public:
  BoneNode(glm::vec3 position);

 private:
  std::shared_ptr<VertexObject> cylinder_mesh_ = PrimitiveFactory::CreateCylinder(0.01, 1.f, 10);
  std::shared_ptr<ShaderProgram> shader_ = std::make_shared<PhongShader>();
  std::shared_ptr<Material> mat_ = std::make_shared<Material>(Material::GetDefault());
};
}  // namespace GLOO

#endif