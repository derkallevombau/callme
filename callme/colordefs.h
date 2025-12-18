#pragma once

// "\e" is understood by GCC, but not MSVC.

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