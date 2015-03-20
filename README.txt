                                   FMIT
                        Free Music Instrument Tuner
                              Version 0.99.6
                   https://github.com/gillesdegottex/fmit


This software is coded in C/C++ using the Qt library (http://qt-project.org).
It is stored as a GitHub project (https://github.com/gillesdegottex/fmit).
(It was previously stored on gna.org (https://gna.org/projects/fmit)
for versions<=0.99.5).


Copyright (c) 2004 Gilles Degottex <gilles.degottex@gmail.com>


License
    This software is under the GPL (v2) License. See the file LICENSE.txt
    or http://www.gnu.org/licenses/gpl-2.0.html
    Some internal libs of this software (see directories in libs) are under
    the LGPL (v2) License (see the respective directory for more details).

Disclaimer
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
    ALSO, THE COPYRIGHT HOLDERS AND CONTRIBUTORS DO NOT TAKE ANY LEGAL
    RESPONSIBILITY REGARDING THE IMPLEMENTATIONS OF THE PROCESSING TECHNIQUES
    OR ALGORITHMS (E.G. CONSEQUENCES OF BUGS OR ERRONEOUS IMPLEMENTATIONS).
    See the GNU General Public License (LICENSE.txt) for additional details.

Contributors
    Florian Hars for the tunning scales, Arnout Engelen for the JACK issues

    Packagers: Toni Graffy and all the PackMan team ! (SuSE); Ludovic Reslinger
    (Debian); Edward Tomasz Napierala (FreeBSD)
    Donators: Joel Brandt, Martin Lynch, you
    Other helpful people:
    John Anderson for the microtonal module request; Scott Bahling for the
    request of the scales in cents; Jacob Shapiro for his help tp make FMIT
    working under Windows/Cygwin; Peter;


Compilation & Installation

    FMIT currently uses cmake:

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

    ------------------ For non-standard compilation ------------------
    check options:
    $ cmake -L ..
    To change an option, for example configure with PortAudio:
    $ cmake -D SOUNDSYSTEM_USE_PORTAUDIO=ON ..
