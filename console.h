/*
 * console.h - Cross-Platform Console Functions
 *
 * This header provides a unified interface for console operations
 * that works on both Windows and macOS/Unix systems.
 *
 * Usage:
 * Manual override (usually not required, as there is an auto detection below):
 *    #define PLATFORM_WINDOWS  // For Windows
 *    OR
 *    #define PLATFORM_UNIX     // For macOS/Unix
 *    Then include this file
 */

 #ifndef _CONSOLE_H_
 #define _CONSOLE_H_

 #include <iostream>

  // ==================== PLATFORM  ====================
  // You can manually define one of these:
  // #define PLATFORM_WINDOWS
  // #define PLATFORM_UNIX
  // usually this is not required, as there is an auto detection below
  // (if you do want to manually define the platform, you can do it on your compilation command, using -D, e.g. -DPLATFORM_WINDOWS or -DPLATFORM_UNIX)

 // Auto-detect platform if not manually defined
 #if !defined(PLATFORM_WINDOWS) && !defined(PLATFORM_UNIX)
 #if defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
 #define PLATFORM_WINDOWS
 #else
 #define PLATFORM_UNIX
 #endif
 #endif

 // ==================== PLATFORM-SPECIFIC INCLUDES ====================
 #ifdef PLATFORM_WINDOWS
 #include <windows.h>
 #include <conio.h>
 #else
 #include <unistd.h>
 #include <termios.h>
 #include <fcntl.h>
 #include <cstdlib>
 #endif

 // ==================== UNIX TERMINAL MANAGEMENT ====================
 #ifdef PLATFORM_UNIX
 static struct termios orig_termios;
 static bool terminal_initialized = false;

 // Restore terminal to original settings
 inline void restore_terminal() {
     if (terminal_initialized) {
         tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
         terminal_initialized = false;
     }
 }

 // Set terminal to raw mode (no echo, no line buffering)
 inline void init_terminal() {
     if (!terminal_initialized) {
         struct termios new_termios;

         tcgetattr(STDIN_FILENO, &orig_termios);
         atexit(restore_terminal);

         new_termios = orig_termios;
         new_termios.c_lflag &= ~(ICANON | ECHO);
         tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

         fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL, 0) | O_NONBLOCK);
         terminal_initialized = true;
     }
 }
 #endif

 // ==================== CROSS-PLATFORM FUNCTIONS ====================

 /**
  * Clear the console screen
  */
 inline void clrscr() {
 #ifdef PLATFORM_WINDOWS
     system("cls");
 #else
     // macOS/Unix
     std::cout << "\033[2J\033[H";
     std::cout.flush();
 #endif
 }

 /**
  * Hide the console cursor
  */
 inline void hideCursor() {
 #ifdef PLATFORM_WINDOWS
     HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
     CONSOLE_CURSOR_INFO cursorInfo;
     GetConsoleCursorInfo(hConsole, &cursorInfo);
     cursorInfo.bVisible = false;
     SetConsoleCursorInfo(hConsole, &cursorInfo);
 #else
     // macOS/Unix
     std::cout << "\033[?25l";
     std::cout.flush();
 #endif
 }

 /**
  * Move cursor to position (x, y)
  * @param x - horizontal position (0-based)
  * @param y - vertical position (0-based)
  */
 inline void gotoxy(int x, int y) {
 #ifdef PLATFORM_WINDOWS
     std::cout.flush();
     HANDLE hConsoleOutput;
     COORD dwCursorPosition;
     dwCursorPosition.X = x;
     dwCursorPosition.Y = y;
     hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
     SetConsoleCursorPosition(hConsoleOutput, dwCursorPosition);
 #else
     // macOS/Unix
     std::cout.flush();
     std::cout << "\033[" << (y + 1) << ";" << (x + 1) << "H";
 #endif
 }

 /**
  * Check if a key has been pressed (non-blocking)
  * @return true if key pressed, false otherwise
  */
 inline bool check_kbhit() {
 #ifdef PLATFORM_WINDOWS
     return _kbhit();
 #else
     // macOS/Unix
     int ch = getchar();
     if (ch != EOF) {
         ungetc(ch, stdin);
         return true;
     }
     return false;
 #endif
 }

 /**
  * Get a character from keyboard without echo, blocking (waits for key)
  * @return the character code
  */
 inline int get_single_char() {
 #ifdef PLATFORM_WINDOWS
     return _getch();
 #else
     // macOS/Unix
     int key = -1;
     while(key == -1) {
        key = getchar();
     }
     return key;
 #endif
 }

 /**
  * Sleep/pause for specified milliseconds
  * @param milliseconds - time to sleep
  */
 inline void sleep_ms(int milliseconds) {
 #ifdef PLATFORM_WINDOWS
     Sleep(milliseconds);
 #else
     // macOS/Unix
     usleep(milliseconds * 1000);
 #endif
 }

 /**
  * Initialize console for special features (keyboard input, etc.)
  * Call this at the start of your program if using kbhit() or getch()
  */
 inline void init_console() {
 #ifdef PLATFORM_UNIX
     init_terminal();
 #endif
     // Windows doesn't need initialization
 }

 /**
  * Cleanup console before exiting
  * Call this before your program ends (optional - auto-cleanup exists)
  */
 inline void cleanup_console() {
 #ifdef PLATFORM_UNIX
     restore_terminal();
 #endif
 }

 // ==================== BONUS: TEXT COLOR FUNCTIONS ====================

 enum class Color { Black, Blue, Green, Aqua, Red, Purple, Yellow, White, Gray, LightBlue, LightGreen, LightAqua, LightRed, LightPurple, LightYellow, BrightWhite };

 /**
  * Set text color (cross-platform)
  */
 inline void set_color(Color color) {
 #ifdef PLATFORM_WINDOWS
     HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
     SetConsoleTextAttribute(hConsole, static_cast<int>(color));
 #else
     // macOS/Unix
     // ANSI color codes for Unix
     static const char* colors[] = {
         "\033[30m", // Black
         "\033[34m", // Blue
         "\033[32m", // Green
         "\033[36m", // Aqua / Cyan
         "\033[31m", // Red
         "\033[35m", // Purple / Magenta
         "\033[33m", // Yellow / Brown
         "\033[37m", // White / Gray
         "\033[90m", // Gray / Dark Gray
         "\033[94m", // Light Blue
         "\033[92m", // Light Green
         "\033[96m", // Light Aqua / Cyan
         "\033[91m", // Light Red
         "\033[95m", // Light Purple / Magenta
         "\033[93m", // Light Yellow
         "\033[97m"  // Bright White
     };
     std::cout << colors[static_cast<int>(color)];
 #endif
 }

 /**
  * Reset text color to default White on black
  */
 inline void reset_color() {
     set_color(Color::White);
 }


 #endif // _CONSOLE_H_

