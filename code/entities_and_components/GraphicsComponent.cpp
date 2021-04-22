#include "SDL2/SDL.h"

#include <stdexcept>

#include "Entity.hpp"
#include "Sprite.hpp"
#include "Camera.hpp"
#include "PhysicsComponent.hpp"

#include "GraphicsComponent.hpp"

GraphicsComponent::GraphicsComponent(AnimatedSprite&& run, AnimatedSprite&& idle, AnimatedSprite&& attack, AnimatedSprite&& jump, 
                                     AnimatedSprite&& fall, AnimatedSprite&& ground, AnimatedSprite&& dead) :
    run(std::move(run)),
    idle(std::move(idle)),
    attack(std::move(attack)),
    jump(std::move(jump)),
    fall(std::move(fall)),
    ground(std::move(ground)),
    dead(std::move(dead))
{}

void GraphicsComponent::paint(Entity& me) {
    if ((me.current_state() == EntityState::ATTACK && me.graphics->attack.animation_complete()) || 
        (me.current_state() == EntityState::GROUND && me.graphics->ground.animation_complete())) {
        me.states.pop();
    }
    auto [screen_x, screen_y] = Camera::convert_to_screen_coordinates(me.x, me.y);
    switch (me.current_state()) {
        case EntityState::ATTACK:
            attack.paint(screen_x, screen_y, me.facing_left);
            break;
        case EntityState::GROUND:
            ground.paint(screen_x, screen_y, me.facing_left);
            break;
        case EntityState::DEAD:
            dead.paint(screen_x, screen_y, me.facing_left);
            break;
        case EntityState::MOVING:
            if (me.speed_y < 0.0f) {
                jump.paint(screen_x, screen_y, me.facing_left);
            } else if (me.speed_y > 0.0f && !me.physics->on_ground()) {
                fall.paint(screen_x, screen_y, me.facing_left);
            } else if (me.speed_x != 0) {
                run.paint(screen_x, screen_y, me.facing_left);
            } else if (me.speed_x == 0) {
                idle.paint(screen_x, screen_y, me.facing_left);
            } else {
                throw std::runtime_error("Unexpected state");
            }
            break;
        default:
            throw std::runtime_error("Unexpected state");
    }
}