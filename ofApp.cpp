#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	feedback = 1.0;
	audioSetup();
	videoSetup();
}

//--------------------------------------------------------------
void ofApp::draw(){
	refresh();
	buffer.begin();
	shader.begin();
	setUniforms();
	buffer.draw(0, 0);
	shader.end();
	buffer.end();
	buffer.draw(0.0, 0.0);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings& settings) {

}

void ofApp::audioOut(ofSoundBuffer &buffer) {
	for (int a = 0; a < bufferSize; a++) {
		oscillatorASample = unipolar(oscillatorA.getSample());
		oscillatorBSample = unipolar(oscillatorB.getSample());
		oscillators.set(oscillatorASample, oscillatorBSample);
		//feedback += feedbackIncrement;
	}
}

void ofApp::audioSetup() {
	oscillatorA = sinOsc(1.0 / length, 0.0, 1.0, sampleRate);
	oscillatorB = sinOsc(1.6180339887 / length, 0.0, 1.0, sampleRate);
	//feedbackIncrement = 1.0 / (length * (float)sampleRate);
	settings.setOutListener(this);
	settings.sampleRate = sampleRate;
	settings.bufferSize = bufferSize;
	settings.numOutputChannels = channels;
	settings.setApi(ofSoundDevice::Api::MS_DS);
	stream.setup(settings);
}

void ofApp::videoSetup() {
	shader.load("zeroGravity");
	buffer.allocate(ofGetScreenWidth(), ofGetScreenHeight());
	buffer.begin();
	ofClear(0, 0, 0, 255);
	buffer.end();
}

inline float ofApp::unipolar(float input) {
	return input * 0.5 + 0.5;
}

void ofApp::refresh() {
	width = (float)ofGetWidth();
	height = (float)ofGetHeight();
	buffer.allocate(width, height);
	window.set(width, height);
	ofClear(0, 0, 0, 255);
}

void ofApp::setUniforms() {
	shader.setUniform2f("window", window);
	shader.setUniform2f("oscillators", oscillators);
	shader.setUniform1f("feedback", feedback);
}