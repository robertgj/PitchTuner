/**
 \file PitchTuner.h

 \mainpage PitchTuner : Implementation of a YIN pitch estimator and GUI for Linux

 \section intro_sec Introduction
 This project implements a GUI for the \e YIN [1] pitch estimator.

 More recent implementations of pitch estimators are available on the web:
 
\e pYIN [2] adds a probabilistic hidden-Markov model to make multiple YIN pitch
estimates then performs Viterbi decoding of the pitch trajectory to find a final
precise estimate. Software is
<a href="http://code.soundsoftware.ac.uk/projects/pyin">available</a>.

\e CREPE [3] is "based on a deep convolutional neural network that operates
directly on the time-domain waveform". Software is
<a href="https://github.com/marl/crepe">available</a>.

\e Melodia [4] is a "system for the automatic extraction of the main melody from 
polyphonic music recordings". 

<a href="https://essentia.upf.edu">essentia</a> is "... a C++ library for
audio and music analysis, description and synthesis".
It includes implementations of pYIN, CREPE and Melodia.

\subsection References
[1] "YIN, A fundamental frequency estimator for speech and music",
A. de Cheveigne and H. Kawahara, Journal of the Acoustical Society of
America, Vol. 11, No. 4, April 2002, pp. 1917-1930.

[2] "PYIN: A FUNDAMENTAL FREQUENCY ESTIMATOR USING PROBABILISTIC THRESHOLD 
DISTRIBUTIONS", M. Mauch and S. Dixon, ICASSP 2014, pp. 650-663. 

[3] "CREPE: A CONVOLUTIONAL REPRESENTATION FOR PITCH ESTIMATION"
J. W. Kim, J. Salamon, P Li and J. P. Bello, ICASSP 2018, pp. 161-165. 

[4] "Melody Extraction From Polyphonic Music Signals Using Pitch Contour 
Characteristics", J. Salamon and E. Gómez, IEEE Transactions on Audio, Speech, 
and Language Processing, Vol. 20, No. 6, August 2012, pp. 1759-1770.

\section algorithm_sec The YIN algorithm

 \subsection autocorrelation_sec Step 1 : Autocorrelation
 Use the autocorrelation method. For integration window, \c W :
 \f[ r_{t}(\tau)=\sum_{j=t+1}^{t+W}x_{j}x_{j+\tau} \f]

 \subsection difference_sec Step 2 : Difference function
 Model as periodic:
 \f[ x_{t}-x_{t+T}=0 \f]
 so:
 \f[ \sum_{j=t+1}^{t+W}(x_{j}-x_{j+T})^{2}=0 \f]
 For unknown period, \f$T\f$, define:
 \f[ d_{t}(T)=\sum_{j=t+1}^{t+W}(x_{j}-x_{j+T})^{2} \f]
 and search for values of T at which \f$d_{t}=0\f$.
 Expand \f$d_{t}(T)\f$ in terms of \f$r_{t}\f$:
 \f[ d_{t}(T)=r_{t}(0)+r_{t+T}(0)-2r_{t}(T)\f]
 For \f$d_{t}(T)\f$ the sample time lies in \f$t-\frac{T}{2}-\frac{W}{2}+1\f$
 to \f$t+\frac{T}{2}+\frac{W}{2}\f$. For convenience \f$T<2W\f$ and
 the sample time lies in \f$t-\frac{3W}{2}+1\f$ to \f$t-\frac{3W}{2}\f$.

 \subsection cum_mean_sec Step 3 : Cumulative mean normalised difference
 \f[ d'_{t}(T)=\left\{ \begin{array}{cc}
         1 & \text{if } T=0 \\
         d_{t}(T)/\left[\frac{1}{T}\sum_{j=1}^{T}d_{t}(j)\right] &
         \text{ otherwise}
      \end{array}\right.\f]
  Benefits:
   - reduces ``too high'' errors
   - don't need upper frequency limit on search to avoid zero-lag dip
   - normalises for next step

 \subsection abs_thresh_sec Step 4 : Absolute threshold
 Set threshold. Choose the smallest \f$\tau\f$ giving minimum of \f$d_{t}'(T)\f$
 deeper than that threshold.

 \subsection para_interp_sec Step 5 : Parabolic interpolation
 Increase accuracy by parabolic interpolation near each minimum of
 \f$d_{t}(T)\f$ and \f$d_{t}'(T)\f$. 
 \subsection local_est_sec Step 6 : Best local estimate
 From [1, Section II.F]:

 > For each time index \f$t\f$, search for a minimum of
 > \f$d_{\theta}'(T_{\theta})\f$ for \f$\theta\f$ within a small interval
 > \f$\left[t-T_{max}/2,\,t+T_{max}/2\right]\f$, where \f$T_{\theta}\f$ is the
 > estimate at time \f$\theta\f$ and \f$T_{max}\f$ is the largest expected
 > period. Based on this initial estimate, the estimation algorithm is applied
 > again with a restricted search range to obtain the final estimate.

 I do not understand why this repeated search should produce an improved pitch
 estimate.

 \subsection enhancement_sec Enhancements
 \subsubsection varying_dc_sec Remove slowly varying DC
 See Section VI.C of the reference. A slowly varing DC offset is
 produced when the speakers mouth is too close to the microphone. The
 effect of a DC ramp can be removed by setting the derivative of
 \f$d_{t}(T)\f$ with respect to the DC offset to zero. To remove a ramp
 with slope \f$\Delta\f$ added to the input, \f$x_{j}\f$:
 \f[
 \begin{eqnarray*}
      d_{t}(T)  &= & \sum_{j=t+1}^{t+W}(x_{j}-(x_{j+T}-T\Delta))^{2} \\
                &= & \sum_{j=t+1}^{t+W}(x_{j}-x_{j+T})^{2}+WT^{2}\Delta^{2}
         +2T\triangle\sum_{j=t+1}^{t+W}(x_{j}-x_{j+T}) \\
  \frac{d}{d\triangle} d_{t}(T) &= & 
   2WT^{2}\Delta+2T\sum_{j=t+1}^{t+W}(x_{j}-x_{j+T})
 \end{eqnarray*}
 \f]
 So insert:
 \f[ \Delta  = -\frac{1}{TW}\sum_{j=t+1}^{t+W}(x_{j}-x_{j+T})\f]
 and:
 \f[  \begin{eqnarray*}
    d_{t}(T) & = & \sum_{j=t+1}^{t+W}(x_{j}-x_{j+T})^{2}
               -\frac{1}{W}\left[\sum_{j=t+1}^{t+W}(x_{j}-x_{j+T})\right]^{2} \\
             & = & r_{t}(0)+r_{t+T}(0)-2r_{t}(T)
                 -\frac{1}{W}\left[\sum_{j=t+1}^{t+W}(x_{j}-x_{j+T})\right]^{2}
 \end{eqnarray*}
 \f]

 \section implement_sec Implementation
 I assume a Linux operating system and the ALSA sound system.
 The programming language is C++ and I use the wxWidgets GUI toolkit.
 The YIN pitch estimation algorithm runs in a thread. The wxWidgets GUI has the
 following events:
 - \c ID_Pitch : indicates that a pitch estimate is available
 - \c ID_ThreadError : an error occurred in the pitch estimation thread
 - \c ID_EOS : end-of-stream, an error if using an audio device
 - \c ID_DeviceOptions : a request to display the device options dialog box
 - \c ID_About : a request to display the "about" message box
 - \c ID_Quit : a request to exit
 - \c ID_GuiTest : a request to update the fake pitch estimate in the GUI after
                   the GUI test timer times out
 
 \subsection alsa_sec ALSA and wav file interface
 The ALSA specific source files are in the \code src/SimpleAudio/alsa \endcode
 directory. The Linux ALSA sound system is implemented as a layer on top of
 PipeWire or PulseAudio or ... and the API refers to device names that are
 internally mapped to UNIX device files. In this project, the audio input is
 referred to as `"default"` and it is up to the user to configure the audio
 mixer with an application like \e pavucontrol or \e alsamixer.
 
 The available ALSA input and output device names are listed by
 \code arecord -L \endcode
 and
 \code aplay -L \endcode

 The \c SimpleAudio library provides an interface to input signals from the
 default ALSA capture device or from \c wav or text files. Output to \c wav
 files is supported. The ALSA specific source files are in the
 \c src/\c SimpleAudio/\c alsa directory.

 \subsubsection loopback_sec ALSA snd_aloop device
 A loop-back sound device is useful for testing. The pitch estimator was
 originally implemented on FreeBSD/OSS and Windows Mobile. For OSS, sound
 devices appear under \c /dev, as is usual for UNIX, and I implemented a
 \c snd_echo loop-back device. The ALSA loop-back device driver is loaded at
 run-time with
 \code modprobe snd_aloop \endcode
 and at boot-time with (note the spelling):
 \code
 $ cat /etc/modules-load.d/snd-aloop.conf
 snd_aloop
 \endcode
 Here is an example of copying 4 seconds of a file through the loop-back device:
 \code
 (arecord -D hw:Loopback,1 -f S16_LE -r 48000 -d 4 out.wav &); \
 (aplay -D hw:Loopback,0 in.wav);
 \endcode
 Unfortunately, I find that:
 - loading the \c snd_aloop module disables access to my hardware sound card
 - using the \c snd_aloop module triggers selinux warnings
 - once loaded, it is difficult to unload the \c snd_aloop module

 \subsection sasample_sec saSample
 This project attempts to handle three sample types:
   - the source sample,\c saSourceType
   - the internal sample used for computations, \c saSample
   - the sink sample, \c saSinkType
   
 The ALSA device input and output types are "hard-wired" to \c int16_t in
 the files \c saDeviceType.h, \c saInputSource.h and \c saOutputSink.h,
 respectively.
 The file \c saSample.h defines the \c saSample internal type as a \c float
 and defines arithmetic operations on the \c saSample data with compile
 time options for saturation on over-flow, under-flow exceptions and over-flow
 exceptions. Similarly, the file \c saSampleTest.h defines the \c saSampleTest
 internal type as a \c int32_t. In both cases, the base type template is defined
 in \c saSampleBase.h.

 \subsection preprocessor_sec Pre-Processing
 The file \c PreProcessor.cc implements preprocessing of the input audio
 stream:
   - low-pass filter and decimate the input signal
   - high-pass filter to remove DC component
   - apply AGC

 \c ButterworthFilter.h implements fourth-order low-pass and second-order
 high-pass Butterworth filters. \c AutomaticGainControl.h implements a
 simple automatic gain control.

 \subsection pitchest_sec Pitch estimation
 \c PitchTracker.cc implements the YIN pitch estimator algorithm.

 \subsection testing_sec Testing
 The shell scripts in the \c test directory test the command line programs that
 exercise the preprocessing and pitch estimation routines. Run them under
 <a href="https://www.valgrind.org/">valgrind</a> with:
 \code
 $ export VALGRIND_CMD="valgrind -q --leak-check=full"
 $ for file in `ls -1 test/00/t00??a.sh` ; do sh $file ; done
 \endcode
 
  \subsection gui_sec GUI
 \c PitchTuner.cc implements a <a href="https://www.wxwidgets.org/">wxWidgets</a>
 GUI that displays the pitch estimate on a meter. The musical notes displayed
 are the 12-tone equal-tempered scale (12-TET). The *Options* are displayed by
 the keyboard combination *Alt+O*. For example, *Alt+O* followed by *Q* exits
 the program.

 The \c PitchTuner command-line options are:
 \code
$ bin/PitchTuner --help
Usage: PitchTuner [-?] [--file <str>] [--device <str>] [--sample_rate <num>]
[--channel <num>] [--latency_ms <num>] [--subsample <num>] [--lpcutoff <num>]
[--hpcutoff <num>] [--disable_hp_filter] [--disable_agc] [--window_ms <num>]
[--lags_ms <num>] [--sample_ms <num>] [--threshold <double>] [--removeDC]
[--A4Frequency <num>] [--gui_test] [--debug]
  -?, --help          	show help message
  --file=<str>        	input file
  --device=<str>      	input device
  --sample_rate=<num> 	sample rate
  --channel=<num>     	input device channel
  --latency_ms=<num>  	device storage latency (ms)
  --subsample=<num>   	waveform subsample ratio
  --lpcutoff=<num>    	lowpass cutoff frequency
  --hpcutoff=<num>    	highpass cutoff frequency
  --disable_hp_filter 	disable highpass filter
  --disable_agc       	disable AGC
  --window_ms=<num>   	correlation window width (ms)
  --lags_ms=<num>     	maximum correlation lags (ms)
  --sample_ms=<num>   	pitch estimate interval (ms)
  --threshold=<double>	difference function maximum
  --removeDC          	enable DC removal
  --A4Frequency=<num> 	nominal A4 frequency
  --gui_test          	enable GUI testing
  --debug             	enable debugging information
 \endcode
 
 On my system, wxWidgets returns a display PPI of \f$96\f$ rather than the
 expected value of \f$157\f$ and resizing the frame to \f$2000\f$ pixels or more
 causes problems. 

 Here is a screenshot of the GUI with \e tinwhistleD5.wav and \e pavucontrol set
 to record from the "Monitor of Built-in Audio":
 \image html GUI.png
 
 The original icon is
 <a href="https://www.flaticon.com/free-icon/tuning-fork_1005015">here</a>.
 The estimated pitch is shown as \f$599.7\f$ Hz. The large font shows the
 nearest equal-tempered note and frequency. The pitch error is shown in
 \e cents. The interval between two frequencies \f$f_{1}\f$ and \f$f_{2}\f$
 is \f$ 1200\;log_{2}\left(\frac{f_{2}}{f_{1}}\right) \f$ cents. Thus, a
 cent is a \f$100\f$'th of a semi-tone, there are \f$1200\f$ cents in an
 octave and \f$30\f$ cents corresponds to a frequency ratio of
 \f$1.01747969\f$. The two indicator circles are green if the pitch error
 is in the range \f$\pm 15\f$ cents.

 Here is a screenshot of the GUI options dialog:
 \image html GUI_options.png

 \section example_sec Examples of pitch estimation
 
 \subsection midi_piano_sec Equal-tempered C Major scale played on a piano
 The <a href="https://octave.org">Octave</a> script \e pianof0.m calls
 <a href="https://abcmidi.sourceforge.io">abc2midi</a> and
 <a href="https://timidity.sourceforge.net">timidity</a> 
 to generate a \e wav file containing the equal-tempered C Major scale played on 
 a piano. The \e abc file is:
 \code
 X:1 
 T:Notes on piano
 M:4/4
 L:1/4
 Q:1/4=120
 K:C 
 C D E F G A B c
 \endcode

 The \c PitchTracker_test command is:
 \code
$ bin/PitchTracker_test --debug --removeDC --sampleRate 48000 \
  --subSample 2 --inputLpFilterCutoff 1000 --baseLineHpFilterCutoff 200 \
  --threshold 0.100000 --msTsample 1 --file piano.wav
 \endcode
 
 The following figure shows the minimum of the YIN cumulative mean difference 
 function:
 \image html pianof0minCDT.png width=600cm
 
 The following figure shows the \c PitchTracker_test.cc pitch estimates with
 the YIN algorithm:
 \image html pianof0.png width=600cm 
 
 \e pianof0.m finds the following average pitch estimates for the \e timidity
 piano sound font:
 
Note | Nominal F0 | Mean F0 estimate | Error(%) | Error(cents)
---- | ---------: | ---------------: | --------: | -----------:
C4 |  261.63 |  261.77 |  0.05 | 0.95
D4 |  293.66 |  293.53 |  -0.05 | -0.80
E4 |  329.63 |  329.71 |  0.02 | 0.41
F4 |  349.23 |  349.29 |  0.02 | 0.33
G4 |  392.00 |  392.18 |  0.05 | 0.83
A4 |  440.00 |  439.98 |  -0.01 | -0.10
B4 |  493.88 |  494.35 |  0.10 | 1.64
C5 |  523.25 |  523.77 |  0.10 | 1.70

\subsection heed_sec "heed"

The following figure shows the word "heed":
 \image html heed.png width=600cm 

 The \c PitchTracker_test command is:
 \code
$ bin/PitchTracker_test --debug --removeDC --sampleRate 10000 \
  --subSample --msWindow 25 --inputLpFilterCutoff 1000 \
  --baseLineHpFilterCutoff 200 --msTsample 1 --msTmax 10 \
  --threshold 0.100000 --file heed.wav
 \endcode

 The following figure shows the pitch estimates for the word "heed":
 \image html heedf0.png width=600cm 

The origin of the plot of pitch estimate is shifted in time with respect to the
input wave form by the input windowing latency.

The following figure shows the cumulative mean normalised difference function
for the word "heed" at \f$150\f$ms:
 \image html heed_cdt_150ms.png width=600cm 

The following figure shows the cumulative mean normalised difference function
for the word "heed" at \f$250\f$ms:
 \image html heed_cdt_250ms.png width=600cm 

The last two figures show why the pitch estimate approximately doubles.
*/
