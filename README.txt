# What

Basic example for a window control use-case.

## System

+ One window controller app, in Micro (Window_publisher), with support for fully opening and fully closing one window (simulates using door buttons)
+ One cockpit GUI app (cockpit.py), in Pro/Python, with support for fully opening and fully closing a window via the window controller (simulates buttons on the infotainment screen or mobile app)

# How to

1. Compile the Micro app as usual using rtime-make
2. Execute the Micro app as follows:

(venv7305) mzain@RTISP-10122:~/Dev/WindowExample$ ./objs/x64Linux4gcc7.3.0/Window_publisher -id FR

3. Execute the cockpit app as follows:

(venv7305) mzain@RTISP-10122:~/Dev/WindowExample$ python3 cockpit.py 

4. On the Micro app, you may enter the following commands to move the window. You should see the window position updating on the cockpit.py console output (or if you're subscribing via Admin Console)
- open
- close

5. On the cockpit app, you may use the GUI buttons to open or close the window. You should see the window position updating on the cockpit.py console output (or if you're subscribing via Admin Console)

6. You may send new commands from either app to interrupt any command currently executing.

# To Do

- Support more than one window.
- Display window position on the GUI
- Refactor
- ...