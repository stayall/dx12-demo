#include "Exception.h"
#include <sstream>

namespace stay
{
	Exception::Exception(const char* filePath, int line) noexcept
	{
		this->filePath = filePath;
		this->line = line;
	}
	const char* Exception::what() const noexcept
	{
		std::stringstream ss;
		ss << getOriginString();
		whatBuffer = ss.str();
		return whatBuffer.c_str();
	}

	const char* Exception::getType() const noexcept
	{
		return "Base Exception";
	}

	const std::string Exception::getOriginString() const noexcept
	{
		std::stringstream ss;
		ss << "FilePath: " << filePath << std::endl
			<< "FileLine: " << line << std::endl;
		return ss.str();
	}

}