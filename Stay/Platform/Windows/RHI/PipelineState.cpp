#include "PipelineState.h"
#include "../WinError.h"
#include "GraphicsCore.h"
#include "RootSignature.h"

namespace stay
{
	GraphicsPSO::GraphicsPSO(const wchar_t* name)
		: PSO(name)
	{
		ZeroMemory(&m_pipelineStateDesc, sizeof(m_pipelineStateDesc));
		m_pipelineStateDesc.SampleMask = 0xFFFFFFFFu;
		m_pipelineStateDesc.SampleDesc.Count = 1;
		m_pipelineStateDesc.NodeMask = 1;
	}

	void GraphicsPSO::SetRenderTargets(UINT numRTVs, const DXGI_FORMAT *rtvFormats, UINT msaaCount, UINT msaaQuality)
	{
		ASSERT(numRTVs > 0);

		for (UINT i = 0; i < numRTVs; i++)
		{
			ASSERT(rtvFormats[i] != DXGI_FORMAT_UNKNOWN);
			m_pipelineStateDesc.RTVFormats[i] = rtvFormats[i];
		}

		for (UINT i = numRTVs; i < m_pipelineStateDesc.NumRenderTargets; i++)
		{
			m_pipelineStateDesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
		}

		m_pipelineStateDesc.NumRenderTargets = numRTVs;
		m_pipelineStateDesc.SampleDesc.Count = msaaCount;
		m_pipelineStateDesc.SampleDesc.Quality = msaaQuality;
	}

	void GraphicsPSO::Fnalize(ID3D12Device* device)
	{
		ASSERT(m_rootSignature != nullptr, "Root Signature Is Null");
		m_pipelineStateDesc.pRootSignature = m_rootSignature->GetRootSignature();
		THROW_IF_FAILED(device->CreateGraphicsPipelineState(&m_pipelineStateDesc, IID_PPV_ARGS(&m_pipelineState)));
		m_pipelineState->SetName(m_name);

	}

	void GraphicsPSO::SetRenderTarget(const DXGI_FORMAT rtvFormat, UINT msaaCount, UINT msaaQuality)
	{
		SetRenderTargets(1, &rtvFormat, msaaCount, msaaQuality);
	}

}
