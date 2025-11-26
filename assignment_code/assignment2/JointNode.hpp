#ifndef JOINT_NODE_H_
#define JOINT_NODE_H_

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
class JointNode : public SceneNode {
 public:
  JointNode(glm::vec3 position);

 private:
  std::shared_ptr<VertexObject> sphere_mesh_ = PrimitiveFactory::CreateSphere(0.015f, 25, 25);
  std::shared_ptr<ShaderProgram> shader_ = std::make_shared<PhongShader>();
  std::shared_ptr<Material> mat_ = std::make_shared<Material>(Material::GetDefault());
};
}  // namespace GLOO

#endif
