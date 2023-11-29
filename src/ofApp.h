#pragma once

#include "ofMain.h"
#include "sinOsc.hpp"
#include "triLFO.hpp"

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void exit();
	void ofSoundStreamSetup();
	void audioIn(ofSoundBuffer& buffer);

	void audioSetup();
	void refresh();
	void updateParameters();

	ofSoundStream stream;
	ofSoundStreamSettings settings;
	ofShader shader;
	ofFbo buffer0;
	ofFbo buffer1;
	vector<float> inputBuffer;
	int sampleRate;
	int bufferSize;
	float bufferRoot;
	float buffer4Root;
	float nyquist;
	int bufferIndex;
	array<float, 4> harmonics;
	float midIncrement;
	float highIncrement;
	array<float, 2> lastSample;
	array<float, 2> lastHighSample;
	array<float, 2> highSampleSum;
	array<float, 2> lastMidSample;
	array<float, 2> midSampleSum;
	array<float, 2> sample;
	array<float, 2> sampleSum;
	array<float, 2> sampleAbsSum;
	array<float, 2> sampleDifference;
	array<float, 2> sampleDifferenceLow;
	array<float, 2> sampleDifferenceMid;
	array<float, 2> sampleDifferenceHigh;
	array<float, 2> sampleDirectional;
	array<float, 2> sampleAmplitude;
	array<float, 2> low;
	array<float, 2> mid;
	array<float, 2> high;
	float length;
	float harmonicDivisor;
	float progress;
	float progressIncrement;
	float height;
	float width;
	float x;
	float y;
	float oldMix;
	float feedback;
	ofVec2f window;
	ofVec3f feedbackDistortion;
	ofVec4f power;
	ofVec4f feedbackPower;
	ofMatrix3x3 newX;
	ofMatrix3x3 newY;
	ofMatrix3x3 oldX;
	ofMatrix3x3 oldY;
	ofMatrix3x3 distortX;
	ofMatrix3x3 distortY;
	ofMatrix3x3 displace;
	ofMatrix3x3 amplitude;
	array<sinOsc, 4> oscillators;
	array<float, 4> samples;
};