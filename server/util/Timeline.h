#pragma once
#include <SFML/System/Time.hpp> 
#include <SFML/System/Clock.hpp> 

using namespace sf;

class Timeline {
private:
	Clock baseTimeline;

	float startTime;
	float deltaT;
	int scaleFactor;
	float inline getCurrentRealTime();

public:
	explicit Timeline(float deltaT = 1.0f);
	void setScaleFactor(int newFactor);
	float getTime();
	void updateFrame();
};