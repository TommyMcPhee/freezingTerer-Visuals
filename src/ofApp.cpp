#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	audioSetup();
	length = 315.0;
	harmonicDivisor = length / 1.61803399;
	progress = 0.0;
	progressIncrement = (float)bufferSize / (length * (float)sampleRate);
	shader.load("discretionShader");
	buffer0.allocate(ofGetScreenWidth(), ofGetScreenHeight());
	buffer0.clear();
	buffer1.allocate(ofGetScreenWidth(), ofGetScreenHeight());
	buffer1.clear();
	buffer0.begin();
	ofClear(0, 0, 0, 255);
	buffer0.end();
	buffer1.begin();
	ofClear(0, 0, 0, 255);
	buffer1.end();
	x = 0.0;
	y = 0.0;
	oldMix = 0.5;
	feedback = 0.0;
	feedbackDistortion = ofVec3f(1.0, 1.0, 1.0);
	power = ofVec4f(1.0, 1.0, 1.0, 1.0);
	feedbackPower = ofVec4f(1.0, 1.0, 1.0, 1.0);
	newX = ofMatrix3x3(0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0);
	newY = ofMatrix3x3(0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0);
	oldX = ofMatrix3x3(0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0);
	oldY = ofMatrix3x3(0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0);
	distortX = ofMatrix3x3(0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0);
	distortY = ofMatrix3x3(0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0);
	displace = ofMatrix3x3(0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0);
	amplitude = ofMatrix3x3(1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
	harmonics = { 2.0, 3.0, 4.0, 5.0 };
	midIncrement = 0.0;
	highIncrement = 0.0;
	lastSample = { 0.0, 0.0 };
	lastHighSample = { 0.0, 0.0 };
	for (int a = 0; a < oscillators.size(); a++) {
		oscillators.at(a) = sinOsc(harmonics.at(a) / harmonicDivisor, 0.0, 1.0, (int)((float)sampleRate / bufferSize));
	}
}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {
	refresh();
	buffer0.begin();
	shader.begin();
	shader.setUniform1f("oldMix", oldMix);
	shader.setUniform1f("feedback", feedback);
	shader.setUniform2f("window", window);
	shader.setUniform3f("feedbackDistortion", feedbackDistortion);
	shader.setUniform4f("power", power);
	shader.setUniform4f("feedbackPower", feedbackPower);
	shader.setUniformMatrix3f("newX", newX);
	shader.setUniformMatrix3f("newY", newY);
	shader.setUniformMatrix3f("oldX", oldX);
	shader.setUniformMatrix3f("oldY", oldY);
	shader.setUniformMatrix3f("distortX", distortX);
	shader.setUniformMatrix3f("displace", displace);
	shader.setUniformMatrix3f("amplitude", amplitude);
	buffer1.draw(x, y);
	shader.end();
	buffer0.end();
	buffer1.begin();
	buffer0.draw(x, y);
	buffer1.end();
	buffer0.draw(x, y);
	//ofDrawRectangle(10.0, 10.0, 10.0, 10.0);
}

void ofApp::exit() {
	ofSoundStreamClose();
}

void ofApp::ofSoundStreamSetup() {

}

void ofApp::audioIn(ofSoundBuffer& buffer) {
	updateParameters();
	progress += progressIncrement;
	for (int a = 0; a < buffer.getNumFrames(); a++) {
		for (int b = 0; b < 2; b++){
			bufferIndex = a * 2 + b;
			sample[b] = buffer[bufferIndex];
			//inputBuffer.at(bufferIndex) = sample[b];
			sampleSum[b] += sample[b];
			sampleAbsSum[b] += abs(sample[b]);
			sampleDifference[b] += abs(lastSample[b] - sample[b]);
			lastSample[b] = sample[b];
		}
		if (highIncrement >= bufferRoot) {
			for (int a = 0; a < 2; a++) {
				sampleDifferenceHigh[a] = sampleDifference[a] / bufferRoot;
				highSampleSum[a] += abs(lastHighSample[a] - sampleDifferenceHigh[a]);
				lastHighSample[a] = sampleDifferenceHigh[a];
				sampleDifference[a] = 0.0;
				midIncrement++;
				if (midIncrement >= buffer4Root) {
					sampleDifferenceMid[a] = highSampleSum[a] / buffer4Root;
					midSampleSum[a] += abs(lastMidSample[a] - sampleDifferenceMid[a]);
					midIncrement = 0.0;
				}
			}
			highIncrement = 0.0;
		}
		highIncrement++;
	}
	for (int a = 0; a < 2; a++) {
		sampleDirectional[a] = sampleSum[a] / (float)buffer.getNumFrames();
		sampleSum[a] = 0;
		sampleAmplitude[a] = sampleAbsSum[a] / (float)buffer.getNumFrames();
		sampleAbsSum[a] = 0.0;
		sampleDifferenceLow[a] = midSampleSum[a] * buffer4Root / (float)bufferRoot;
		midSampleSum[a] = 0;
	}
}

