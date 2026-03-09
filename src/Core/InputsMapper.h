#pragma once
#include "Core/InputsEnums.h"
#include <Windows.h>

namespace core
{
	Keyboard::Key MapVKToKey(WPARAM vkCode);
	Mouse::Button MapMouseButton(UINT msg);
}

