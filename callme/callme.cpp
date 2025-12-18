#include <iostream>
#include <format>

#ifdef _WIN32
#include <windows.h>

// Added by Copilot to enable processing of ANSI escape sequences.
// dwMode is 7 when starting in WT (thus VT is already enabled),
// but it cannot hurt to keep this.
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
#endif

constexpr auto ansiBlack = "\x1b[30m";
constexpr auto ansiRed = "\x1b[31m";
constexpr auto ansiGreen = "\x1b[32m";
constexpr auto ansiYellow = "\x1b[33m";
constexpr auto ansiBlue = "\x1b[34m";
constexpr auto ansiMagenta = "\x1b[35m";
constexpr auto ansiCyan = "\x1b[36m";
constexpr auto ansiWhite = "\x1b[37m";

constexpr auto ansiLightBlack = "\x1b[90m";
constexpr auto ansiLightRed = "\x1b[91m";
constexpr auto ansiLightGreen = "\x1b[92m";
constexpr auto ansiLightYellow = "\x1b[93m";
constexpr auto ansiLightBlue = "\x1b[94m";
constexpr auto ansiLightMagenta = "\x1b[95m";
constexpr auto ansiLightCyan = "\x1b[96m";
constexpr auto ansiLightWhite = "\x1b[97m";

constexpr auto ansiReset = "\x1b[0m";

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
		// N.B. 3: std::format is a C++20 feature.
		std::cout << std::format("{}{}{}: >{}{}{}<\n", ansiLightGreen, i, ansiReset,
			i % 2 ? ansiLightCyan : ansiLightMagenta, argv[i], ansiReset);
	}
}