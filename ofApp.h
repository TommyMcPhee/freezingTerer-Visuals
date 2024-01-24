#pragma once

#include "ofMain.h"
#include "sinOsc.hpp"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void draw();
		void keyReleased(int key);
		void ofSoundStreamSetup(ofSoundStreamSettings &settings);
		void audioOut(ofSoundBuffer &buffer);

		void audioSetup();
		void videoSetup();
		inline float unipolar(float input);
		void refresh();
		void setUniforms();
		ofSoundStreamSettings settings;
		ofSoundStream stream;
		const int sampleRate = 48000;
		const int bufferSize = 256;
		const int channels = 2;
		sinOsc oscillatorA;
		sinOsc oscillatorB;
		const float length = 210.0;
		float feedback;
		float feedbackIncrement;
		ofFbo buffer;
		ofShader shader;
		float oscillatorASample;
		float oscillatorBSample;
		ofVec2f oscillators;
		float width;
		float height;
		ofVec2f window;
		
};
