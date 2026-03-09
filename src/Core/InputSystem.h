#pragma once
#include "InputsEnums.h"
#include <array>

namespace core
{
    class InputSystem
    {
    public:
        static void NewFrame();

        static void SetKey(Keyboard::Key key, bool pressed);
        static void SetMouseButton(Mouse::Button button, bool pressed);
        static void SetMousePosition(float x, float y);

        static std::array<InputState::State, Keyboard::AMOUNT> s_keys;
        static std::array<InputState::State, Mouse::AMOUNT>    s_mouseButtons;

        static float s_mouseX;
        static float s_mouseY;
    };
}
