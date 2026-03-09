#ifndef UI_RENDERER_CPP_INCLUDED
#define UI_RENDERER_CPP_INCLUDED

#include "UIRender.h"

#include "DxContext.h" 
#include "UploadContext.h"
#include "Pipeline.h"
#include "Texture2D.h"

bool UIRender::Initialize(DxContext* dx, UploadContext* uploader, DescriptorHeapManager* heap, Pipeline* pipeline)
{
	m_dx = dx;
    m_uploader = uploader;
    m_srvHeap = heap;
    m_pipeline = pipeline;

    m_splashTex = new Texture2D();
    if (m_splashTex->LoadFromFileWIC(dx->GetDevice(), uploader, heap, L"../../res/SplashScreen.png", true) == false)
        return false;

    // IMPORTANT: le SRV est déjà alloué via srvHeap->Allocate() dans Texture2D. 
    m_splashSrv = m_splashTex->GetSrv();

    // 2) VB pour quad fullscreen
    if (CreateFullscreenQuadVB(dx->GetDevice()) == false) return false;
	if (CreateTintConstantBuffer(dx->GetDevice()) == false) return false;

    m_fontAtlas = new Texture2D();
    if (m_fontAtlas->LoadFromFileWIC(dx->GetDevice(), uploader, heap, L"../../res/Fonts/minecraft_32.png", false) == false)
		return false;
    m_fontSrv = m_fontAtlas->GetSrv();

    if (CreateTextVB(dx->GetDevice(), 64) == false) return false;
    if (LoadBMFontText("../../res/Fonts/minecraft_32.fnt", m_fontData) == false) return false;

	return true;
}

void UIRender::Render(ID3D12GraphicsCommandList* cmd, float screenW, float screenH, const UIFrame& ui)
{
    if (cmd == nullptr) return;

    if (ui.showSplash) 
        DrawSplash(cmd);

	DrawScoreCrosshairBatch(cmd, screenW, screenH, ui);
}

void UIRender::UpdateTint(float r, float g, float b, float a)
{
    uint8_t* dst = m_uiCBMapped;

    float* f = reinterpret_cast<float*>(dst);
    f[0] = r; f[1] = g; f[2] = b; f[3] = a;
}

void UIRender::Shutdown()
{
    if (m_quadVB) 
    { 
        m_quadVB->Release();
        m_quadVB = nullptr; 
    }
    if (m_uiCB)
    { 
        m_uiCB->Release();
        m_uiCB = nullptr;
    }
    if (m_textVB) 
    {
        m_textVB->Release();
        m_textVB = nullptr; 
    }

    delete m_splashTex;
    m_splashTex = nullptr;
	delete m_fontAtlas;
    m_fontAtlas = nullptr;
}

bool UIRender::CreateFullscreenQuadVB(ID3D12Device* device)
{
	XMFLOAT4 color = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
    const UIVertex verts[6] =
    {
        // tri 1
        { {-1.f, -1.f}, {+0.f, +1.f}, color},
        { {-1.f, +1.f}, {+0.f, +0.f}, color},
        { {+1.f, +1.f}, {+1.f, +0.f}, color},

        // tri 2
        { {-1.f, -1.f}, {+0.f, +1.f}, color},
        { {+1.f, +1.f}, {+1.f, +0.f}, color},
        { {+1.f, -1.f}, {+1.f, +1.f}, color},
    };

    const UINT vbSize = (UINT)sizeof(verts);

    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto bufDesc = CD3DX12_RESOURCE_DESC::Buffer(vbSize);

    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_quadVB)
    );

    if (FAILED(hr)) return false;

    void* mapped = nullptr;
    CD3DX12_RANGE range(0, 0);
    hr = m_quadVB->Map(0, &range, &mapped);

    if (FAILED(hr)) return false;

    memcpy(mapped, verts, vbSize);
    m_quadVB->Unmap(0, nullptr);

    m_quadVBV.BufferLocation = m_quadVB->GetGPUVirtualAddress();
    m_quadVBV.SizeInBytes = vbSize;
    m_quadVBV.StrideInBytes = sizeof(UIVertex);

    return true;
}

