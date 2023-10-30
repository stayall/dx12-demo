#ifndef __STAY_EMPTY_APPLICATION_H_ 

#define __STAY_EMPTY_APPLICATION_H_
#include "../Common/BaseApplication.h"

namespace stay
{
	BaseApplication g_App;
	IApplication* g_pApp = &g_App;
}
#endif // !__STAY_EMPTY_APPLICATION_H_ 