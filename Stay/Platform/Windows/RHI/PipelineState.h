#pragma once

#include "../WinStd.h"


namespace stay
{
	class RootSignature;

	class PSO
	{
	public:
		PSO(const wchar_t* name = L"Unnamed PSO") : m_name(name), m_rootSignature(nullptr), m_pipelineState(nullptr) {};
		~PSO() { m_pipelineState->Release(); }

		void SetRootSignature(RootSignature* rootSignature) { m_rootSignature = rootSignature; }
		RootSignature* GetRootSignature() const { return m_rootSignature; }

		ID3D12PipelineState* GetPipelineState() const { return m_pipelineState; }
	protected:
		const wchar_t* m_name;

		RootSignature* m_rootSignature = nullptr;
		ID3D12PipelineState* m_pipelineState = nullptr;
	};

	class GraphicsPSO : public PSO
	{
	public:
		GraphicsPSO(const wchar_t* name = L"Unnamed Graphics PSO");

		void SetInputLayout(D3D12_INPUT_ELEMENT_DESC* inputLayouts, UINT size) { m_pipelineStateDesc.InputLayout = { inputLayouts, size }; }
		void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive) { m_pipelineStateDesc.PrimitiveTopologyType = primitive; }
		void SetBlendState(const D3D12_BLEND_DESC& blendState) { m_pipelineStateDesc.BlendState = blendState;}
		void SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerState) {
			m_pipelineStateDesc.RasterizerState
				= rasterizerState;
		}
		void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilState) { m_pipelineStateDesc.DepthStencilState = depthStencilState; }
		void SetRenderTarget(const DXGI_FORMAT rtvFormat, UINT msaaCount = 1, UINT msaaQuality = 0);
		void SetRenderTargets(UINT numRTVs, const DXGI_FORMAT* rtvFormats, UINT msaaCount = 1, UINT msaaQuality = 0);
		void SetDepthStencilFormat(DXGI_FORMAT dsvFormats) { m_pipelineStateDesc.DSVFormat = dsvFormats; };

		void SetVertexShader(const D3D12_SHADER_BYTECODE &vs) { m_pipelineStateDesc.VS = vs; }
		void SetPiexlShader(const D3D12_SHADER_BYTECODE &ps) { m_pipelineStateDesc.PS = ps; }

		void Fnalize(ID3D12Device* device);

	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_pipelineStateDesc;
		
	};
}