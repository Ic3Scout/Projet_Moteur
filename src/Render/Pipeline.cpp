#ifndef PIPELINE_CPP_INCLUDED
#define PIPELINE_CPP_INCLUDED

#include "Pipeline.h"

static UINT ShaderCompileFlags()
{
#if defined(DEBUG) || defined(_DEBUG)
    return D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    return D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
}

bool Pipeline::InitializePipeline(ID3D12Device* device)
{
    // Formats identiques à SwapChainTargets (R8G8B8A8 + D24S8)
    return InitializeGraphics(
        device,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        true
    );
}

bool Pipeline::InitializeGraphics(ID3D12Device* device, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat, bool enableDepth)
{
    if (device == nullptr) return false;

    Shutdown();

    if (BuildRootSignature(device) == false) return false;
	if (BuildUIRootSignature(device) == false) return false;

	// --------------------
	// DEFAULT SHADERS
	// --------------------
    ID3DBlob* defaultVS = nullptr;
    ID3DBlob* defaultPS = nullptr;

    if (CompileShader(L"../../res/Shaders/Default_VS.hlsl", "VSMain", "vs_5_0", defaultVS) == false) return false;
    if (CompileShader(L"../../res/Shaders/Default_PS.hlsl", "PSMain", "ps_5_0", defaultPS) == false) return false;

    static D3D12_INPUT_ELEMENT_DESC inputLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = m_defaultRootSig;

    psoDesc.VS = { defaultVS->GetBufferPointer(), defaultVS->GetBufferSize() };
    psoDesc.PS = { defaultPS->GetBufferPointer(), defaultPS->GetBufferSize() };

    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

    if (enableDepth == false)
    {
        psoDesc.DepthStencilState.DepthEnable = false;
        psoDesc.DepthStencilState.StencilEnable = false;
    }

    psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = rtvFormat;
    psoDesc.DSVFormat = enableDepth ? dsvFormat : DXGI_FORMAT_UNKNOWN;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;

    HRESULT hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_defaultPso));
    if (FAILED(hr))
        return false;

    // --------------------
    // UI SHADERS 
    // --------------------
    ID3DBlob* uiVS = nullptr;
    ID3DBlob* uiPS = nullptr;

    // Exemple: fichiers/entry différents pour l'UI
    if (CompileShader(L"../../res/Shaders/UI_VS.hlsl", "VSMain", "vs_5_0", uiVS) == false) return false;
    if (CompileShader(L"../../res/Shaders/UI_PS.hlsl", "PSMain", "ps_5_0", uiPS) == false) return false;

    static D3D12_INPUT_ELEMENT_DESC uiLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 8,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    // PSO UI
    D3D12_GRAPHICS_PIPELINE_STATE_DESC uiDesc = {};
    uiDesc.pRootSignature = m_uiRootSig;

    uiDesc.VS = { uiVS->GetBufferPointer(), uiVS->GetBufferSize() };
    uiDesc.PS = { uiPS->GetBufferPointer(), uiPS->GetBufferSize() };

    uiDesc.SampleMask = UINT_MAX;
    uiDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    uiDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    uiDesc.DepthStencilState.DepthEnable = FALSE;
    uiDesc.DepthStencilState.StencilEnable = FALSE;

    uiDesc.InputLayout = { uiLayout, _countof(uiLayout) };
    uiDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    uiDesc.NumRenderTargets = 1;
    uiDesc.RTVFormats[0] = rtvFormat;
    uiDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
    uiDesc.SampleDesc.Count = 1;
    uiDesc.SampleDesc.Quality = 0;

    // alpha blend on
    uiDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    uiDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
    uiDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    uiDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    uiDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    uiDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    uiDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
    uiDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

    hr = device->CreateGraphicsPipelineState(&uiDesc, IID_PPV_ARGS(&m_uiPso));
    if (FAILED(hr))
        return false;

    return true;
}

void Pipeline::Shutdown()
{
    SafeRelease(m_defaultPso);
    SafeRelease(m_defaultRootSig);
    SafeRelease(m_uiPso);
	SafeRelease(m_uiRootSig);
}

