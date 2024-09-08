#include <SFML/System.hpp>
#include "Timeline.h"

using namespace sf;

Timeline::Timeline(float deltaT) {
	this->baseTimeline = Clock();
	this->startTime = getCurrentRealTime();
	this->deltaT = deltaT;
	this->scaleFactor = 1;
}

float inline Timeline::getCurrentRealTime() {
	return this->baseTimeline.getElapsedTime().asSeconds();
}

float Timeline::getTime() {
	float now_time = getCurrentRealTime();
	float elapsed_time_raw = now_time - this->startTime;
	return (elapsed_time_raw / this->deltaT) * this->scaleFactor;
}

void Timeline::setScaleFactor(int newScale) {
	this->scaleFactor = newScale;
}

void Timeline::updateFrame() {
	this->baseTimeline.restart();
}