#pragma once
#include <JuceHeader.h>

class RingModulator
{
public:
    void prepare(double sampleRate)
    {
        mSampleRate = sampleRate;
        mPhase = 0.0f;
    }
    
    float process(float inputSample, float carrierFreq, float depth)
    {
        float carrier = std::sin(juce::MathConstants<float>::twoPi * mPhase);
        
        mPhase += carrierFreq/ (float)mSampleRate;
        if (mPhase >= 1.0f)
            mPhase -= 1.0f;
        
        // depth 0 = dry, depth 1 = full ring mod
        return inputSample * (1.0f - depth + depth * carrier);
    }
    
private:
    double mSampleRate = 44100.0;
    float mPhase = 0.0f;
};
