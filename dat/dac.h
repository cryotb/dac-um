#pragma once

namespace dac
{
	void Init();
	void Think();
	void CreateConsole();

	auto GetModules() -> std::vector<MODULEENTRY32>;
	auto GetThreads() -> std::vector<DWORD>;
	u64 GetImageForAddr(u64 addr);
	bool GenStackTraceForThread(u32 tid, std::vector<u64>& buffer);
	void** FindThreadInitPtr();

	// logic funcs
	bool CheckThreadLimits();

	// some helpers
	inline auto query_virt_mem(u64 addr)
	{
		MEMORY_BASIC_INFORMATION result;
		VirtualQuery(PTR_OF(addr), &result, sizeof(result));
		return result;
	}

	inline auto memcpy_eh(void* dst, u64 src, size_t len)
	{
		void* result = nullptr;

		__try
		{
			result = memcpy(dst, PTR_OF(src), len);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			result = nullptr;
		}

		return result;
	}

	template < typename T >
	inline auto read_eh(u64 src)
	{
		T result;
		memcpy_eh(&result, src, sizeof(result));
		return (T)result;

	}
}
