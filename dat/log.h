#pragma once

#define ARG_SPACE 512

__forceinline auto msg(const char* fmt, ...)
{
	va_list vl;
	auto buflen = strlen(fmt) + ARG_SPACE;
	auto buf = new char[buflen];

	RtlZeroMemory(&vl, sizeof(vl));
	RtlZeroMemory(buf, buflen);

	va_start(vl, fmt);
	vsprintf_s(buf, buflen, fmt, vl);
	va_end(vl);

	printf("MSG  %s\n", buf);

	delete[] buf;
}

__forceinline auto warn(const char* fmt, ...)
{
	va_list vl;
	auto buflen = strlen(fmt) + ARG_SPACE;
	auto buf = new char[buflen];

	RtlZeroMemory(&vl, sizeof(vl));
	RtlZeroMemory(buf, buflen);

	va_start(vl, fmt);
	vsprintf_s(buf, buflen, fmt, vl);
	va_end(vl);

	printf("WRN %s\n", buf);

	delete[] buf;
}
