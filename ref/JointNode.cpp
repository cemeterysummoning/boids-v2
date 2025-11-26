#include "JointNode.hpp"

#include <fstream>

#include "gloo/utils.hpp"
#include "gloo/InputManager.hpp"
#include "gloo/MeshLoader.hpp"

#include "gloo/shaders/PhongShader.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"

namespace GLOO {
    JointNode::JointNode(glm::vec3 position) {
        CreateComponent<ShadingComponent>(shader_);
        CreateComponent<RenderingComponent>(sphere_mesh_);
        CreateComponent<MaterialComponent>(mat_);

        GetTransform().SetPosition(position);
    }
}