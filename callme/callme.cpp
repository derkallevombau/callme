#include <iostream>
#include <format>
#include <windows.h>
#include <sal.h>
#include <string>
#include <cstdio>

#include "colordefs.h"

// Enables processing of ANSI escape sequences.
// When starting in WT, dwMode is 7 (ENABLE_VIRTUAL_TERMINAL_PROCESSING == 4,
// thus VT processing is already enabled), but after AllocConsole(), it is 3,
// so we need this.
// N.B.: A static *function* can only be called from within the same
// translation unit, which simplifies linkage.
static void enableVirtualTerminalProcessing()
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) return;

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode)) return;

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);
}

// Entry point when linking with /SUBSYSTEM:WINDOWS (Unicode version), as opposed to
// `main` when linking with /SUBSYSTEM:CONSOLE.
// See https://learn.microsoft.com/en-us/windows/win32/learnwin32/winmain--the-application-entry-point
// and https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-winmain.
// Annotations: See https://stackoverflow.com/questions/13078953/code-analysis-says-inconsistent-annotation-for-wwinmain-this-instance-has-no.
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
	// See https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-commandlinetoargvw.
	int argc;
	LPWSTR* argv = CommandLineToArgvW(pCmdLine, &argc);

	// Try to attach to parent process console first (if started from terminal) 
	// See https://learn.microsoft.com/en-us/windows/console/attachconsole
	bool attachedToParent = AttachConsole(ATTACH_PARENT_PROCESS);

	// If not started from console, allocate a new one
	if (!attachedToParent)
	{
		// See https://learn.microsoft.com/en-us/windows/console/allocconsole.
		if (!AllocConsole())
		{
			LocalFree(argv);
			return -1;
		}
	}

	// Redirect standard streams to the console (whether attached or allocated).
	// This is necessary because this is not a console application, which in turn
	// is necessary to be able to detect if we have been started from within a console
	// or not.
	// freopen_s closes the file associated with the file stream pointed to by the
	// fourth argument (FILE*), then opens the file specified by the second arg and
	// associates that file with the file stream pointed to by the fourth arg.
	// The pointer whose address has been passed in the first arg is set to point to
	// that file stream too; we use dummies here.
	// The crucial point is that an existing file stream (here the standard streams)
	// is associated with a newly opened file.
	// See https://en.cppreference.com/w/c/io/freopen.
	FILE* pFileIn = nullptr;
	FILE* pFileOut = nullptr;
	FILE* pFileErr = nullptr;
	freopen_s(&pFileIn, "CONIN$", "r", stdin);
	freopen_s(&pFileOut, "CONOUT$", "w", stdout);
	freopen_s(&pFileErr, "CONOUT$", "w", stderr);

	// Synchronize C++ streams with C streams
	std::cin.clear();
	std::cout.clear();
	std::cerr.clear();

	enableVirtualTerminalProcessing();

	for (int i = 0; i < argc; i++)
	{
		// Convert wide string to narrow string for std::format
		std::wstring warg(argv[i]);
		std::string arg(warg.begin(), warg.end());

		// N.B.: Using "\n" instead of std::endl saves unnecessary flushes.
		// N.B. 2: It's "\n" also on Windows; the runtime choses the correct
		// line break for the platform.
		// N.B. 3: std::format is a C++20 feature.
		std::cout << std::format("{}{}{}: >{}{}{}<\n", ansiLightGreen, i, ansiReset,
			i % 2 ? ansiLightCyan : ansiLightMagenta, arg, ansiReset);
	}

	if (!attachedToParent)
	{
		// Wait for user input before exiting since we are running in our own
		// console window which is closed immediately on exit.
		std::cout << "\nPress any key to exit.";

		char* pInput = new char[1];
		std::cin.read(pInput, 1);
		delete[] pInput;

		// Free allocated console (but not an existing one to which we attached).
		FreeConsole();
	}

	// Free memory allocated by CommandLineToArgvW.
	// See https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-commandlinetoargvw
	LocalFree(argv);

	return 0;
}