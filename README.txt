
                                   FMIT
                        Free Music Instrument Tuner
                              Version 1.0.12
                   http://gillesdegottex.github.io/fmit/

FMIT is a graphical utility for tuning your musical instruments, with error
and volume history and advanced features like microtonal tuning, statistics,
and various views like waveform shape, harmonics ratios and real-time Discrete
Fourier Transform (DFT). All views and advanced features are optional so that
the interface can also be very simple.

This software is coded in C/C++ using the Qt library (http://qt-project.org).
It is stored as a GitHub project (https://github.com/gillesdegottex/fmit).
(It was previously stored on gna.org (https://gna.org/projects/fmit)
for versions<=0.99.5).

Goals
    * Estimate the fundamental frequency (f0) of an audio signal, in real-time.
      (the f0, not the perceived pitch)
    * Keep any view optional so that the interface can be as simple as possible.
      (make it easy to see on a distant screen)
    * Support as many sound systems as possible.


Copyright (c) 2004 Gilles Degottex <gilles.degottex@gmail.com>


License
    Most of the code of this software is under the GPL (v2) License.
    See the file COPYING_GPL.txt or http://www.gnu.org/licenses/gpl-2.0.html

    Some abstract or helper classes are under LGPL (v2.1).
    See the file COPYING_LGPL.txt or http://www.gnu.org/licenses/lgpl-2.1.html

    The internal libs of this software (see directories in libs) are also under
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

    Packagers: Toni Graffy and all the PackMan team, Ludovic Reslinger,
        Edward Tomasz Napierala, John Wright.
    Translators: Omega9 (Russian), skarmoutsosv (Greek), Pindaro (Portuguese),
        Florian Maier (German).
    Other helpful people: John Anderson for the microtonal module request,
        Scott Bahling for the request of the scales in cents,
        Jacob Shapiro for his help to make FMIT working under Windows/Cygwin, Peter.
