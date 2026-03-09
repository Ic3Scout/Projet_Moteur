#pragma once
#include "InputsEnums.h"
#include <vector>

namespace core
{
    namespace Input
    {
        bool GetKey(Keyboard::Key key);
        bool GetKeyDown(Keyboard::Key key);
        bool GetKeyUp(Keyboard::Key key);

        bool GetMouseButton(Mouse::Button button);
        bool GetMouseButtonDown(Mouse::Button button);
        bool GetMouseButtonUp(Mouse::Button button);

        float GetMouseX();
        float GetMouseY();
        std::vector<float> GetMousePosition();

        void SetMousePosition(float x, float y);

        void Update();

        static bool m_preventDeltaUpdate = false;
        static std::vector<float> m_mousePosLastFrame;
        
    }
}
