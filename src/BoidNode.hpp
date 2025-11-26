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
class BoidNode : SceneNode{
    public: 
        BoidNode(){
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 velocity_ = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 acceleration_ = glm::vec3(1.0f, 0.0f, 0.0f);

            this->GetTransform().SetPosition(position);

            float close_range_ = 1.5f;
            float visible_range_ = 5.0f;
            float visible_angle_ = 3.14f;
        }

        BoidNode(float x, float y, float vx, float vy, float ax, float ay, float close_range, float visible_range, float visible_angle) : 
                velocity_(glm::vec3{vx, vy, 0.f}), acceleration_{ax, ay, 0.f}, close_range_{close_range}, visible_range_    {visible_range}, visible_angle_{visible_angle} {
                
                glm::vec3 position = glm::vec3(x, y, 0.0f);
                this->GetTransform().SetPosition(position);
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

        void set_rotation(const glm::quat& rotation) {
            this->GetTransform().SetRotation(rotation);
        };

        void set_velocity(const glm::vec3& vel) {
            velocity_ = vel;
        };

        void set_acceleration(const glm::vec3& acc) {
            acceleration_ = acc;
        };


    private:
        float close_range_;
        float visible_range_;
        float visible_angle_;

        glm::vec3 velocity_;
        glm::vec3 acceleration_;

        void time_step(float dt) {
           
        };
        // heading is just normalized velocity
        // do mesh and shit later
};
} // namespace GLOO
#endif