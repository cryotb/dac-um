#include "pch.h"

DWORD ThrNop(LPVOID lpParam)
{
	/* thread init func redirects to this one if wants blocked. */
	/* can't just return from thread init as it will mess things up. */
	return 0;
}

bool hk::Start()
{
	auto ppfnThreadInit = dac::FindThreadInitPtr();
	if (!ppfnThreadInit) return false;

	orig::UserThreadStart = *ppfnThreadInit;
	*ppfnThreadInit = UserThreadStart;

	return true;
}

void hk::UserThreadStart(int a1, u64 dwStartAddr, void* lpParam)
{
	if (dac::GetImageForAddr(dwStartAddr) == NULL)
	{
		warn("blocking attempt to launch thread on non-image memory!");
		dwStartAddr = BASE_OF(ThrNop);
	}

	return GET_ORIG(UserThreadStart)(a1, dwStartAddr, lpParam);
}
