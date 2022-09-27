# qLouder
A tool to measure and design loudspeakers.

It is loosely inspired by the famous [Room EQ Wizard](https://www.roomeqwizard.com).
However, it aims to provide a better user experience as well as pleasing aesthetics.
qLouder can also be considered as the successor of qloud:
http://gaydenko.com/qloud/
https://github.com/molke-productions/qloud/

<img width="992" alt="qLouder v0.0.1" src="https://user-images.githubusercontent.com/1805183/160924496-d879493e-a46a-47d0-a49e-e64d4e891b1a.png">


# Target use
The measurement and design features of qLouder can help you to create an 
optimal filter set and crossover for your DIY loudspeaker.

# Measurement process
* generate and play back an audio test signal (sine sweep) via sound card, amplifier and loudspeaker box
* measure signal via measurement microphone
* generate impulse response from recorded signal
* time gate impulse response
* generate frequency response
* apply microphone calibration file
* design a target curve (house curve)
* design correction filters to match your target curve
* design a crossover for your 2-way speaker

# Current features
* Sine sweep generation (including proper windowing).
* Time gating of measured impulse response
* Microphone calibration
* Crossover
* Loudness based house curve
* Parametric equalizer
* Different sample rates for playback and recording
* Simple music player

# TODO
* SPL meter
* Persistent measurements
* Harman house curve
* Remove QtMultimedia and use Cinder instead
* Remove PulseAudio as hard dependency for Cinder
* Detect hot plugged audio devices

# Requirements
* fftw3
* Qt 5.x or 6.x (Core Quick QuickControls2 Multimedia Charts Svg Xml)
* libqt5multimedia5-plugins
* PulseAudio is running

# Download and compile
```
git clone https://github.com/mincequi/qLouder
cd qLouder
mkdir build && cd build
cmake ..
make
```

# Contribute
Contributations are always welcome. In the best case in the form of a pull request :)
