#pragma once

#include <SDL2/SDL.h>

#include <iostream>
#include <stdexcept>
#include <stack>

#include "Entity.hpp"
#include "Window.hpp"
#include "Sprite.hpp"
#include "Event.hpp"

enum class PlayerState {
    MOVING, ATTACK
};

class Player : public Entity {
public:
    Player() :
        x(Window::center_x()),
        y(Window::center_y()),
        speed_x(0.0f),
        speed_y(0.0f),
        facing_left(false),
        on_ground(false),
        bounding_box({20, 16, 22, 28}),
        run(Sprite("sprites/playerrun.png", 78, 58, bounding_box, 8, 10)),
        idle(Sprite("sprites/playeridle.png", 78, 58, bounding_box, 11, 10)),
        attack(Sprite("sprites/playerattack.png", 78, 58, bounding_box, 3, 10)),
        jump(Sprite("sprites/playerjump.png", 78, 58, bounding_box, 1, 10)),
        fall(Sprite("sprites/playerfall.png", 78, 58, bounding_box, 1, 10))
    {
        states.push(PlayerState::MOVING);
    }

    void paint() override {
        assert (!states.empty());
        switch (states.top()) {
            case PlayerState::ATTACK:
                attack.paint(x, y, facing_left);
                break;
            case PlayerState::MOVING:
                if (speed_y < 0.0f) {
                    jump.paint(x, y, facing_left);
                } else if (speed_y > 0.0f) {
                    fall.paint(x, y, facing_left);
                } else if (speed_x != 0) {
                    run.paint(x, y, facing_left);
                } else if (speed_x == 0) {
                    idle.paint(x, y, facing_left);
                } else {
                    throw runtime_error("Unexpected state");
                }
                break;
            default:
                throw runtime_error("Unexpected state");
        }
    }

    void handle_event(Event e) override {
        switch (e) {
            case Event::LEFT_PRESS:
                facing_left = true;
                speed_x = -0.5f;
                break;
            case Event::RIGHT_PRESS:
                facing_left = false;
                speed_x = 0.5f;
                break;
            case Event::LEFT_RELEASE:
                if (speed_x < 0) {
                    speed_x = 0.0f;
                }
                break;
            case Event::RIGHT_RELEASE:
                if (speed_x > 0) {
                    speed_x = 0.0f;
                }
                break;
            case Event::ATTACK:
                if (states.top() != PlayerState::ATTACK) {
                    states.push(PlayerState::ATTACK);
                    attack.set_first_frame();
                }
                break;
            case Event::JUMP:
                if (on_ground) {
                    on_ground = false;
                    y -= 1.0f;
                    speed_y -= 1.0f;
                }
            default:
                break;
        }
    }

    void tick() override {
        float ground_level = 288.0f;
        if (y + bounding_box.h >= ground_level) {
            y = ground_level - bounding_box.h;
            on_ground = true;
            speed_y = 0;
        }
        x += speed_x;
        y += speed_y;
        if (!on_ground) {
            speed_y += 0.008f;
        }
        if (states.top() == PlayerState::ATTACK && attack.frames_elapsed() >= attack.get_num_frames()) {
            states.pop();
        }
    }

private:
    float x, y; // the top left corner of the player
    float speed_x, speed_y;
    bool facing_left, on_ground;
    SDL_Rect bounding_box; // in coordinates relative to the top left of the frame.
    stack<PlayerState> states;
    Sprite run, idle, attack, jump, fall;
};