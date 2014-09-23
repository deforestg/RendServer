RendServer
==========

This is a basic first person shooter server implementation in C++. I used Boost libaries for the TCP and UDP connections, and pthreads and mutexes for cuncurrency with the main game loop. The server currently primarily acts as a data relay, but handles player joins, leaves, timeouts, deaths, and respawns.
