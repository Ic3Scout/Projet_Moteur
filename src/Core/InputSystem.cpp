#include "InputSystem.h"

namespace core
{

    std::array<InputState::State, Keyboard::AMOUNT> InputSystem::s_keys{};
    std::array<InputState::State, Mouse::AMOUNT>    InputSystem::s_mouseButtons{};

    float InputSystem::s_mouseX = 0;
    float InputSystem::s_mouseY = 0;

    void InputSystem::NewFrame()
    {
        for (auto& k : s_keys)
        {
            if (k == InputState::DOWN)
                k = InputState::PRESSED;
            else if (k == InputState::UP)
                k = InputState::NONE;
        }

        for (auto& b : s_mouseButtons)
        {
            if (b == InputState::DOWN)
                b = InputState::PRESSED;
            else if (b == InputState::UP)
                b = InputState::NONE;
        }
    }

    void InputSystem::SetKey(Keyboard::Key key, bool pressed)
    {
        auto& state = s_keys[key];

        if (pressed)
        {
            if (state == InputState::NONE || state == InputState::UP)
                state = InputState::DOWN;
        }
        else
        {
            if (state == InputState::PRESSED || state == InputState::DOWN)
                state = InputState::UP;
        }
    }

    void InputSystem::SetMouseButton(Mouse::Button button, bool pressed)
    {
        auto& state = s_mouseButtons[button];

        if (pressed)
        {
            if (state == InputState::NONE || state == InputState::UP)
                state = InputState::DOWN;
        }
        else
        {
            if (state == InputState::PRESSED || state == InputState::DOWN)
                state = InputState::UP;
        }
    }

    void InputSystem::SetMousePosition(float x, float y)
    {
        s_mouseX = x;
        s_mouseY = y;
    }

}
