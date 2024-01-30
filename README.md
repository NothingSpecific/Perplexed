# Perplexed
A lightweight IDE for streamlining development processes.


Perplexed currently supports basic text editing functionality. Eventually it's planned to be a full-featured IDE, but for now, text editing is a good start.

To build:

1. Install GLFW into `/usr/local/include/GLFW`
2. Install SDL2 into `/usr/local/include/SDL2`

Once your system has GLFW and SDL2 installed, you should have the necessary dependencies. From here, you can build with

`make`

and install with

`sudo make install`

Perplexed will then be installed as `/usr/local/bin/perplexed` where you can run it as the shell command `perplexed`.
