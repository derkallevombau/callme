A tiny C++ program for Windows that does nothing but list its command line arguments. \
Very simple, but an indispensable helper to spot quoting problems when doing shell
programming.

I wouldn't publish it if it did not have a special feature:
- If started from within a console, it will print to that console and exit.
- If started from a non-console process, it will open an own console window that will stay
  open until you dismiss it.

So you can check what arguments you pass e. g. from a scheduled task and yet get
non-blocking behavior in terminals.

Options:
- `-s`: Force separate window (e. g. to not clutter terminal output)
