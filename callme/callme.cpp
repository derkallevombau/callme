#include <iostream>

#ifdef _WIN32
#include <windows.h>

// Added by Copilot to enable processing of ANSI escape sequences.
// The reason why it didn't work first was that I used "\e" which
// according to CP is understood by GCC, but not MSVC.
// dwMode is 7 when starting in WT (thus VT is already enabled),
// but it cannot hurt to keep this.
static void enableVirtualTerminalProcessing()
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) return;

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode)) return;

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);
}
#endif

int main(int argc, char* argv[])
{
#ifdef _WIN32
	enableVirtualTerminalProcessing();
#endif

	for (int i = 0; i < argc; i++)
	{
		// N.B.: Using "\n" instead of std::endl saves unnecessary flushes.
		// N.B. 2: It's "\n" also on Windows; the runtime choses the correct
		// line break for the platform.
		std::cout << i << ": >\x1b[96m" << argv[i] << "\x1b[0m<\n";
	}
}