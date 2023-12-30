#pragma once

#include "../Interface/IRuntimeModule.h"
#include "Platform/Windows/RHI/GraphicsCore.h"



namespace stay::GameCore
{
	//FIXED:: App semantice have problem 
	//App* g_App = GameCore::g_App;

	class GraphicsModule :public IRuntimeMoudle
	{
	public:
		// ͨ�� IRuntionMoudle �̳�
		virtual int initialize() override;
		virtual void finalize() override;
		virtual void Tick() override;
	};
}