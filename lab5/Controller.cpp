#include "Controller.h"
#include "Model.h"

Controller::Controller(Application *parent) : app(parent)
{
}


Controller::~Controller()
{
}

bool Controller::keyPressed(const OgreBites::KeyboardEvent& evt)
{
	if (evt.keysym.sym == OgreBites::SDLK_ESCAPE)
	{
		app->getRoot()->queueEndRendering();
	}
	else if (evt.keysym.sym == OgreBites::SDLK_SPACE)
	{
		std::cout << "\nSpace pressd\n";
	}
	return true;
}

void Controller::buttonHit(OgreBites::Button * b)
{
	if (b->getName() == "parse_button")
	{
		app->getModel()->parse_code("code.xml");
	}
	else if (b->getName() == "exit_button")
	{
		app->getRoot()->queueEndRendering();
	}
	else if (b->getName() == "nexti_button")
	{
		app->getModel()->exec_line();
	}
}


bool Controller::mousePressed(const OgreBites::MouseButtonEvent & evt)
{
	if (app->getTray()->mousePressed(evt)) return true;
	/* normal mouse processing here... */

	return true;
}

bool Controller::mouseReleased(const OgreBites::MouseButtonEvent & evt)
{
	if (app->getTray()->mouseReleased(evt)) return true;
	/* normal mouse processing here... */
	return true;
}

bool Controller::mouseMoved(const OgreBites::MouseMotionEvent & evt)
{
	if (app->getTray()->mouseMoved(evt)) return true;
	/* normal mouse processing here... */
	return true;
}