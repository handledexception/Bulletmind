#include <Windows.h>

#include "platform/platform.h"
// #include "platform/win-version.h"
#include "time/time_convert.h"

static LARGE_INTEGER clock_freq;
static bool clock_initialized = false;

static inline u64 get_clock_freq(void)
{
	if (!clock_initialized) {
		QueryPerformanceFrequency(&clock_freq);
		clock_initialized = true;
	}
	return clock_freq.QuadPart;
}

void os_sleep_ms(const u32 duration)
{
	u32 d = duration;
	// if (get_win_ver() >= 0x0602 & duration > 0)
	//     d--;
	Sleep(d);
}

u64 os_get_time_ns(void)
{
	LARGE_INTEGER current_time;
	f64 time_val = 0.0;

	QueryPerformanceCounter(&current_time);
	time_val = (f64)current_time.QuadPart;
	time_val *= 1000000000.0;
	time_val /= (f64)get_clock_freq();

	return (u64)time_val;
}

f64 os_get_time_sec(void)
{
	return nsec_to_sec_f64(os_get_time_ns());
}

f64 os_get_time_msec(void)
{
	return nsec_to_msec_f64(os_get_time_ns());
}