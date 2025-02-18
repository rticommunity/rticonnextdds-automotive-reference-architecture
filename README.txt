# What

Basic example for a window control use-case.

## System

+ One window controller app, in Micro (WindowApplication), with support for fully opening and fully closing up to two windows (simulates using door buttons)
+ One cockpit GUI app (cockpit.py), in Pro/Python, with support for fully opening and fully closing four windows via window controllers (simulates buttons on the infotainment screen or mobile app)

# How to

1. Compile the Micro app as usual using rtime-make
2. Execute the Micro app as follows:

(connext) user@linux:~/Dev/WindowExample$ ./objs/x64Linux4gcc7.3.0/WindowApplication -ids FL,FR

and optionally a second app:

(connext) user@linux:~/Dev/WindowExample$ ./objs/x64Linux4gcc7.3.0/WindowApplication -ids RL,RR

3. Execute the cockpit app as follows:

(connext) user@linux:~/Dev/WindowExample$ python3 cockpit.py 

4. On the Micro app, you may enter the following commands to move a window. You should see the window position updating on the cockpit.py (or if you're subscribing via Admin Console)
- open FL
- close FL

5. On the cockpit app, you may use the GUI buttons to open or close the windows. You should see the window position updating on the cockpit.py (or if you're subscribing via Admin Console)

6. You may send new commands from either app to interrupt any command currently executing.