void ofApp::audioSetup() {
	ofSoundStreamSettings settings;
	sampleRate = 48000;
	bufferSize = 512;
	bufferRoot = 2.0 * pow((float)bufferSize, 0.5);
	buffer4Root = pow(bufferRoot, 0.5);
	settings.setInListener(this);
	settings.sampleRate = sampleRate;
	settings.bufferSize = bufferSize;
	settings.numInputChannels = 2;
	settings.setApi(ofSoundDevice::Api::MS_DS);
	stream.setup(settings);
	inputBuffer.resize(bufferSize * 2);
	nyquist = (float)sampleRate * 0.5;
}

void ofApp::refresh() {
	width = (float)ofGetWidth();
	height = (float)ofGetHeight();
	buffer0.allocate(width, height);
	buffer1.allocate(width, height);
	ofClear(0, 0, 0, 255);
	window.set(width, height);
}

void ofApp::updateParameters() {
	for (int a = 0; a < oscillators.size(); a++) {
		samples[a] = oscillators[a].getSample();
	}
	oscillators[0].setFreq((1.0 - (sampleAmplitude[0] * sampleAmplitude[0] * sampleAmplitude[0])) * harmonics[0] / harmonicDivisor);
	oscillators[1].setFreq((1.0 - (sampleAmplitude[0] * sampleAmplitude[0] * sampleAmplitude[1])) * harmonics[1] / harmonicDivisor);
	oscillators[2].setFreq((1.0 - (sampleAmplitude[0] * sampleAmplitude[1] * sampleAmplitude[1])) * harmonics[2] / harmonicDivisor);
	oscillators[3].setFreq((1.0 - (sampleAmplitude[1] * sampleAmplitude[1] * sampleAmplitude[1])) * harmonics[3] / harmonicDivisor);
	float sum2a = samples.at(0) + samples.at(1);
	float sum2b = samples.at(0) + samples.at(2);
	float sum2c = samples.at(0) + samples.at(3);
	float sum2d = samples.at(1) + samples.at(2);
	float sum2e = samples.at(1) + samples.at(3);
	float sum2f = samples.at(2) + samples.at(3);
	float suma2 = sum2a * -1.0;
	float sumb2 = sum2b * -1.0;
	float sumc2 = sum2c * -1.0;
	float sumd2 = sum2d * -1.0;
	float sume2 = sum2e * -1.0;
	float sumf2 = sum2f * -1.0;
	float sum3a = samples.at(0) + samples.at(1) + samples.at(2);
	float sum3b = samples.at(0) + samples.at(1) + samples.at(3);
	float sum3c = samples.at(0) + samples.at(2) + samples.at(3);
	float sum3d = samples.at(1) + samples.at(2) + samples.at(3);
	float suma3 = sum3a * -1.0;
	float sumb3 = sum3b * -1.0;
	float sumc3 = sum3c * -1.0;
	float sumd3 = sum3d * -1.0;
	float sum4 = samples.at(0) + samples.at(1) + samples.at(2) + samples.at(3);
	float negative4 = sum4 * -1.0;
	float sum32a = sum3a + sum3b;
	float sum32b = sum3a + sum3c;
	float sum32c = sum3a + sum3d;
	float sum32d = sum3b + sum3c;
	float sum32e = sum3b + sum3d;
	float sum32f = sum3c + sum3d;
	float suma32 = sum32a * -1.0;
	float sumb32 = sum32b * -1.0;
	float sumc32 = sum32c * -1.0;
	float sumd32 = sum32d * -1.0;
	float sume32 = sum32e * -1.0;
	float sumf32 = sum32f * -1.0;
	float newXRPhase = sum2d * PI;
	float newXGPhase = sum2e * PI;
	float newXBPhase = sum2f * PI;
	float newYRPhase = sum2a * PI;
	float newYGPhase = sum2b * PI;
	float newYBPhase = sum2c * PI;
	float oldXRPhase = sum32d * TWO_PI / 3.0;
	float oldXGPhase = sum32e * TWO_PI / 3.0;
	float oldXBPhase = sum32f * TWO_PI / 3.0;
	float oldYRPhase = sum32a * TWO_PI / 3.0;
	float oldYGPhase = sum32b * TWO_PI / 3.0;
	float oldYBPhase = sum32c * TWO_PI / 3.0;
	float distortXRPhase = (sum2a + sum32a) * PI * 0.125;
	float distortXGPhase = (sum2b + sum32b) * PI * 0.125;
	float distortXBPhase = (sum2c + sum32c) * PI * 0.125;
	float distortYRPhase = (sum2d + sum32d) * PI * 0.125;
	float distortYGPhase = (sum2e + sum32e) * PI * 0.125;
	float distortYBPhase = (sum2f + sum32f) * PI * 0.125;
	oldMix = abs(sum4) / 4.0;
	feedback = pow((1.0 - progress), abs(negative4) / 4.0);
	feedbackDistortion.set(sampleAmplitude[0] - sampleAmplitude[1], sampleDirectional[0] + sampleDirectional[1], sampleAmplitude[0] + sampleAmplitude[1]);
	power.set(abs(samples.at(1) * (1.0 - (sampleDifferenceLow[0] * sampleDifferenceLow[1]))), abs(samples.at(3)) * (1.0 - (sampleDifferenceMid[0] * sampleDifferenceMid[1])), abs(samples.at(2)) * (1.0 - (sampleDifferenceHigh[0] * sampleDifferenceHigh[1])), abs(samples.at(0)));
	feedbackPower.set(abs(sum3c) / 3.0, abs(sum3a) / 3.0, abs(sum3b) / 3.0, abs(sum3d) / 3.0);
	newX.set(newXRPhase, sum2a, abs(sumd2), newXGPhase, sum2b, abs(sume2), newXBPhase, sum2c, abs(sumf2));
	newY.set(newYRPhase, sum2d, abs(suma2), newYGPhase, sum2e, abs(sumb2), newYBPhase, sum2f, abs(sumc2));
	oldX.set(oldXRPhase, suma2, abs(sum2d), oldXGPhase, sumb2, abs(sum2e), oldXBPhase, sumc2, abs(sum2f));
	oldY.set(oldYRPhase, sumd2, abs(sum2a), oldYGPhase, sume2, abs(sum2b), oldYBPhase, sumf2, abs(sum2c));
	distortX.set(distortXRPhase, sum2a + sum32b, abs(sum2a + sum32c), distortXGPhase, sum2a + sum32d, abs(sum2a + sum32e), distortXBPhase, sum2a + sum32f, abs(sum2b + sum32a));
	distortY.set(distortYRPhase, sum2b + sum32c, abs(sum2b + sum32d), distortYGPhase, sum2b + sum32e, abs(sum2c + sum32f), distortYBPhase, sum2c + sum32a, abs(sum2c + sum32b));
	displace.set((sum2c + sum32c) / 8.0, (sum2c + sum32e) / 8.0, abs(sum2c + sum32f), (sum2d + sum32a) / 8.0, (sum2d + sum32b) / 8.0, abs(sum2d + sum32c), (sum2d + sum32e) / 8.0, (sum2d + sum32f) / 8.0, abs(sum2e + sum32a));
	amplitude.set(abs((sum2e + sum32b) / 8.0), abs((sum2e + sum32c) / 8.0), abs((sum2e + sum32d) / 8.0), abs((sum2e + sum32f) / 8.0), abs((sum2f + sum32a) / 8.0), abs((sum2f + sum32b) / 8.0), abs((sum2f + sum32c) / 8.0), abs((sum2f + sum32d) / 8.0), abs((sum2f + sum32e) / 8.0));
}