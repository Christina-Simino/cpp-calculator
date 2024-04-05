# cpp-calculator

Uses C++ Language Standard ISO C++20 Standard.
Windows SDK Version 10.0.
Programmed and tested using a Windows 10 x64 machine.
Unicode is disabled.  This is to prevent certain type errors.

Uses <Windows.h>, <commctrl.h>, <string>, and <vector>.  Uses namespace std.

NOTES ON TRACE LOG:
-TRACE_ENABLED is a global bool that records if trace logging is enabled.  It is set to TRUE initially.
-Turning the trace off prevents trace logging from being printed to trace log box.
-Turning the trace on (if off before) clears the decision counter and trace log box.

NOTES ON DECISION COUNTING:
-Decisions are counted for iterating through for loops.
-For switch statements, any case is recorded as one decision.  The reasoning behind this is because when compiled, switch statements are converted to jump tables which reduces the number of comparisons required in comparison to numerous if-else statements.
-They are not counted for each loop of the message-handling while loop located in WinMain.
-The function used for printing trace logs includes an if statement to check if logging is enabled.  This decision is not documented or recorded in the trace logs for demonstration purposes.  Every time a message is sent to the trace log, one additional decision is made.
-Decisions are not counted for the default case of the main loop, where no input is received.  Documenting this case quickly clutters the trace log and causes the program to lag.  As such, this logging is currently disabled for demonstration purposes.
