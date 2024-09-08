#pragma once
#include <iostream>
#include "duktape.h"

using namespace std;

class ScriptManager {
private:
	static duk_context *ctx;
	static ScriptManager* instance;
	static void runtimeErrorHandler(void *udata, const char *msg);
	static duk_ret_t nativeMoveAutoMovable(duk_context *cx);	// Requires that a AutoMovable * be passed onto the ctx
	static duk_ret_t nativeDeathEvent(duk_context *cx);			// Requires that a Renderable * be passed onto the ctx
	ScriptManager();
public:
	static ScriptManager* getInstance();
	void runScript(string *javascript);
	~ScriptManager();
};