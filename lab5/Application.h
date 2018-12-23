#pragma once
#include <Ogre.h> 
#include <OgreApplicationContext.h> 
#include <OgreInput.h> 
#include <OgreRTShaderSystem.h> 
#include <OgreOverlay.h> 
#include <OgreTrays.h>

#include <iostream>

class Controller;
class Model;

class Application : public OgreBites::ApplicationContext
{
public:
	Application();

	void setup(void);

	OgreBites::TrayManager* getTray() { return mTrayMgr; }
	Model* getModel() { return model; }
	
	OgreBites::TextBox *code_tb;
	OgreBites::TextBox *stack_tb;

	OgreBites::SelectMenu *esp;
	OgreBites::SelectMenu *ebp;
	OgreBites::SelectMenu *eip;
	OgreBites::SelectMenu *eax;

	~Application();

private:
	OgreBites::TrayManager* mTrayMgr;

	Controller *ctrl;
	Model	   *model;
};

