#ifndef WINDOW_H_DEFINED
#define WINDOW_H_DEFINED

#include "Helpers/d3dUtil.h"
class Renderer;

class Window
{
public:
    Window(int width, int height, const wchar_t* title);
    ~Window();

    bool ProcessMessages();

    static Window* GetInstance();
    HWND GetHandle() const;

    uint32_t GetWidth() const { return m_width; }
    uint32_t GetHeight() const { return m_height; }

	bool IsMinimized() const { return m_minimized; }
	bool IsMaximized() const { return m_maximized; }
	bool IsResizing() const { return m_resizing; }
	bool IsFullscreen() const { return m_fullscreen; }

	void SetMinimized(bool minimized) { m_minimized = minimized; }
	void SetMaximized(bool maximized) { m_maximized = maximized; }
	void SetResizing(bool resizing) { m_resizing = resizing; }
    void SetFullscreen(bool fullscreen);

    void LockCursor(bool enable);
    void UpdateCursorCenter();

    bool IsCursorLocked() const { return m_cursorLocked; }
    const POINT& GetCursorCenter() const { return m_cursorCenter; }
 
	bool IsOpen() const { return m_isOpen; }

private:
    static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static Window* s_pInstance;

    HWND m_hWindow = nullptr;

	uint32_t m_width = 0;
	uint32_t m_height = 0;

    bool m_minimized = false;   // is the application minimized?
    bool m_maximized = false;   // is the application maximized?
    bool m_resizing = false;    // are the resize bars being dragged?
    bool m_fullscreen = false;  // fullscreen enabled
	bool m_isOpen = true;      // is the application running?

    // Cursor state
    bool m_cursorLocked = false;
    POINT m_cursorCenter{};
};

#endif // !ENGINE_WINDOW_H