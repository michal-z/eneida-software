#include "Pch.h"


#ifdef EASTL_EXCEPTIONS_ENABLED
#if EASTL_EXCEPTIONS_ENABLED == 0
#pragma message("EASTL_EXCEPTIONS_ENABLED 0")
#endif
#endif

#ifdef EASTL_RTTI_ENABLED
#if EASTL_RTTI_ENABLED == 0
#pragma message("EASTL_RTTI_ENABLED 0")
#endif
#endif

#ifdef TBB_USE_EXCEPTIONS
#if TBB_USE_EXCEPTIONS == 0
#pragma message("TBB_USE_EXCEPTIONS 0")
#endif
#endif

void *operator new[](size_t size, const char* /*name*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
{
	return malloc(size);
}

void *operator new[](
	size_t size, size_t alignment, size_t alignmentOffset, const char* /*name*/,
	int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
{
	return _aligned_offset_malloc(size, alignment, alignmentOffset);
}

double GetTime()
{
	static LARGE_INTEGER frequency;
	static LARGE_INTEGER startCounter;
	if (frequency.QuadPart == 0)
	{
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&startCounter);
	}
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return (counter.QuadPart - startCounter.QuadPart) / (double)frequency.QuadPart;
}

void UpdateFrameTime(HWND window, const char *windowText, double *time, float *deltaTime)
{
	static double lastTime = -1.0;
	static double lastFpsTime = 0.0;
	static unsigned frameCount = 0;

	if (lastTime < 0.0)
	{
		lastTime = GetTime();
		lastFpsTime = lastTime;
	}

	*time = GetTime();
	*deltaTime = (float)(*time - lastTime);
	lastTime = *time;

	if ((*time - lastFpsTime) >= 1.0)
	{
		double fps = frameCount / (*time - lastFpsTime);
		double ms = (1.0 / fps) * 1000.0;
		char text[256];
		snprintf(text, sizeof(text), "[%.1f fps  %.3f ms] %s", fps, ms, windowText);
		SetWindowText(window, text);
		lastFpsTime = *time;
		frameCount = 0;
	}
	frameCount++;
}

static LRESULT CALLBACK ProcessWindowMessage(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		if (wparam == VK_ESCAPE)
		{
			PostQuitMessage(0);
			return 0;
		}
		break;
	}
	return DefWindowProc(window, message, wparam, lparam);
}

static HWND MakeWindow(const char *name, unsigned resolutionX, unsigned resolutionY)
{
	WNDCLASS winclass = {};
	winclass.lpfnWndProc = ProcessWindowMessage;
	winclass.hInstance = GetModuleHandle(nullptr);
	winclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	winclass.lpszClassName = name;
	if (!RegisterClass(&winclass))
		assert(0);

	RECT rect = { 0, 0, (LONG)resolutionX, (LONG)resolutionY };
	if (!AdjustWindowRect(&rect, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, 0))
		assert(0);

	HWND window = CreateWindowEx(
		0, name, name, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, NULL, 0);
	assert(window);

	return window;
}

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	SetProcessDPIAware();
	HWND window = MakeWindow("eneida", 1280, 720);

	eastl::vector<int> v;
	v.push_back(1);

	tbb::concurrent_vector<int> cv;
	cv.push_back(2);

	tbb::parallel_for(0, 10, [](int i)
	{
		char text[64];
		snprintf(text, sizeof(text), "%d\n", i);
		OutputDebugString(text);
	});

	for (;;)
	{
		MSG message = {};
		if (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&message);
			if (message.message == WM_QUIT)
				break;
		}
		else
		{
			double frameTime;
			float frameDeltaTime;
			UpdateFrameTime(window, "eneida", &frameTime, &frameDeltaTime);
		}
	}

	return 0;
}
