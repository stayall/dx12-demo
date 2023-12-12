#ifndef _STAY_EXCEPTION_H_
#define _STAY_EXCEPTION_H_

#include <exception>
#include <string>
#include  <stdexcept>

namespace stay
{

	class Exception : public std::exception
	{
	public:
		Exception(const char* filePath, int line) noexcept;
		virtual const char* what() const noexcept;
		virtual const char* getType() const noexcept;
		virtual const std::string getOriginString() const noexcept;
		virtual ~Exception() = default;
	private:
		int line;
		std::string filePath;
	protected:
		mutable std::string whatBuffer;
	};
}
#endif // !_STAY_EXCEPTION_H_

