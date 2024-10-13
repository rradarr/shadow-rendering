#pragma once

using Microsoft::WRL::ComPtr;

class Material
{
public:
    Material() = default;
    Material(const std::string vertexShader, const std::string pixelShader);
    void SetShaders(const std::string vertexShaderFileName, const std::string pixelShaderFileName);
    // Final step of material creation, actually compiles shaders, allocates DX resources etc.
    void CreateMaterial();

    ComPtr<ID3D12PipelineState> GetPSO() const { return pipelineState; };
    ComPtr<ID3D12RootSignature> GetRootSignature() const { return rootSignature; };

    // TODO: Probably remove these from all materials.
    enum RootSignatureParameters {
        TABLE_LIGHT_PARAMS = 0,
        CBV_WVP = 1,
        PARAM_COUNT
    };

protected:
    ComPtr<ID3DBlob> LoadAndCompileShader(const std::string fileName, const std::string compilationTarget);
    void CreateRootSignature();
    void CreatePSO(ComPtr<ID3DBlob> vertexShader, ComPtr<ID3DBlob> pixelShader);

    // Inheriting classes can override the following methods to specialize.
    virtual std::vector<D3D12_ROOT_PARAMETER> CreateRootParameters();
    virtual std::vector<D3D12_STATIC_SAMPLER_DESC> CreateSamplers();
    virtual D3D12_ROOT_SIGNATURE_FLAGS CreateRootSignatureFlags();
    virtual void CustomizePipelineStateObjectDescription(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc);

    ComPtr<ID3D12RootSignature> rootSignature;
    ComPtr<ID3D12PipelineState> pipelineState;

    std::string vertexShaderFileName;
    std::string pixelShaderFileName;

private:
    std::vector<D3D12_DESCRIPTOR_RANGE> descriptorTableVertexRanges;
    std::vector<D3D12_ROOT_PARAMETER> rootParameters;
};