bool UIRender::CreateTintConstantBuffer(ID3D12Device* device)
{
    if (device == nullptr)
        return false;

    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto bufDesc = CD3DX12_RESOURCE_DESC::Buffer(256);

    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_uiCB)
    );

    if (FAILED(hr)) return false;

    void* mapped = nullptr;
    CD3DX12_RANGE range(0, 0);
    hr = m_uiCB->Map(0, &range, &mapped);

    if (FAILED(hr)) return false;

    m_uiCBMapped = reinterpret_cast<uint8_t*>(mapped);

    return true;
}

bool UIRender::CreateTextVB(ID3D12Device* device, uint32_t maxChars)
{
    m_textMaxChars = maxChars;
    m_textVBMaxVerts = maxChars * 6;

    const UINT bufferSize = m_textVBMaxVerts * sizeof(UIVertex);

    D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC bufDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    HRESULT hr = device->CreateCommittedResource(
        &heapProps, D3D12_HEAP_FLAG_NONE, &bufDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&m_textVB)
    );

    if (FAILED(hr)) return false;

    void* mapped = nullptr;
    CD3DX12_RANGE range(0, 0);
    hr = m_textVB->Map(0, &range, &mapped);

    if (FAILED(hr)) return false;

    m_textVBMapped = (uint8_t*)mapped;

    m_textVBV.BufferLocation = m_textVB->GetGPUVirtualAddress();
    m_textVBV.StrideInBytes = sizeof(UIVertex);
    m_textVBV.SizeInBytes = bufferSize;

    return true;
}

UINT UIRender::CreateTextVertices(const char* text,
    float x, float y,
    float scale,
    float screenW, float screenH,
    const XMFLOAT4& color,
    UINT startVertex)
{
    UIVertex* v = reinterpret_cast<UIVertex*>(m_textVBMapped);

    UINT vertCount = startVertex;

    float penX = x;
    float baselineY = y + m_fontData.base * scale;

    for (const char* p = text; *p; ++p)
    {
        unsigned char c = (unsigned char)(*p);
        const Glyph& g = m_fontData.glyphs[c];

        if (g.w == 0 || g.h == 0)
        {
            penX += g.xAdvance * scale;
            continue;
        }

        if (vertCount + 6 > m_textVBMaxVerts)
            break;

        float x0 = penX + g.xOffset * scale;
        float y0 = baselineY + g.yOffset * scale;
        float x1 = x0 + g.w * scale;
        float y1 = y0 + g.h * scale;

        float ndcX0 = PxToNdcX(x0, screenW);
        float ndcX1 = PxToNdcX(x1, screenW);
        float ndcY0 = PxToNdcY(y0, screenH);
        float ndcY1 = PxToNdcY(y1, screenH);

        float u0 = (float)g.x / (float)m_fontData.atlasW;
        float v0 = (float)g.y / (float)m_fontData.atlasH;
        float u1 = (float)(g.x + g.w) / (float)m_fontData.atlasW;
        float v1 = (float)(g.y + g.h) / (float)m_fontData.atlasH;

        v[vertCount + 0] = { { ndcX0, ndcY1 }, { u0, v1 }, color };
        v[vertCount + 1] = { { ndcX0, ndcY0 }, { u0, v0 }, color };
        v[vertCount + 2] = { { ndcX1, ndcY0 }, { u1, v0 }, color };

        v[vertCount + 3] = { { ndcX0, ndcY1 }, { u0, v1 }, color };
        v[vertCount + 4] = { { ndcX1, ndcY0 }, { u1, v0 }, color };
        v[vertCount + 5] = { { ndcX1, ndcY1 }, { u1, v1 }, color };

        vertCount += 6;
        penX += g.xAdvance * scale;
    }

    return vertCount - startVertex;
}

