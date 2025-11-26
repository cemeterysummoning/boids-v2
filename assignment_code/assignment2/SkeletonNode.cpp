#include "SkeletonNode.hpp"

#include <fstream>

#include "gloo/utils.hpp"
#include "gloo/InputManager.hpp"
#include "gloo/MeshLoader.hpp"

#include "gloo/shaders/PhongShader.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/shaders/SimpleShader.hpp"

#include <typeinfo>

#include "JointNode.hpp"
#include "BoneNode.hpp"

void print_v(glm::vec3 v) {
  std::cout << v[0] << ' ' << v[1] << ' ' << v[2] << '\n';
}

namespace GLOO {
SkeletonNode::SkeletonNode(const std::string& filename)
    : SceneNode(), draw_mode_(DrawMode::Skeleton) {
  LoadAllFiles(filename);
  DecorateTree();

  mesh_mat_->SetDiffuseColor(glm::vec3{1.f, 1.f, 1.f});
  mesh_mat_->SetAmbientColor(glm::vec3{1.f, 1.f, 1.f});
  mesh_mat_->SetSpecularColor(glm::vec3{1.f, 1.f, 1.f});

  // Force initial update.
  OnJointChanged();
}

void SkeletonNode::ToggleDrawMode() {
  draw_mode_ =
      draw_mode_ == DrawMode::Skeleton ? DrawMode::SSD : DrawMode::Skeleton;
  // TODO: implement here toggling between skeleton mode and SSD mode.
  // The current mode is draw_mode_;
  // Hint: you may find SceneNode::SetActive convenient here as
  // inactive nodes will not be picked up by the renderer.
  if (draw_mode_ == DrawMode::SSD) {
    skin_ptr_->SetActive(true);
  } else {
    skin_ptr_->SetActive(false);
  }
}

void SkeletonNode::DecorateTree() {
  // TODO: set up addtional nodes, add necessary components here.
  // You should create one set of nodes/components for skeleton mode
  // (spheres for joints and cylinders for bones), and another set for
  // SSD mode (you could just use a single node with a RenderingComponent
  // that is linked to a VertexObject with the mesh information. Then you
  // only need to update the VertexObject - updating vertex positions and
  // recalculating the normals, etc.).

  // The code snippet below shows how to add a sphere node to a joint.
  // Suppose you have created member variables shader_ of type
  // std::shared_ptr<PhongShader>, and sphere_mesh_ of type
  // std::shared_ptr<VertexObject>.
  // Here sphere_nodes_ptrs_ is a std::vector<SceneNode*> that stores the
  // pointer so the sphere nodes can be accessed later to change their
  // positions. joint_ptr is assumed to be one of the joint node you created
  // from LoadSkeletonFile (e.g. you've stored a std::vector<SceneNode*> of
  // joint nodes as a member variable and joint_ptr is one of the elements).
  //

  for (int i = 1; i < joints_.size(); i++) {
    // we already set the position of this child node in initialization, so the sphere should be at the origin of its own frame.
    auto sphere_node = make_unique<JointNode>(glm::vec3(0.f, 0.f, 0.f)); 

    joints_.at(i)->AddChild(std::move(sphere_node));
    
  }
  
  for (int i = 1; i < joints_.size(); i++) {
    if (joints_.at(i)->GetParentPtr() == joints_.at(0)) { // skips root node
      continue;
    }
    auto displacement = joints_.at(i)->GetTransform().GetPosition();

    auto cylinder_node = make_unique<BoneNode>(glm::vec3(0.f, 0.f, 0.f));
    cylinder_node->GetTransform().SetScale(glm::vec3(1.0f, glm::length(displacement), 1.0f));
    
    glm::vec3 c = glm::vec3{0.f, 1.f, 0.f};
    glm::vec3 rot_axis = glm::normalize(glm::cross(c, displacement));
    float angle = acos(glm::dot(c, glm::normalize(displacement)));
    cylinder_node->GetTransform().SetRotation(rot_axis, angle);
    joints_.at(i)->GetParentPtr()->AddChild(std::move(cylinder_node));
  }
  std::cout << inverse_bind_mats_.size() << '\n';

  auto skin = make_unique<SceneNode>();
  skin->CreateComponent<ShadingComponent>(shader_);
  skin->CreateComponent<RenderingComponent>(current_mesh_);
  skin->CreateComponent<MaterialComponent>(mesh_mat_);
  ComputeVertexNormals();
  skin->GetTransform().SetPosition(-joints_.at(0)->GetTransform().GetPosition());
  skin_ptr_ = skin.get();
  skin->SetActive(false);

  joints_.at(0)->AddChild(std::move(skin));
}

void SkeletonNode::ComputeVertexNormals() {
  auto bind_normals = make_unique<NormalArray>();
  auto current_normals = make_unique<NormalArray>();
  auto positions = bind_mesh_->GetPositions();
  auto indices = bind_mesh_->GetIndices();

  std::vector<glm::vec3> temp_normals;

  // initialize vector array for normal averaging
  for (int i = 0; i < positions.size(); i++) {
    temp_normals.push_back(glm::vec3{0.f, 0.f, 0.f});
  }

  // for each face
  for (int i = 0; i < indices.size(); i += 3) {
    int i1 = indices.at(i);
    int i2 = indices.at(i + 1);
    int i3 = indices.at(i + 2);

    glm::vec3 v1 = positions.at(i1);
    glm::vec3 v2 = positions.at(i2);
    glm::vec3 v3 = positions.at(i3);

    glm::vec3 n = glm::cross(v2 - v1, v3 - v1);
    // vertex 1
    temp_normals.at(i1) = temp_normals.at(i1) + n;
    
    // vertex 2
    temp_normals.at(i2) = temp_normals.at(i2) + n;

    // vertex 3
    temp_normals.at(i3) = temp_normals.at(i3) + n;
  }

  for (glm::vec3& per_vertex : temp_normals) {
    bind_normals->push_back(glm::normalize(per_vertex));
    current_normals->push_back(glm::normalize(per_vertex));
  }
  bind_mesh_->UpdateNormals(std::move(bind_normals));
  current_mesh_->UpdateNormals(std::move(current_normals));
}

void SkeletonNode::Update(double delta_time) {
  // Prevent multiple toggle.
  static bool prev_released = true;
  if (InputManager::GetInstance().IsKeyPressed('S')) {
    if (prev_released) {
      ToggleDrawMode();
    }
    prev_released = false;
  } else if (InputManager::GetInstance().IsKeyReleased('S')) {
    prev_released = true;
  }
}

void SkeletonNode::OnJointChanged() {
  // TODO: this method is called whenever the values of UI sliders change.
  // The new Euler angles (represented as EulerAngle struct) can be retrieved
  // from linked_angles_ (a std::vector of EulerAngle*).
  // The indices of linked_angles_ align with the order of the joints in .skel
  // files. For instance, *linked_angles_[0] corresponds to the first line of
  // the .skel file.

  auto updated_vertices = make_unique<PositionArray>();
  auto updated_normals = make_unique<NormalArray>();

  for (int i = 0; i < linked_angles_.size(); i++) {
    auto angle = linked_angles_.at(i);
    joints_.at(i)->GetTransform().SetRotation(glm::quat(glm::vec3(angle->rx, angle->ry, angle->rz)));
  }
  
  for (int i = 0; i < bind_mesh_->GetPositions().size(); i++) {
    glm::mat4 weighted_transform = glm::mat4(0.f);
    glm::vec4 p = glm::vec4(bind_mesh_->GetPositions().at(i), 1.f);
    glm::vec4 n = glm::vec4(bind_mesh_->GetNormals().at(i), 0.f);
    for (int j = 1; j < joints_.size(); j++) {
      if (weights_.at(i).at(j - 1) == 0.f) continue;

      glm::mat4 T_j = joints_.at(j)->GetTransform().GetLocalToAncestorMatrix(this);
      glm::mat4 B_j_inv = inverse_bind_mats_.at(j - 1);
      weighted_transform = weighted_transform + weights_.at(i).at(j - 1) * T_j * B_j_inv;
    }
    
    updated_vertices->push_back(weighted_transform * p);
    updated_normals->push_back(glm::normalize(glm::inverse(glm::transpose(weighted_transform)) * n));
  }

  current_mesh_->UpdatePositions(std::move(updated_vertices));
  current_mesh_->UpdateNormals(std::move(updated_normals));
}

void SkeletonNode::LinkRotationControl(const std::vector<EulerAngle*>& angles) {
  linked_angles_ = angles;
}

void SkeletonNode::LoadSkeletonFile(const std::string& path) {
  // TODO: load skeleton file and build the tree of joints.
  std::fstream s{path};
  std::string line;

  SceneNode* root;

  if (!s.is_open()) {
    std::cout << "Failed to open file" << '\n';
  } else {
    while (std::getline(s, line)) {
      glm::vec3 displacement;
      int parent;

      std::istringstream sl(line);
      sl >> displacement[0] >> displacement[1] >> displacement[2];
      sl >> parent;

      auto joint = make_unique<SceneNode>();
      joint->GetTransform().SetPosition(displacement);

      auto joint_ptr = joint.get();

      joints_.push_back(std::move(joint_ptr));

      if (parent > -1) {
        joints_.at(parent)->AddChild(std::move(joint));
        inverse_bind_mats_.push_back(glm::inverse(joint_ptr->GetTransform().GetLocalToAncestorMatrix(this)));
      } else {
        root = joint.get();
        this->AddChild(std::move(joint));
      }
    }
  }
}

void SkeletonNode::LoadMeshFile(const std::string& filename) {
  // TODO: store the bind pose mesh in your preferred way.
  bind_mesh_ = MeshLoader::Import(filename).vertex_obj;
  current_mesh_ = MeshLoader::Import(filename).vertex_obj;
}

void SkeletonNode::LoadAttachmentWeights(const std::string& path) {
  // TODO: load attachment weights.
  std::fstream s{path};
  std::string line;

  if (!s.is_open()) {
    std::cout << "failed to open file" << '\n';
  } else {
    while (std::getline(s, line)) {
      std::vector<float> l;

      std::istringstream sl(line);

      for (int i = 0; i < joints_.size() - 1; i++) {
        float f;
        sl >> f;

        l.push_back(f);
      }

      weights_.push_back(l);
    }
  }
}

void SkeletonNode::LoadAllFiles(const std::string& prefix) {
  std::string prefix_full = GetAssetDir() + prefix;
  LoadSkeletonFile(prefix_full + ".skel");
  LoadMeshFile(prefix + ".obj");
  LoadAttachmentWeights(prefix_full + ".attach");
}
}  // namespace GLOO
