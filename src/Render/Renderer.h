#ifndef RENDERER_H_DEFINED
#define RENDERER_H_DEFINED

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "Helpers/d3dUtil.h"

//d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

#include "DescriptorHeapManager.h"
#include "UIRender.h"

class Window;
class DxContext;
class SwapChainTargets;
class Pipeline;
class UploadContext;
class CameraComponent;
class MeshRendererComponent;
class LightRender;

using namespace DirectX;

class Renderer
{
public:
	Renderer() = default;
	~Renderer() { Shutdown(); }

	bool Initialize(Window* window, CameraComponent* camera);
	void Shutdown();

	void Update();
	void Render(std::vector<MeshRendererComponent*> vMesh);
	void DrawObj(MeshRendererComponent& obj);
	XMFLOAT4X4 BuildWorldViewProjMatrix(XMMATRIX& world);

	void SetUiFrame(const UIFrame& frame) { m_uiFrame = frame; }

	Window* GetWindow() const { return m_pWindow; }
	DxContext* GetDxContext() const { return m_pDxContext; }
	SwapChainTargets* GetSwapChainTargets() const { return m_pSwapChainTargets; }
	Pipeline* GetPipeline() const { return m_pPipeline; }
	DescriptorHeapManager* GetDescriptorHeapManager() const { return m_pDescriptorHeapManager; }
	UploadContext* GetUploadContext() const { return m_pUploadContext; }

private:	
    void BeginFrame();
    void EndFrame();

	XMFLOAT4X4 GetViewProjMatrix();

	Window* m_pWindow = nullptr;
	CameraComponent* m_pCamera = nullptr;

	LightRender* m_pLightRender = nullptr;

	UIRender* m_pUiRender = nullptr;
	UIFrame m_uiFrame;

	DxContext* m_pDxContext = nullptr;
	SwapChainTargets* m_pSwapChainTargets = nullptr;
	Pipeline* m_pPipeline = nullptr;
	DescriptorHeapManager* m_pDescriptorHeapManager = nullptr;
	UploadContext* m_pUploadContext = nullptr;
};

#endif
