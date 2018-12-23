#include "Application.h"
#include "Controller.h"
#include "Model.h"

Application::Application() : OgreBites::ApplicationContext("OgreApp")
{
	ctrl = new Controller(this);
	model = new Model(this);
}


Application::~Application()
{
	delete ctrl;
	//delete mTrayMgr;
}

void Application::setup(void)
{
	// do not forget to call the base first
	OgreBites::ApplicationContext::setup();

	// register for input events
	addInputListener(ctrl);

	// get a pointer to the already created root
	Ogre::Root* root = getRoot();
	Ogre::SceneManager* scnMgr = root->createSceneManager();

	// register our scene with the RTSS
	Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
	shadergen->addSceneManager(scnMgr);

	scnMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

	// without light we would just get a black screen    
	Ogre::Light* light = scnMgr->createLight("MainLight");
	Ogre::SceneNode* lightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
	lightNode->attachObject(light);

	lightNode->setPosition(20, 80, 50);

	// also need to tell where we are
	Ogre::SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();

	// create the camera
	Ogre::Camera* cam = scnMgr->createCamera("myCam");
	cam->setNearClipDistance(5); // specific to this sample
	cam->setAutoAspectRatio(true);
	camNode->setPosition(200, 300, 400);
	camNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TransformSpace::TS_WORLD);
	camNode->attachObject(cam);

	// and tell it to render into the main window
	Ogre::Viewport *vp = getRenderWindow()->addViewport(cam);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

	mTrayMgr = new OgreBites::TrayManager("InterfaceName", getRenderWindow());
	scnMgr->addRenderQueueListener(getOverlaySystem());
	mTrayMgr->setListener(ctrl);
	mTrayMgr->showLogo(OgreBites::TL_TOP);
	mTrayMgr->showBackdrop("Examples/Rockwall");

	code_tb = mTrayMgr->createTextBox(OgreBites::TL_TOPLEFT,   "code_tb",  "Code",  350, 450);
	stack_tb = mTrayMgr->createTextBox(OgreBites::TL_TOPRIGHT, "stack_tb", "Stack", 350, 450);

	esp = mTrayMgr->createLongSelectMenu(OgreBites::TL_BOTTOMLEFT,  "esp", "ESP", 280, 1, { "0x00000000" });
	ebp = mTrayMgr->createLongSelectMenu(OgreBites::TL_BOTTOMLEFT,  "ebp", "EBP", 280, 1, { "0x00000000" });
	eip = mTrayMgr->createLongSelectMenu(OgreBites::TL_BOTTOMRIGHT, "eip", "EIP", 280, 1, { "0x00000000" });
	eax = mTrayMgr->createLongSelectMenu(OgreBites::TL_BOTTOMRIGHT, "eax", "EAX", 280, 1, { "0x00000000" });

	mTrayMgr->createSeparator(OgreBites::TL_BOTTOMRIGHT, "sbr", 10);
	mTrayMgr->createSeparator(OgreBites::TL_BOTTOMLEFT, "sbl", 10);

	mTrayMgr->createButton(OgreBites::TL_BOTTOMLEFT, "parse_button", "Parse XML", 150);
	mTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "exit_button", "Exit", 150);
	mTrayMgr->createButton(OgreBites::TL_BOTTOM, "nexti_button", "Execute instruction", 150);	
	
}