#include "APCIO.h"

namespace stay::File
{

	DWORD AsycnIOStream::GetFileSize()
	{
		if (m_file == nullptr)
		{
			return 0;
		}

		DWORD len = ::GetFileSize(m_file, nullptr);
		if (len == INVALID_FILE_SIZE)
		{
			
		}
	}
}
