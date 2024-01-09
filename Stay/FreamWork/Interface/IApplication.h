#ifndef _STAY_IAPPLICATION_H_
#define _STAY_IAPPLICATION_H_

#include "IRuntimeModule.h"

namespace stay
{
	class IApplication : public IRuntimeMoudle
	{
	public:
		virtual int initialize() = 0;
		virtual void finalize() = 0;

		virtual void Tick()  = 0;
		virtual bool isQuit() = 0;
		virtual ~IApplication() = default;

		
	};
}


#endif // !_IAPPLICATION_H_

