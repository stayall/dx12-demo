#ifndef _STAY_CLASS_ATRRIBUTE_H_
#define _STAY_CLASS_ATRRIBUTE_H_

#define Align(x) __declspec(align(x))

class NoCopyable
{
protected:
	NoCopyable() = default;
	NoCopyable(const NoCopyable& ) = delete;
	NoCopyable(const NoCopyable&& ) = delete;
	NoCopyable& operator=(const NoCopyable&) = delete;

};

#endif // !_STAY_CLASS_ATRRIBUTE_H_

