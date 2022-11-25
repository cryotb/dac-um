#include "pch.h"

auto dat::CheckThreadLimits() -> bool
{
	auto threadList = GetThreads();

	for (auto thrId : threadList)
	{
		if (thrId == GetCurrentThreadId())
			continue;

		std::vector<u64> stackTrace;

		if (GenStackTraceForThread(thrId, stackTrace))
		{
			for (u64 rip : stackTrace)
			{
				auto regInfo = query_virt_mem(rip);

				if (regInfo.Protect == PAGE_EXECUTE_READ ||
					regInfo.Protect == PAGE_EXECUTE_READWRITE)
				{
					if (GetImageForAddr(rip) == NULL)
					{
						warn("[ThreadLimit] catched unbacked instr! RIP=%p BASE=%p", 
							rip, regInfo.AllocationBase);

						if (read_eh<u16>(BASE_OF(regInfo.AllocationBase)) == 0x5A4D)
							warn(" -> alloc base of this instr contains PE header!");
					}
				}
			}
		}
	}

	return true;
}
