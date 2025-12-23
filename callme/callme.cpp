#include <iostream>
#include <format>
#include <string>
#include <vector>
#include <cstring>

#include "colordefs.h"
#include "SubsysWinConsole.h"

// We link with /Subsystem:Windows to not get attached to parent process console
// or a newly allocated one automatically. If we do that manually, we are able to
// detect if we have been started from a non-console process and in that case, keep
// our console window open until user dismisses it.
// With /Subsystem:Console, we could only choose between prompting user even within
// a terminal and our window being closed immediately when started from non-console
// process.
// Using main() as entry point in a "Windows" (not Console) app needs linking with
// /Entry:mainCRTSetup. The benefit over (w)WinMain() is that we don't need to call
// CommandLineToArgvW() and then narrow the wide strings (there is no "CommandLineToArgvA()").
// Furthermore, that function is inconsistent: No args => argv[0] == module path,
// else => argv[0] == first arg. main()'s argv: [0] == module path (always).
int main(int argc, char* argv[])
{
	using namespace std;

	auto argsToPrint = vector<string>();
	bool forceSeparateWin = false;
	bool separatorFound = false;

	for (int i = 1; i < argc; i++) // Skip module name (argv[0]).
	{
		// When "--" is encountered for the first time, argv[j] for j=1..i-1
		// are options to this program and not to be printed.
		// N.B.: We need to use strcmp() to compare two char*.
		// There are overloads of operator== that do string comparison if at least
		// one operand is a std::string, but a string literal is merely a const char[<length>].
		if (!separatorFound && i != 1 && strcmp(argv[i], "--") == 0)
		{
			separatorFound = true;
			
			argsToPrint.clear();

			for (int j = 1; j < i; j++)
			{
				if (strcmp(argv[j], "-s") == 0)
				{
					forceSeparateWin = true;
				}
				else
				{
					if (!SubsysWinConsole::Initialise()) return -1;

					cerr << format("Error: Unknown command line option: '{}'\n", argv[j]);
				}
			}
		}
		else
		{
			argsToPrint.push_back(argv[i]);
		}
	}

	if (!SubsysWinConsole::Initialise(forceSeparateWin)) return -1;

	for (int i = 0; i < argsToPrint.size(); i++)
	{
		// N.B.: Using "\n" instead of std::endl saves unnecessary flushes.
		// N.B. 2: It's "\n" also on Windows; the runtime choses the correct line break
		// for the platform.
		// N.B. 3: std::format is a C++20 feature.
		cout << format("{}{}{}: >{}{}{}<\n", ansiLightGreen, i + 1, ansiReset,
			i % 2 ? ansiLightCyan : ansiLightMagenta, argsToPrint[i], ansiReset);
	}

	SubsysWinConsole::End();

	return 0;
}