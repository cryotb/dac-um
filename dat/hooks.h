#pragma once

#define GET_ORIG( NAME ) reinterpret_cast<decltype(&NAME)>( orig::##NAME )

namespace hk
{
	bool Start();

	void UserThreadStart(int a1, u64 dwStartAddr, void* lpParam);

	namespace orig
	{
		inline void* UserThreadStart{ };
	}
}
