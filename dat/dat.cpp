#include "pch.h"

void dac::Init()
{
	dac::CreateConsole();
	printf("%s\n", DAT_WELCOME_MSG);

	hk::Start();
}

void dac::Think()
{
	CheckThreadLimits();
}

void dac::CreateConsole()
{
	AllocConsole();

	freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
	freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
}

auto dac::GetModules() -> std::vector<MODULEENTRY32>
{
	std::vector<MODULEENTRY32> result;

	MODULEENTRY32 entry{};
	entry.dwSize = sizeof(MODULEENTRY32);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());

	if (Module32First(hSnapshot, &entry) == FALSE)
	{
		CloseHandle(hSnapshot);
		return result;
	}

	result.push_back(entry);

	while (Module32Next(hSnapshot, &entry))
	{
		result.push_back(entry);
	}

	CloseHandle(hSnapshot);

	return result;
}

auto dac::GetThreads() -> std::vector<DWORD>
{
	std::vector<DWORD> result;

	THREADENTRY32 entry{};
	entry.dwSize = sizeof(THREADENTRY32);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);

	if (Thread32First(hSnapshot, &entry) == FALSE)
	{
		CloseHandle(hSnapshot);
		return result;
	}

	result.push_back(entry.th32ThreadID);

	while (Thread32Next(hSnapshot, &entry))
	{
		if (entry.th32OwnerProcessID == GetCurrentProcessId())
			result.push_back(entry.th32ThreadID);
	}

	CloseHandle(hSnapshot);

	return result;
}

auto dac::GetImageForAddr(u64 addr) -> u64
{
	auto modList = GetModules();

	for (const auto& modEntry : modList)
	{
		if (addr >= BASE_OF(modEntry.modBaseAddr) &&
			addr < (BASE_OF(modEntry.modBaseAddr) + modEntry.modBaseSize))
			return BASE_OF(modEntry.modBaseAddr);
	}

	return 0;
}

// thank EasyAntiCheat for usage example ;-)
bool dac::GenStackTraceForThread(u32 tid, std::vector<u64>& buffer)
{
	CONTEXT ctx;
	memset(&ctx, 0, sizeof(ctx));
	ctx.ContextFlags = CONTEXT_CONTROL;

	if (tid == GetCurrentThreadId())
		return false;

	auto th = OpenThread(0x48u, 0, tid);
	if (!th)
		return false;

	auto tc = GetThreadContext(th, &ctx);
	CloseHandle(th);
	if (!tc)
		return false;

	auto rip = ctx.Rip;
	if (!ctx.Rip)
		return false;

	u32 eCode{ };
	u64 imageBase = 0, estabFrame = 0;
	void* handlerData = 0;
	KNONVOLATILE_CONTEXT_POINTERS nvCp;
	memset(&nvCp, 0, sizeof(nvCp));

	for (u32 n = 0; n < 9; ++n)
	{
		auto unwindFunc = RtlLookupFunctionEntry(
			ctx.Rip,
			&imageBase,
			0i64
		);

		if (unwindFunc)
		{
			__try
			{
				RtlVirtualUnwind(
					0i64,
					imageBase,
					ctx.Rip,
					unwindFunc,
					&ctx,
					&handlerData,
					&estabFrame,
					&nvCp
				);
			}
			__except (eCode = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER)
			{
				//
				// this method is prone to race conditions.
				// we don't suspend the thread due to performance impact,
				// which means context could be invalid the moment we access it.
				// 
				warn("RtlVirtualUnwind - exception: %x", eCode);
				return !buffer.empty();
			}

			if (!ctx.Rip)
				return !buffer.empty();

			buffer.push_back(ctx.Rip);
		}
	}

	return !buffer.empty();
}

void** dac::FindThreadInitPtr()
{
	auto hNTDLL = GetModuleHandle(L"ntdll.dll");
	if (!hNTDLL) return nullptr;

	auto pfnUserThreadStart = GetProcAddress(hNTDLL, "RtlUserThreadStart");
	if (!pfnUserThreadStart) return nullptr;

	return reinterpret_cast<void**>(RESOLVE_INSTR(BASE_OF(pfnUserThreadStart) + 7, 3, 7));
}
