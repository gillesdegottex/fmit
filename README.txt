=================== FMIT - Free Music Instrument Tuner =========================

Author: Gilles Degottex <gilles.degottex@gmail.com>

Contributor: Florian Hars for the tunning scales, Arnout Engelen for the JACK issues
Packagers: Toni Graffy and all the PackMan team ! (SuSE); Ludovic Reslinger (Debian); Edward Tomasz Napierala (FreeBSD)
Donators: Joel Brandt, Martin Lynch, you
Other helpful people:
John Anderson for the microtonal module request; Scott Bahling for the request of the scales in cents; Jacob Shapiro to help me make FMIT working under Windows/Cygwin; Peter; ...

======================= COMPILATION & INSTALLATION =============================

To compile FMIT:
From fmit package directory, create a directory build:
$ mkdir build
$ cd build
Configure with standard options (see below for options):
$ cmake ..

Then as usual:
Compile
$ make
and install as root
# make install

---------------------- For non-standard compilation ----------------------------
check options:
$ cmake -L ..
To change an option, for example configure with PortAudio:
$ cmake -D SOUNDSYSTEM_USE_PORTAUDIO=ON ..