void UIRender::DrawSplash(ID3D12GraphicsCommandList* cmd)
{
    // Bind PSO/RS UI
    cmd->SetPipelineState(m_pipeline->GetUIPSO());
    cmd->SetGraphicsRootSignature(m_pipeline->GetUIRootSignature());

    ID3D12DescriptorHeap* heaps[] = { m_srvHeap->GetHeap() };
    cmd->SetDescriptorHeaps(1, heaps);

    UpdateTint( 1.f, 1.f, 1.f, 0.5f);

    // Root param 0 = CBV(b0)
    cmd->SetGraphicsRootConstantBufferView(0, GetTintCBAddress());
    
    // Root param 1 = SRV table t0 (comme ton root signature)
    cmd->SetGraphicsRootDescriptorTable(1, m_splashSrv.gpu);

    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd->IASetVertexBuffers(0, 1, &m_quadVBV);
    cmd->DrawInstanced(6, 1, 0, 0);
}

void UIRender::DrawScoreCrosshairBatch(ID3D12GraphicsCommandList* cmd, float screenW, float screenH, const UIFrame& ui)
{
    cmd->SetPipelineState(m_pipeline->GetUIPSO());
    cmd->SetGraphicsRootSignature(m_pipeline->GetUIRootSignature());

    ID3D12DescriptorHeap* heaps[] = { m_srvHeap->GetHeap() };
    cmd->SetDescriptorHeaps(1, heaps);

    UpdateTint(1.f, 1.f, 1.f, 1.f);
    cmd->SetGraphicsRootConstantBufferView(0, GetTintCBAddress());

    cmd->SetGraphicsRootDescriptorTable(1, m_fontSrv.gpu);

    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd->IASetVertexBuffers(0, 1, &m_textVBV);

    UINT offset = 0;
    XMFLOAT4 col(1.f, 1.f, 1.f, 1.f);

    // SCORE
    if (ui.showScore)
    {
        char buf[64];
        sprintf_s(buf, "%d", ui.score);

        UINT scoreVerts = CreateTextVertices(buf, 20.f, 20.f, 2.f, screenW, screenH, col, offset);

        cmd->DrawInstanced(scoreVerts, 1, offset, 0);

        offset += scoreVerts;
    }

    // CROSSHAIR
    if (ui.showCrosshair)
    {
        const char* cross = "+";

        float cx = screenW * 0.5f - 8.f;
        float cy = screenH * 0.5f - 32.f;

        UINT crossVerts = CreateTextVertices(cross, cx, cy, 1.f, screenW, screenH, col, offset);

        cmd->DrawInstanced(crossVerts, 1, offset, 0);
    }
}

bool UIRender::ParseIntField(const std::string& token, const char* key, int& out)
{
    size_t p = token.find(key);
    if (p == std::string::npos) 
        return false;

    p += strlen(key);
    out = std::stoi(token.substr(p));

    return true;
}

bool UIRender::LoadBMFontText(const std::string& path, SpriteFont& font)
{
    std::ifstream f(path);
    if (!f) return false;

    std::string line;
    while (std::getline(f, line))
    {
        if (line.rfind("common ", 0) == 0)
        {
            // common lineHeight=.. scaleW=.. scaleH=..
            std::istringstream iss(line);
            std::string tok;
            while (iss >> tok)
            {
                ParseIntField(tok, "lineHeight=", font.lineHeight);
                ParseIntField(tok, "base=", font.base);
                ParseIntField(tok, "scaleW=", font.atlasW);
                ParseIntField(tok, "scaleH=", font.atlasH);
            }
        }
        else if (line.rfind("char ", 0) == 0)
        {
            int id = -1, x = 0, y = 0, w = 0, h = 0, xo = 0, yo = 0, xa = 0;

            std::istringstream iss(line);
            std::string tok;
            while (iss >> tok)
            {
                ParseIntField(tok, "id=", id);
                ParseIntField(tok, "x=", x);
                ParseIntField(tok, "y=", y);
                ParseIntField(tok, "width=", w);
                ParseIntField(tok, "height=", h);
                ParseIntField(tok, "xoffset=", xo);
                ParseIntField(tok, "yoffset=", yo);
                ParseIntField(tok, "xadvance=", xa);
            }

            if (id >= 0 && id < 256)
                font.glyphs[id] = { x,y,w,h,xo,yo,xa };
        }
    }
    return true;
}

#endif // !UI_RENDERER_CPP_INCLUDED