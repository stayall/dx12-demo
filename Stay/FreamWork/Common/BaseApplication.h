#ifndef _STAY_BASE_APPLICATION_H_
#define _STAY_BASE_APPLICATION_H_
#include "../Interface/IApplication.h"

namespace stay
{
	class BaseApplication : public IApplication
	{
	public:
		BaseApplication() = default;
		// Í¨¹ý IApplication ¼Ì³Ð
		virtual int initialize() override;
		virtual void Tick() override;
		virtual void finalize() override;
		virtual bool isQuit() noexcept;

	protected:
		bool m_bQuit = false;
	};
}


#endif // !_IAPPLICATION_H_