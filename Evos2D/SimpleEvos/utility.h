
#ifndef __UTILITY_H__
#define __UTILITY_H__


static void DebugPrint(const char *fmt, ...)
{
	char buf[128];
	va_list marker;
	va_start(marker, fmt);
	vsprintf_s(buf, fmt, marker);
	va_end(marker);
	OutputDebugString(buf);
}


#endif
