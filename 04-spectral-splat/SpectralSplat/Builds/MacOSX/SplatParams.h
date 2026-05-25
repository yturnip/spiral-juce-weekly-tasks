#pragma once

struct SplatParams
{
    float x             = 0.0f;
    float y             = 0.0f;
    float z             = 0.0f;
    float density       = 0.5f; // drives freeze frequency + bin removal
    float eccentricity  = 0.5f; // drives bandwidth/resonance
    float shColor       = 0.0f; // drives spectral tilt/shift
}
