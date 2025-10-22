# RNBO_FFT_ModulationFilter
This is a Filter combining fft~ and oscillator by RNBO (Max Msp).




I have also arranged the code to use the [AudioProcessorValueTreeState](https://docs.juce.com/master/classAudioProcessorValueTreeState.html).
Built from the [rnbo.example.juce](https://github.com/Cycling74/rnbo.example.juce).  

For *Getting Started*, please refer to the original repository. Also included in this repository is the C++ program output from RNBO in `export/`. And Max patch is included in patches/.

## System Algorithm
This system utilizes the property that **convolution in the time domain** = **multiplication in the frequency domain**, and is a filter that is expressed by multiplying a window function in the frequency domain.  
So, how can we ensure that one period of the window function perfectly covers the range from 0 to the Nyquist frequency?

First, we need to determine the frequency resolution (per bin).  
The frequency per bin can be calculated by (sample rate / FFT size). For example, an FFT size of 1024 gives a frequency of approximately 43Hz. In the case of max msp's fft~, samples after 512 (after the Nyquist frequency) are folded over and are not used as data.  
In other words, only half the data (512 samples) is used, so the frequency per bin is 86Hz.

What this 86Hz means is that it is the frequency representation of the FFT frame length of 1024 samples.  
As a result, multiplying by an 86Hz window function allows us to cover one frame of the FFT.

This time, I have prepared a triangle wave and a sawtooth wave as window functions.

## Demonstration
[Youtube<img width="766" alt="filterMod" src="https://github.com/user-attachments/assets/f92c8238-1696-46d4-83ca-5d71eb8bde5c"/>](https://youtube.com/shorts/zf1GzuQChmU)

## Parameter explanation
- **filterType** : There is no deep technical meaning behind this item. It is generally assumed that Type A will be used.  
 In the frequency domain, to express the window function flowing from high to low frequencies, or from low to high frequencies, due to the nature of fft~, it is necessary to apply * -1 to the frequency data of the window function from bin indices after the Nyquist frequency.  
In Type B, experimentally, rather than applying * -1 to the frequency data, I applied the amplitude value of the window function * -1.

- **direction** : In the frequency domain, you can control the direction in which the window function flows: from high to low frequencies or from low to high frequencies.

- **dutyCycle** : You can control the shape of the window function. A dutyCycle of 0 produces a rising sawtooth wave, 0.5 produces a triangle wave, and 1 produces a falling sawtooth wave.

- **freq** : You can control the frequency of the window function. Specifically, the default is (sample rate / FFT size) * 2 Hz, and you can control the offset value from there.

## Reference
- [jr-granular](https://github.com/szkkng/jr-granular)  
- [Tutorial:Saving and loading your plug-in state](https://juce.com/tutorials/tutorial_audio_processor_value_tree_state/)
