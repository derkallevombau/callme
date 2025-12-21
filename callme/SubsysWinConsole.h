#pragma once

#include <windows.h>
#include <cstdio>
#include <iostream>

// The name refers to the /Subsystem:Windows linker option with which
// this class can be used to get a console.
class SubsysWinConsole
{
private:
	// C++17 feature: `inline static` allows static fields to be declared
	// *and defined* inside the class definition.
	// Methods defined where declared are implicitly inline.
	inline static bool attachedToParent = false;

	// Enables processing of ANSI escape sequences.
	// When starting in WT, dwMode is 7 (ENABLE_VIRTUAL_TERMINAL_PROCESSING == 4,
	// thus VT processing is already enabled), but after AllocConsole(), it is 3,
	// so we need this.
	static void enableVirtualTerminalProcessing()
	{
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut == INVALID_HANDLE_VALUE) return;

		DWORD dwMode = 0;
		if (!GetConsoleMode(hOut, &dwMode)) return;

		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hOut, dwMode);
	}

public:
	// Prevent instantiation (no "static class" in C++)
	SubsysWinConsole() = delete;

	// Call this method before any console I/O.
	// First, this method tries to attach to parent process console and
	// if that fails, it tries to allocate a new one.
	// Then, it connects the standard streams to the console and enables
	// processing of ANSI escape sequences.
	// Returns false if both attaching and allocating fail.
	// Call End() when finished with console I/O.
	static bool Initialise(bool forceSeparateWin = false)
	{
		if (!forceSeparateWin)
		{
			// Try to attach to parent process console first.
			// If we have been started from a console, this should work.
			// See https://learn.microsoft.com/en-us/windows/console/attachconsole.
			attachedToParent = AttachConsole(ATTACH_PARENT_PROCESS);
		}

		// If not started from console or started with "-s", allocate a new one.
		if (!attachedToParent)
		{
			// See https://learn.microsoft.com/en-us/windows/console/allocconsole.
			if (!AllocConsole()) return false;
		}

		// Redirect standard streams to the console (whether attached or allocated).
		// In a console app, this is done automatically.
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

		// Synchronize C++ streams with C streams.
		std::cin.clear();
		std::cout.clear();
		std::cerr.clear();

		enableVirtualTerminalProcessing();

		return true;
	}

	// Call this method when you don't do any more console I/O.
	// If a new console has been allocated, the user will be prompted to dismiss it.
	// After user input, the console will be freed.
	// If attached to parent process console, this does nothing.
	static void End()
	{
		if (attachedToParent) return;

		// Wait for user input before exiting since we are running in our own
		// console window which is closed immediately on exit.
		std::cout << "\nPress Enter to exit.";

		// Reads one character, but only after Enter.
		std::cin.get();

		// Free allocated console (but not an existing one to which we attached).
		FreeConsole();
	}

	static bool AttachedToParent()
	{
		return attachedToParent;
	}
};