bool Pipeline::BuildRootSignature(ID3D12Device* device)
{
    // 4 params :
    // 0 = CBV b0 (Object)
    // 1 = CBV b1 (Frame)
    // 2 = Table SRV t0 (Texture)
    // 3 = Table SRV t1 (Lights buffer)

    D3D12_ROOT_PARAMETER params[4] = {};
    // Param 0 : CBV b0 (VS)

    params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    params[0].Descriptor.ShaderRegister = 0; // b0
    params[0].Descriptor.RegisterSpace = 0;
    params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    // Param 1 : CBV b1 (frame, caméra + nb lights)
    params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    params[1].Descriptor.ShaderRegister = 1; // b1
    params[1].Descriptor.RegisterSpace = 0;
    params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    // Param 2 : Descriptor table SRV t0 (PS)
    D3D12_DESCRIPTOR_RANGE range = {};
    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    range.NumDescriptors = 1;
    range.BaseShaderRegister = 0; // t0
    range.RegisterSpace = 0;
    range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    params[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    params[2].DescriptorTable.NumDescriptorRanges = 1;
    params[2].DescriptorTable.pDescriptorRanges = &range;
    params[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // Param 3 : SRV table t1 (lights)
    //D3D12_DESCRIPTOR_RANGE lightRange = {};
    //lightRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    //lightRange.NumDescriptors = 1;
    //lightRange.BaseShaderRegister = 1; // t1
    //lightRange.RegisterSpace = 0;
    //lightRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    //params[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    //params[3].DescriptorTable.NumDescriptorRanges = 1;
    //params[3].DescriptorTable.pDescriptorRanges = &lightRange;
    //params[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // Static sampler s0
    D3D12_STATIC_SAMPLER_DESC samp = {};
    samp.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    samp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samp.ShaderRegister = 0; // s0
    samp.RegisterSpace = 0;
    samp.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    samp.MinLOD = 0.0f;
    samp.MaxLOD = D3D12_FLOAT32_MAX;
    samp.MaxAnisotropy = 1;
    samp.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    D3D12_ROOT_SIGNATURE_DESC desc = {};
    desc.NumParameters = _countof(params);
    desc.pParameters = params;
    desc.NumStaticSamplers = 1;
    desc.pStaticSamplers = &samp;
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob* serialized = nullptr;
    ID3DBlob* error = nullptr;

    HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &serialized, &error);
    if (FAILED(hr))
    {
        if (error) 
            OutputDebugStringA((char*)error->GetBufferPointer());
        SafeRelease(error);
        SafeRelease(serialized);
        return false;
    }

    hr = device->CreateRootSignature(0, serialized->GetBufferPointer(), serialized->GetBufferSize(), IID_PPV_ARGS(&m_defaultRootSig));
    if (FAILED(hr))
    {
        SafeRelease(error);
        SafeRelease(serialized);
        return false;
	}

    SafeRelease(error);
    SafeRelease(serialized);
    return SUCCEEDED(hr);
}

bool Pipeline::BuildUIRootSignature(ID3D12Device* device)
{
    // Param 0 : CBV b0 (VS)
    D3D12_ROOT_PARAMETER params[2] = {};

    params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    params[0].Descriptor.ShaderRegister = 0; // b0
    params[0].Descriptor.RegisterSpace = 0;
    params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    // Param 1 : Descriptor table SRV t0 (PS)
    D3D12_DESCRIPTOR_RANGE range = {};
    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    range.NumDescriptors = 1;
    range.BaseShaderRegister = 0; // t0
    range.RegisterSpace = 0;
    range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    params[1].DescriptorTable.NumDescriptorRanges = 1;
    params[1].DescriptorTable.pDescriptorRanges = &range;
    params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // Static sampler s0
    D3D12_STATIC_SAMPLER_DESC samp = {};
    samp.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    samp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samp.ShaderRegister = 0; // s0
    samp.RegisterSpace = 0;
    samp.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    samp.MinLOD = 0.0f;
    samp.MaxLOD = D3D12_FLOAT32_MAX;
    samp.MaxAnisotropy = 1;
    samp.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    D3D12_ROOT_SIGNATURE_DESC desc = {};
    desc.NumParameters = _countof(params);
    desc.pParameters = params;
    desc.NumStaticSamplers = 1;
    desc.pStaticSamplers = &samp;
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


    ID3DBlob* serialized = nullptr;
    ID3DBlob* error = nullptr;

    HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &serialized, &error);

    if (FAILED(hr))
    {
        if (error)
            OutputDebugStringA((char*)error->GetBufferPointer());
        SafeRelease(error);
        SafeRelease(serialized);
        return false;
    }

    hr = device->CreateRootSignature(0, serialized->GetBufferPointer(), serialized->GetBufferSize(), IID_PPV_ARGS(&m_uiRootSig));

    if (FAILED(hr))
    {
        SafeRelease(error);
        SafeRelease(serialized);
        return false;
    }

    SafeRelease(error);
    SafeRelease(serialized);

	return SUCCEEDED(hr);
}

bool Pipeline::CompileShader(const std::wstring& file, const std::string& entry, const std::string& target, ID3DBlob*& outBlob)
{
    SafeRelease(outBlob);

    ID3DBlob* errors = nullptr;

    HRESULT hr = D3DCompileFromFile(
        file.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entry.c_str(),
        target.c_str(),
        ShaderCompileFlags(),
        0,
        &outBlob,
        &errors
    );

    if (FAILED(hr))
    {
        if (errors)
        {
            OutputDebugStringA((const char*)errors->GetBufferPointer());
        }
        SafeRelease(errors);
        return false;
    }

    SafeRelease(errors);
    return SUCCEEDED(hr);
}

#endif // !PIPELINE_CPP_INCLUDED