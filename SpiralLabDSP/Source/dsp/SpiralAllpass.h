/*
    SpiralLab DSP Library
    https://digitalmedia.ok.ubc.ca/spiral/
 
    Implementation by Yohanes Turnip,
    inspired by examples in Reiss & McPherson,
    "Audio Effects: Theory, Implementation and Application", 2nd ed.
*/

#pragma once
#include <vector>
#include <cmath>

class SpiralAllpass
{
public:
    void setSampleRate (double sr)      { sampleRate = sr; }
    
    void setCenterFrequency (float freqHz)
    {
        jassert (sampleRate > 0.0);

        centerFreq = freqHz;

        const float T  = 1.0f / (float) sampleRate;
        const float wc = 2.0f * juce::MathConstants<float>::pi * freqHz;
        const float K  = std::tan (wc * T * 0.5f;

        a = (1.0f - K) / (1.0f + K);  
    }
    
    void reset (int numChannels)
    {
        x1.assign ((size_t) numChannels, 0.0f);
        y1.assign ((size_t) numChannels, 0.0f);
    }
    
    float processSample (float x, int channel)
    {
        auto& x1c = x1[(size_t) channel];
        auto& y1c = y1[(size_t) channel];

        float y = -a * x + x1c + a * y1c;

        x1c = x;
        y1c = y;
        
        return y;
    }
    
private:
    double sampleRate { 44100.0 };
    float  centerFreq { 1000.0f };
    float  a          { 0.0f };

    std::vector<float> x1, y1;
};
