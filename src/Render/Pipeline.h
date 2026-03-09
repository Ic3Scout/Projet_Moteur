#ifndef PIPELINE_H_INCLUDED
#define PIPELINE_H_INCLUDED

#include "Helpers/d3dUtil.h"

class Pipeline
{
public:
    Pipeline() = default;
	~Pipeline() {Shutdown();}

	bool InitializePipeline(ID3D12Device* device);

    bool InitializeGraphics(ID3D12Device* device, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat, bool enableDepth);

    void Shutdown();

    ID3D12PipelineState* GetPSO() const { return m_defaultPso; }
    ID3D12RootSignature* GetRootSignature() const { return m_defaultRootSig; }
	ID3D12PipelineState* GetUIPSO() const { return m_uiPso; }
    ID3D12RootSignature* GetUIRootSignature() const { return m_uiRootSig; }

private:
    bool BuildRootSignature(ID3D12Device* device);
	bool BuildUIRootSignature(ID3D12Device* device);
    bool CompileShader(const std::wstring& file, const std::string& entry, const std::string& target, ID3DBlob*& outBlob);

private:
    ID3D12RootSignature* m_defaultRootSig;
    ID3D12PipelineState* m_defaultPso;

    ID3D12RootSignature* m_uiRootSig;
    ID3D12PipelineState* m_uiPso;
};

#endif // !PIPELINE_H_INCLUDED