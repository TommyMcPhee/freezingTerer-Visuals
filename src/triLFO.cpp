#include "triLFO.hpp"

triLFO::triLFO() {
    
}
triLFO::triLFO(float _freq, float _phase, float _amp, int _sampleRate) {
    frequency = _freq;
    phase = _phase;
    amplitude = _amp;
    sampleRate = _sampleRate;
    calculatePhaseInc();
}
float triLFO::getSample()
{
    float sample;
    checkPhase();
    sample = phase;
    sample = abs(phase * 2.0 - 1.0) * amplitude;
    incrementPhase();
    return sample;
}

void triLFO::setFreq(float _freq){
        frequency = _freq;
        calculatePhaseInc();
    }

void triLFO::setAmp(float _amp){
    amplitude = _amp;
}

void triLFO::setPhase(float _phase) {
    phase = _phase;
}

void triLFO::setSampleRate(int _sampleRate) {
    sampleRate = _sampleRate;
    calculatePhaseInc();
}

float triLFO::getPhaseInc() {
    return phaseInc;
}

void triLFO::calculatePhaseInc() {
    phaseInc = frequency / (float)sampleRate;
};

void triLFO::checkPhase() {
    if(phase > 1.0) { phase -= 1.0; }
}

void triLFO::incrementPhase(){
    phase += phaseInc;
}

float triLFO::getFrequency(){
    return frequency;
}

float triLFO::getAmplitude() {
    return amplitude;
}

float triLFO::getPhase() {
    return phase;
}

int triLFO::getSampleRate() {
    return sampleRate;
}
