#ifndef BOID_H_
#define BOID_H_
#include <glm/glm.hpp>

class Boid {
    public: 
        Boid(){
            glm::vec3 position_ = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 velocity_ = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 acceleration_ = glm::vec3(1.0f, 0.0f, 0.0f);
            float close_range_ = 1.5f;
            float visible_range_ = 5.0f;
            float visible_angle_ = 3.14f;
        }

        Boid(float x, float y, float vx, float vy, float ax, float ay, float close_range, float visible_range) : 
                position_(glm::vec3{x, y, 0.f}), velocity_(glm::vec3{vx, vy, 0.f}), acceleration_{ax, ay, 0.f}, close_range_{close_range}, visible_range_{visible_range} {
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

        glm::vec3 get_position() const {
            return position_;
        };
        
        glm::vec3 get_velocity() const {
            return velocity_;
        };
        
        glm::vec3 get_acceleration() const {
            return acceleration_;
        };

        void set_velocity(const glm::vec3& vel) {
            velocity_ = vel;
        };

        void set_acceleration(const glm::vec3& acc) {
            acceleration_ = acc;
        };

        void set_position(const glm::vec3& pos) {
            position_ = pos;
        }


    private:
        glm::vec3 position_;
        glm::vec3 velocity_;
        glm::vec3 acceleration_;
        float close_range_;
        float visible_range_;
        float visible_angle_;

        void time_step(float dt) {
            velocity_ += acceleration_ * dt;
            position_ += velocity_ * dt;
        };
        // heading is just normalized velocity
        // do mesh and shit later
};
#endif