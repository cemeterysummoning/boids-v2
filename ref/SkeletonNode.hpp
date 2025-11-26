#ifndef SKELETON_NODE_H_
#define SKELETON_NODE_H_

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "JointNode.hpp"
#include "BoneNode.hpp"

#include <string>
#include <vector>

void print_v(glm::vec3 v);

namespace GLOO {
class SkeletonNode : public SceneNode {
 public:
  enum class DrawMode { Skeleton, SSD };
  struct EulerAngle {
    float rx, ry, rz;
  };

  SkeletonNode(const std::string& filename);
  void LinkRotationControl(const std::vector<EulerAngle*>& angles);
  void Update(double delta_time) override;
  void OnJointChanged();

 private:
  void LoadAllFiles(const std::string& prefix);
  void LoadSkeletonFile(const std::string& path);
  void LoadMeshFile(const std::string& filename);
  void LoadAttachmentWeights(const std::string& path);

  void ToggleDrawMode();
  void DecorateTree();

  void ComputeVertexNormals();

  DrawMode draw_mode_;
  // Euler angles of the UI sliders.
  std::vector<EulerAngle*> linked_angles_;

  std::vector<SceneNode*> joints_;

  std::shared_ptr<VertexObject> cylinder_mesh_ = PrimitiveFactory::CreateCylinder(0.01, 1.f, 10);
  std::shared_ptr<VertexObject> sphere_mesh_ = PrimitiveFactory::CreateSphere(0.015f, 25, 25);
  std::shared_ptr<ShaderProgram> shader_ = std::make_shared<PhongShader>();
  std::shared_ptr<Material> mat_ = std::make_shared<Material>(Material::GetDefault());
  std::shared_ptr<Material> mesh_mat_ = std::make_shared<Material>(Material::GetDefault());

  SceneNode* skin_ptr_;
  
  std::shared_ptr<VertexObject> bind_mesh_;
  std::shared_ptr<VertexObject> current_mesh_;
  
  std::vector<std::vector<float>> weights_;

  std::vector<glm::mat4> inverse_bind_mats_;
};
}  // namespace GLOO

#endif
