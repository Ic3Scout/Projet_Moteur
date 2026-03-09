#ifndef WINDOW_CPP_INCLUDED
#define WINDOW_CPP_INCLUDED

#include "Window.h"
#include "Core/InputSystem.h"
#include "Core/InputsMapper.h"

Window* Window::s_pInstance = nullptr;

Window::Window(int width, int height, const wchar_t* title)
{
    if (s_pInstance != nullptr)
        throw std::runtime_error("Window: une instance existe d�j�.");

    s_pInstance = this;

    m_width = width;
    m_height = height;

    HINSTANCE hInst = GetModuleHandle(nullptr);

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = StaticWindowProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"Window";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClassEx(&wc);

    RECT r{ 0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };
    AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);

    m_hWindow = CreateWindowEx(
        0,
        wc.lpszClassName,
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        r.right - r.left,
        r.bottom - r.top,
        nullptr, nullptr,
        hInst,
        nullptr
    );

    ShowWindow(m_hWindow, SW_SHOW);
}

Window::~Window()
{
    if (m_hWindow)
        DestroyWindow(m_hWindow);

    s_pInstance = nullptr;
}

bool Window::ProcessMessages()
{
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            return false;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return true;
}

Window* Window::GetInstance()
{
    return s_pInstance;
}

HWND Window::GetHandle() const
{
    return m_hWindow;
}

LRESULT Window::StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (s_pInstance == nullptr)
        return DefWindowProc(hwnd, msg, wParam, lParam);

    return s_pInstance->WindowProc(hwnd, msg, wParam, lParam);
}

LRESULT Window::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	using namespace core;

    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
		m_isOpen = false;
        return 0;
    case WM_SIZE:
        RECT rc;
		GetClientRect(hwnd, &rc);
        m_width = rc.right - rc.left;
        m_height = rc.bottom - rc.top;
		m_resizing = true;
		break;
    case WM_KEYDOWN:
        InputSystem::SetKey(MapVKToKey(wParam), true);
        break;

    case WM_KEYUP:
        InputSystem::SetKey(MapVKToKey(wParam), false);
        break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        InputSystem::SetMouseButton(
            MapMouseButton(msg),
            msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN
        );
        break;

    case WM_MOUSEMOVE:
        if (!m_cursorLocked)
        {
            InputSystem::SetMousePosition(
                LOWORD(lParam),
                HIWORD(lParam)
            );
        }
        break;
    case WM_KILLFOCUS:
        LockCursor(false);
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void Window::SetFullscreen(bool fullscreen)
{ 
    m_fullscreen = fullscreen; 
}

void Window::UpdateCursorCenter()
{
    RECT rc;
    GetClientRect(m_hWindow, &rc);

    POINT center;
    center.x = (rc.right - rc.left) / 2;
    center.y = (rc.bottom - rc.top) / 2;

    ClientToScreen(m_hWindow, &center);
    m_cursorCenter = center;
}

void Window::LockCursor(bool enable)
{
    if (enable == m_cursorLocked)
        return;

    m_cursorLocked = enable;

    if (enable)
    {
        UpdateCursorCenter();

        RECT rect;
        GetClientRect(m_hWindow, &rect);
        ClientToScreen(m_hWindow, (POINT*)&rect.left);
        ClientToScreen(m_hWindow, (POINT*)&rect.right);

        ClipCursor(&rect);
        while (ShowCursor(FALSE) >= 0);
        SetCursorPos(m_cursorCenter.x, m_cursorCenter.y);
    }
    else
    {
        ClipCursor(nullptr);
        while (ShowCursor(TRUE) < 0);
    }
}

#endif // !WINDOW_CPP_INCLUDED