#ifndef BOID_H_
#define BOID_H_
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
class BoidNode : public SceneNode{
    public: 
        BoidNode(){
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
            velocity_ = glm::vec3(1.0f, 1.0f, 1.0f);
            acceleration_ = glm::vec3(1.0f, 0.0f, 0.0f);

            CreateComponent<ShadingComponent>(shader_);
            CreateComponent<RenderingComponent>(sphere_mesh_);
            CreateComponent<MaterialComponent>(mat_);

            this->GetTransform().SetPosition(position);

            close_range_ = 1.5f;
            visible_range_ = 5.0f;
            visible_angle_ = 3.14f;
        }

        BoidNode(float x, float y, float z, float vx, float vy, float vz, float ax, float ay, float az, float close_range, float visible_range, float visible_angle) : 
            velocity_(glm::vec3{vx, vy, vz}), acceleration_{ax, ay, az}, close_range_{close_range}, visible_range_    {visible_range}, visible_angle_{visible_angle} {
            
            glm::vec3 position = glm::vec3(x, y, z);
            this->GetTransform().SetPosition(position);

            CreateComponent<ShadingComponent>(shader_);
            CreateComponent<RenderingComponent>(sphere_mesh_);
            CreateComponent<MaterialComponent>(mat_);
        };

        float get_close_range() const {
            return close_range_;
        };
        
        float get_visible_range() const {
            return visible_range_;
        };
        
        float get_visible_angle() const {
            return visible_angle_;
        };

        void set_close_range(float close_range) {
            close_range_ = close_range;
        };
        
        void set_visible_range(float visible_range) {
            visible_range_ = visible_range;
        };
        
        void set_visible_angle(float visible_angle) {
            visible_angle_ = visible_angle;
        };
        
        glm::quat get_rotation() const {
            return this->GetTransform().GetRotation();
        };
        
        void set_rotation(const glm::quat& rotation) {
            this->GetTransform().SetRotation(rotation);
        };

        Transform& get_transform() {
            return this->GetTransform();
        };

        glm::vec3 get_position() const {
            return this->GetTransform().GetPosition();
        };

        glm::vec3 get_velocity() const {
            return velocity_;
        };
        
        glm::vec3 get_acceleration() const {
            return acceleration_;
        };

        void set_position(const glm::vec3 position){
            this->GetTransform().SetPosition(position);
        };

        void set_velocity(const glm::vec3& vel) {
            velocity_ = vel;
            if (glm::length(velocity_) > max_speed_) {
                velocity_ = glm::normalize(velocity_) * max_speed_;
            }
        };

        void set_acceleration(const glm::vec3& acc) {
            acceleration_ = acc;
            if (glm::length(acceleration_) > max_acc_) {
                acceleration_ = glm::normalize(acceleration_) * max_acc_;
            }
        };


    private:
        float close_range_;
        float visible_range_;
        float visible_angle_;

        glm::vec3 velocity_;
        glm::vec3 acceleration_;

        float max_speed_ = 1.f;
        float max_acc_ = 0.05;

        void time_step(float dt) {
           
        };

        std::shared_ptr<VertexObject> sphere_mesh_ = PrimitiveFactory::CreateSphere(1.f, 25, 25);
        std::shared_ptr<ShaderProgram> shader_ = std::make_shared<PhongShader>();
        std::shared_ptr<Material> mat_ = std::make_shared<Material>(Material::GetDefault());
        // heading is just normalized velocity
        // do mesh and shit later
};
} // namespace GLOO
#endif