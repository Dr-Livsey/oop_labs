#pragma once
#include "Application.h"

class Controller : public OgreBites::InputListener, public OgreBites::TrayListener
{
public:
	Controller(Application *parent = 0);

	bool keyPressed(const OgreBites::KeyboardEvent& evt);

	bool mousePressed(const OgreBites::MouseButtonEvent& evt);
	bool mouseReleased(const OgreBites::MouseButtonEvent& evt);
	bool mouseMoved(const OgreBites::MouseMotionEvent& evt);

	void buttonHit(OgreBites::Button *b);

	~Controller();
private:
	Application *app;
};

