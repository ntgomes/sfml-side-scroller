#include "ScriptManager.h"
#include "AutoMovable.h"
#include "GameEventManager.h"

// Defining static variables and private constructors
ScriptManager::ScriptManager() {};
ScriptManager* ScriptManager::instance;
duk_context* ScriptManager::ctx;

void ScriptManager::runtimeErrorHandler(void *udata, const char *msg) {
	(void)udata;  /* ignored in this case, silence warning */

	/* Note that 'msg' may be NULL. */
	fprintf(stderr, "*** DUKTAPE FATAL ERROR: %s\n", (msg ? msg : "no message"));
	fflush(stderr);
	abort();
}

duk_ret_t ScriptManager::nativeMoveAutoMovable(duk_context *cx) {
	void *am_raw = (void *)((int)duk_get_number(cx, -2));
	void *mt_raw = (void *)((int)duk_get_number(cx, -1));
	AutoMovable *am = static_cast<AutoMovable *>(am_raw);
	Timeline *tim = static_cast<Timeline *>(mt_raw);
	am->move(tim);
	return 0;
}

duk_ret_t ScriptManager::nativeDeathEvent(duk_context *cx) {
	Renderable *r = static_cast<Renderable *>((void *)((int)duk_get_number(cx, -2)));

	// Get the color of the renderable (for use of spawning the renderable)
	Color *oldColor = new Color(r->getColor());
	// Kill the character by setting it invisible
	r->setColor(Color((int) duk_get_number(cx, -1), (int)duk_get_number(cx, -1), 
		(int)duk_get_number(cx, -1), (int)duk_get_number(cx, -1)));

	GameEvent *reviveEvent = new GameEvent(GameEventManager::getInstance()->getCurrentEventTime(), 0, EventType::CHARACTER_SPAWN, 2, r, oldColor);
	GameEventManager::getInstance()->raiseEvent(reviveEvent);
	return 0;
}

ScriptManager* ScriptManager::getInstance() {
	if (instance == nullptr) {
		instance = new ScriptManager();
		ctx = duk_create_heap(NULL, NULL, NULL, NULL, runtimeErrorHandler);
		duk_push_c_function(ctx, nativeMoveAutoMovable, 2);
		duk_put_global_string(ctx, "moveAutoMovable");
		duk_push_c_function(ctx, nativeDeathEvent, 2);
		duk_put_global_string(ctx, "killChar");
	}
	return instance;
}

void ScriptManager::runScript(string *javascript) {
	duk_eval_string_noresult(this->ctx, javascript->c_str());
}

ScriptManager::~ScriptManager() {
	duk_destroy_heap(this->ctx);
}
