#ifndef _STAY_IRUNTION_MODULE_H_
#define _STAY_IRUNTION_MODULE_H_

#include "IUnkown.h"

namespace stay
{
	class IRuntionMoudle : public IUnkown
	{
	public:
		virtual int initialize() = 0;
		virtual void finalize() = 0;

		virtual void Tick() = 0;

		virtual ~IRuntionMoudle() = default;
	};
}


#endif // !_STAY_IRUNTION_MODULE_H_

