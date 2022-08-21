# Octane

## Polyphonic Wavetable Synthesizer

Octane is a powerful plugin synthesizer built with the JUCE framework. The flexibility of Octane lies in its four anti-aliasing wavetable oscillators, which support preset waveforms as well as user waveforms imported from an audio file.

### Features

- Intuituve drag-and-drop modulation routing system
- 4 band-limited, anti-aliasing wavetable oscillators
- 6 voices of polyphony
- Four LFOs with customizable wave shapes

### What is wavetable synthesis?

Wavetable synthesis is a type of digital audio synthesis which produces waveforms based on data stored in a 'wavetable'.
A wavetable is a digital representation of a single cycle of some waveform- usually stored as a small, single-channel audio file.
A wavetable oscillator works based on interpolation between sample values in the wavetable. Each call for the next sample from the oscillator will advance the point in the wavetable from which the next sample will be calculatd. The oscillator's phase within the wavetable is advanced based on the oscillator's frequency. In other words, wavetable synths work by calculating sample values based on cycling through the wavetable at the given frequency.

In practice this means that wavetable synthesizers like Octane are a great tool for creating rich synth sounds beyond the limits of other types of synthesis. This provides
a world of flexibility and options for musicians and producers.

### Wavetable oscillator considerations

Since wavetable synthesis works by reproducing a waveform based on interpolating between table values to reproduce a waveform at any frequency, the upper harmonics of that waveform change along with the oscillator's frequency. This is where the nature of digital audio poses an issue. At higher pitches, this means that many of the waveform's upper harmonics surpass the [Nyquist frequency](https://en.wikipedia.org/wiki/Nyquist_frequency). The Nyquist frequency is equal to one half the oscillator's sample rate. 44.1kHz is a widely favored sample rate for this reason, the Nyquist frequency is 22.05kHz-- just outside the upper limits of [human hearing](https://en.wikipedia.org/wiki/Hearing_range). The upper harmonics above the Nyquist frequency are reproduced at a lower frequency than the fundamental thanks to the phenomenon of [aliasing](https://en.wikipedia.org/wiki/Aliasing). At higher pitches, aliasing can result in very unpleasant distortion in the oscillator's sound.

### Limiting upper harmonics

To prevent that distortion, I made use of a concept known as bandlimiting.
Bandlimiting allows the upper harmonics which pass the Nyquist frequency to be removed from the signal while all others remain intact.
The process of creating band-limited wavetables requires a bit of more complex math but the core concept is fairly simple.

#### The bandlimiting algorithm

Each oscillator has a a set of 10 wavetables; each of which has had all its harmonics above some frequency removed.
The process of generating these tables is the core of the bandlimiting algorithm. Each table goes through this process with its own maximum frequecy.

1. The original wavetable is represented as an array of complex numbers. The imaginary part of the array stores the wave data, while the real part values are all set to zero to start.
2. The complex number arrays are put into a Fast Fourier Transform algorithm to convert the wave from the time domain to the frequency domain.
3. The array members which represent harmonics above the wavetable's designated maximum frequency are set to zero.
4. The complex number arrays are again put into the FFT algorithm to convert the frequency domain representation of the wave back into a discrete time representation.
5. The maximum frequency is divided by two and the next table is created.

Based on its spectral content, each table has a designated frequency range. The function to get the next sample from the oscillator requires a frequency parameter which is needed to determine which table should be used to calculate the output sample.
