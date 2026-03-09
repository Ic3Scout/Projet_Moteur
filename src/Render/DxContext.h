#ifndef DXCONTEXT_H_INCLUDED
#define DXCONTEXT_H_INCLUDED

#include "Helpers/d3dUtil.h"

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class DxContext
{
public:
	DxContext();
	~DxContext();

    bool CreateDevice();
	bool CreateCommandObjects();

	void WaitForGpu();
	void FlushCommandQueue();

	void Shutdown();

	IDXGIFactory4* GetFactory()  { return m_dxgiFactory; }
	ID3D12Device* GetDevice()  { return m_d3dDevice; }
	ID3D12Fence* GetFence()  { return m_fence; }
	UINT64 GetCurrentFence()  { return m_currentFence; }
	ID3D12CommandQueue* GetCommandQueue()  { return m_commandQueue; }
	ID3D12CommandAllocator* GetCommandAllocator()  { return m_directCmdListAlloc; }
	ID3D12GraphicsCommandList* GetCommandList()  { return m_commandList; }
    
	UINT GetRtvDescriptorSize() const { return m_rtvDescriptorSize; }
    UINT GetDsvDescriptorSize() const { return m_dsvDescriptorSize; }
	UINT GetCbvSrvUavDescriptorSize() const { return m_cbvSrvUavDescriptorSize; }

private:
    IDXGIFactory4* m_dxgiFactory = nullptr;
    ID3D12Device* m_d3dDevice = nullptr;

    ID3D12Fence* m_fence = nullptr;
    UINT64 m_currentFence = 0;

    ID3D12CommandQueue* m_commandQueue = nullptr;
    ID3D12CommandAllocator* m_directCmdListAlloc = nullptr;
    ID3D12GraphicsCommandList* m_commandList = nullptr;

    UINT m_rtvDescriptorSize = 0;
    UINT m_dsvDescriptorSize = 0;
    UINT m_cbvSrvUavDescriptorSize = 0;
};

#endif // !DXCONTEXT_H_INCLUDED