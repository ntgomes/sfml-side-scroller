#pragma once
#include <sstream>
#include <fstream>
#include "GameObjectManager.h"

using namespace std;

class ReplayManager {
private:
	string replayString;				// The string holding all replay data
	bool isRecording;					// The boolean controlling the new recording capability
	RenderWindow *replayer;				// The POINTER to the window that will play the replay
	static ReplayManager *instance;
	static Mutex replayLock;
	ReplayManager();
public:
	static ReplayManager *getInstance();
	void startRecording();				// If the isRecording value is false, then make it true 
	void record();						// If the isRecording value is true, then get all renderable positions
	void stopRecording();				// Turn isRecording value to false
	void replayRecording(RenderWindow *replayWindow);		// Parse through the replay string that was made from record()
};