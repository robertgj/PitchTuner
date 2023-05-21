---
layout: post
title: PitchTuner. An implementation of a YIN pitch estimator and GUI for Linux.
---
## Introduction
This project implements a GUI for the *YIN* [1] pitch estimator.

More recent implementations of pitch estimators are available on the web.

*pYIN* [2] adds a Markov model to make multiple YIN pitch estimates then performs
Viterbi decoding of the pitch trajectory to find a final precise estimate.
Software is [available](http://code.soundsoftware.ac.uk/projects/pyin).

*CREPE* [3] is "... based on a deep convolutional neural network that operates 
directly on the time-domain waveform".
Software is [available](https://github.com/marl/crepe).

*Melodia* [4] is a "system for the automatic extraction of the main melody from 
polyphonic music recordings". 

[essentia](https://essentia.upf.edu) is "... a C++ library for audio and music
analysis, description and synthesis". It includes implementations of pYIN,
CREPE and Melodia.

#### References

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

## The YIN algorithm
#### Step 1 : Autocorrelation
Use the autocorrelation method. For integration window, *W* :

$$ r_{t}(\tau)=\sum_{j=t+1}^{t+W}x_{j}x_{j+\tau} $$

#### Step 2 : Difference function
Model as periodic:

$$ x_{t}-x_{t+T}=0 $$

so:

$$ \sum_{j=t+1}^{t+W}(x_{j}-x_{j+T})^{2}=0$$
 
For unknown period, \\(T\\), define:

$$ d_{t}(T)=\sum_{j=t+1}^{t+W}(x_{j}-x_{j+T})^{2}$$
 
and search for values of T at which \\(d_{t}=0\\).

Expand \\(d_{t}(T)\\) in terms of \\(r_{t}\\):

$$ d_{t}(T)=r_{t}(0)+r_{t+T}(0)-2r_{t}(T)$$

For \\(d_{t}(T)\\) the sample time lies in \\(t-\frac{T}{2}-\frac{W}{2}+1\\)
to \\(t+\frac{T}{2}+\frac{W}{2}\\). For convenience \\(T<2W\\) and
the sample time lies in \\(t-\frac{3W}{2}+1\\) to \\(t-\frac{3W}{2}\\).

#### Step 3 : Cumulative mean normalised difference:
 
$$ d'_{t}(T)=\left\{ \begin{array}{cc}
         1 & \text{if } T=0 \\
         d_{t}(T)/\left[\frac{1}{T}\sum_{j=1}^{T}d_{t}(j)\right] &
         \text{ otherwise}
      \end{array}\right.$$
      
 Benefits:
   - reduces ``too high'' errors
   - don't need upper frequency limit on search to avoid zero-lag dip
   - normalises for next step

#### Step 4 : Absolute threshold
 Set threshold. Choose the smallest \\(\tau\\) giving minimum of \\(d_{t}'(T)\\)
 deeper than that threshold.

#### Step 5 : Parabolic interpolation
 Increase accuracy by parabolic interpolation near each minimum of
\\(d_{t}(T)\\) and \\(d_{t}'(T)\\).
 
#### Step 6 : Best local estimate
For each time index \\(t\\), search for a minimum of
\\(d_{\theta}'(T_{\theta})\\) for \\(\theta\\) within a small interval
\\(\left[t-T_{max}/2,\,t+T_{max}/2\right]\\), where \\(T_{\theta}\\) is the
estimate at time \\(\theta\\) and \\(T_{max}\\) is the largest expected period.
Based on this initial estimate, the estimation algorithm is applied again
with a restricted search range to obtain the final estimate.
 

## Implementation
This project is written in C++ with the [wxWidgets](https://www.wxwidgets.org)
toolkit and runs on Linux with the ALSA sound system. The ALSA audio input is 
referred to as "*default*" and it is up to the user to configure the audio mixer 
with an application like **pavucontrol** or **alsamixer**.

The GUI displays the pitch estimate on a meter. The musical notes displayed 
are the 12-tone equal-tempered scale (12-TET). Here is a screenshot of the GUI 
for *tinwhistleD5.wav* with **pavucontrol** set to record from the "*Monitor of 
Built-in Audio*":

![]({{ site.baseurl }}/public/GUI.png#center)

The estimated pitch is shown as \\(599.7\\) Hz. The large font shows the nearest
equal-tempered note and frequency. The pitch error is shown in *cents*.
The interval between two frequencies \\(f_{1}\\) and \\(f_{2}\\) is
\\( 1200\;log_{2}\left(\frac{f_{2}}{f_{1}}\right)\\) cents.
Thus, a cent is a \\(100\\)'th of a semi-tone and there are \\(1200\\) 
cents in an octave. \\(30\\) cents corresponds to a frequency ratio of 
\\(1.01747969\\).The two indicator circles are green if the pitch error is in 
the range \\(\pm 15\\) cents. The *Options* are displayed by the keyboard
combination *Alt+O*. For example, *Alt+O* followed by *Q* exits the program.

 The **PitchTuner** command-line options are:
```
$ bin/PitchTuner --help
Usage: PitchTuner [-?] [--file <str>] [--device <str>] 
[--sample_rate <num>] [--channel <num>] [--latency_ms <num>] 
[--subsample <num>] [--lpcutoff <num>] [--hpcutoff <num>] 
[--disable_hp_filter] [--disable_agc] [--window_ms <num>] 
[--lags_ms <num>] [--sample_ms <num>] [--threshold <double>] 
[--removeDC] [--A4Frequency <num>] [--gui_test] [--debug]
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
```

### Examples of pitch estimation

#### Equal-tempered C Major scale played on a piano
The [Octave](https://octave.org) script, *pianof0.m*, calls
[abc2midi](https://abcmidi.sourceforge.io) and
[timidity](https://timidity.sourceforge.net)
to generate a *wav* file containing the equal-tempered C Major scale played on 
a piano. The *abc* file is:
```
 X:1 
 T:Notes on piano
 M:4/4
 L:1/4
 Q:1/4=120
 K:C 
 C D E F G A B c
```

 The **PitchTracker_test** command is:
```
$ bin/PitchTracker_test --debug --removeDC --sampleRate 48000 \
  --subSample 2 --inputLpFilterCutoff 1000 \
  --baseLineHpFilterCutoff 200 --threshold 0.100000 \
  --msTsample 1 --file piano.wav
```

The following figure shows the minimum of the YIN cumulative mean difference 
function:
![]({{ site.baseurl }}/public/pianof0minCDT.png#center)
 
The following figure shows the YIN pitch estimates:
![]({{ site.baseurl }}/public/pianof0.png#center)
 
*pianof0.m* finds the following average pitch estimates for the **timidity**
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

#### "heed"
The following figure shows the word "heed":
![]({{ site.baseurl }}/public/heed.png#center)

 The **PitchTracker_test** command is:
```
$ bin/PitchTracker_test --debug --removeDC --sampleRate 10000 \
  --subSample 1 --msWindow 25 --inputLpFilterCutoff 1000 \
  --baseLineHpFilterCutoff 200 --msTsample 1 --msTmax 10 \
  --threshold 0.1 --file heed.wav
```

The following figure shows the pitch estimates for the word "heed":
![]({{ site.baseurl }}/public/heedf0.png#center)

The origin of the plot of pitch estimate is shifted in time with respect to the
input wave form by the input window length plus maximum correlation lags.

The following figure shows the cumulative mean normalised difference function 
for the word "heed" at \\(150\\)ms:
![]({{ site.baseurl }}/public/heed_cdt_150ms.png#center)

The following figure shows the cumulative mean normalised difference function 
for the word "heed" at \\(250\\)ms:
![]({{ site.baseurl }}/public/heed_cdt_250ms.png#center)

The last two figures show why the pitch estimate approximately doubles.

## About this page
This page was generated by the [Jekyll](http://jekyllrb.com) static site
generator using the [Poole](http://getpoole.com) theme by
[Mark Otto]({{ site.baseurl }}/LICENSE.md). The equations were rendered by
[MathJax](https://www.mathjax.org/). The original favicon image is
[here](https://www.flaticon.com/free-icon/tuning-fork_1005015).
