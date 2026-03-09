#ifndef SWAP_CHAIN_TARGETS_H_INCLUDED
#define SWAP_CHAIN_TARGETS_H_INCLUDED

#include "Helpers/d3dUtil.h"

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class Renderer;

class SwapChainTargets
{
public:
    SwapChainTargets(Renderer* pRenderer, uint32_t width, uint32_t height);
	~SwapChainTargets();

    bool CreateSwapChain(HWND hwnd, uint32_t width, uint32_t height);
	bool CreateRtvDsvHeaps();
    void CreateRenderTargets();
    void CreateDepthStencil(uint32_t width, uint32_t height);

	void Resize(uint32_t width, uint32_t height);

    void UpdateCurrentBackBuffer();

	IDXGISwapChain* GetSwapChain() const { return m_swapChain; }
	ID3D12DescriptorHeap* GetRtvHeap() const { return m_rtvHeap; }
	ID3D12DescriptorHeap* GetDsvHeap() const { return m_dsvHeap; }
	ID3D12Resource* GetCurrentBackBuffer() const { return m_swapChainBuffer[m_currBackBuffer]; }
	UINT GetCurrentBackBufferIndex() const { return m_currBackBuffer; }

    D3D12_VIEWPORT* GetScreenViewport() { return &m_screenViewport; }
	D3D12_RECT* GetScissorRect() { return &m_scissorRect; }

	void Shutdown();

private:
	Renderer* m_pRenderer = nullptr;

    IDXGISwapChain* m_swapChain = nullptr;

    static const uint8_t SwapChainBufferCount = 2;
    uint8_t m_currBackBuffer = 0;
    ID3D12Resource* m_swapChainBuffer[SwapChainBufferCount];
    ID3D12Resource* m_depthStencilBuffer = nullptr;

    ID3D12DescriptorHeap* m_rtvHeap = nullptr;
    ID3D12DescriptorHeap* m_dsvHeap = nullptr;
    SwapChainTargets* m_pSwapChainTargets = nullptr;

    D3D12_VIEWPORT m_screenViewport;
    D3D12_RECT m_scissorRect;

    // Derived class should set these in derived constructor to customize starting values.
    std::wstring m_mainWindowCaption = L"Window";
    D3D_DRIVER_TYPE m_d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
    DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
};

#endif // !SWAP_CHAIN_TARGETS_H_INCLUDED
