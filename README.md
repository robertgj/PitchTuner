## PitchTuner : Implementation of a YIN pitch estimator and GUI for Linux

This project implements a GUI for the YIN pitch estimator. See:
"YIN, A fundamental frequency estimator for speech and music",
A. de Cheveigne and H. Kawahara, Journal of the Acoustical Society of
America, Vol. 11, No. 4, April 2002, pp. 1917-1930.

This project is written in C++ and runs on Linux with the ALSA sound system. 
The ALSA audio input is referred to as "*default*" and it is up to the user to 
configure the audio mixer with an application like **pavucontrol** or 
**alsamixer**. The GUI displays the pitch estimate on a meter. The musical 
notes displayed are the 12-tone equal-tempered scale (12-TET). 

Assuming the necessary Linux libraries (**wxWidgets**), library header files and 
tools (**g++** and **doxygen**) are present, running **make** in the 
top-level directory *should* build this project.
 
### External source code and licensing

The *docs* directory contains a fork of the [Poole](http://getpoole.com)
theme for the [Jekyll](http://jekyllrb.com) static web page generator.

The GUI is built with [wxWidgets](https://www.wxwidgets.org).

The original icon is
[here](https://www.flaticon.com/free-icon/tuning-fork_1005015).

Other code is licensed under the [MIT licence](LICENCE).
