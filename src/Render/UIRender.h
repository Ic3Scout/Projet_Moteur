#ifndef UI_RENDER_H_INCLUDED
#define UI_RENDER_H_INCLUDED

#include "Helpers/d3dUtil.h"

using namespace DirectX;

class DxContext;
class UploadContext;
class Texture2D;
class Pipeline;

#include "Render/DescriptorHeapManager.h"

struct UIVertex
{
	XMFLOAT2 Pos;
	XMFLOAT2 UV;
	XMFLOAT4 color;
};

struct UIConstants
{
	float gTint[4]; // rgba
};

struct UIFrame 
{ 
	bool showSplash = false;
	bool showScore = false;
	bool showCrosshair = false;
	int score = 0;
};

struct Glyph
{
	int x, y, w, h;       // pixels dans l'atlas
	int xOffset, yOffset; // offset sur la ligne
	int xAdvance;         // avance après ce caractère
};

struct SpriteFont
{
	int atlasW = 0;
	int atlasH = 0;
	int lineHeight = 0;
	int base = 0;
	Glyph glyphs[256] = {};   // ASCII
};

class UIRender
{
public:
	UIRender() = default;
	~UIRender() { Shutdown(); }

	bool Initialize(DxContext* dx, UploadContext* uploader, DescriptorHeapManager* heap, Pipeline* pipeline);
	void Render(ID3D12GraphicsCommandList* cmd, float screenW, float screenH, const UIFrame& ui);

	void UpdateTint(float r, float g, float b, float a);

	D3D12_GPU_VIRTUAL_ADDRESS GetTintCBAddress() const { return m_uiCB ? m_uiCB->GetGPUVirtualAddress() : 0; }

	void Shutdown();

private:
	bool CreateFullscreenQuadVB(ID3D12Device* device);
	bool CreateTintConstantBuffer(ID3D12Device* device);

	bool CreateTextVB(ID3D12Device* device, uint32_t maxChars);
	UINT CreateTextVertices(const char* text, float x, float y, float scale,
		float screenW, float screenH, const XMFLOAT4& color, UINT startVertex);

	void DrawSplash(ID3D12GraphicsCommandList* cmd);
	void DrawScoreCrosshairBatch(ID3D12GraphicsCommandList* cmd, float screenW, float screenH, const UIFrame& ui);

	float PxToNdcX(float x, float screenW) { return (x / screenW) * 2.0f - 1.0f; }
	float PxToNdcY(float y, float screenH) { return 1.0f - (y / screenH) * 2.0f; }

	bool ParseIntField(const std::string& token, const char* key, int& out);
	bool LoadBMFontText(const std::string& path, SpriteFont& font);

	DxContext* m_dx = nullptr;
	UploadContext* m_uploader = nullptr;
	Pipeline* m_pipeline = nullptr;
	DescriptorHeapManager* m_srvHeap = nullptr;

	ID3D12Resource* m_uiCB = nullptr;
	uint8_t* m_uiCBMapped = nullptr;

	// Splash
	Texture2D* m_splashTex = nullptr;
	DescriptorHandle m_splashSrv;

	ID3D12Resource* m_quadVB = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_quadVBV{};

	// Score
	Texture2D* m_fontAtlas = nullptr;
	DescriptorHandle m_fontSrv;
	SpriteFont m_fontData;

	ID3D12Resource* m_textVB = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_textVBV{};
	uint8_t* m_textVBMapped = nullptr;
	UINT m_textVBMaxVerts = 0;
	UINT m_textMaxChars = 0;
};

#endif // !UI_RENDER_H_INCLUDED