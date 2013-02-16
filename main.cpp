#include <iostream>
#include <OgreException.h>
#include <OgreConfigFile.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>

#include "main.h"
using namespace std;

ZombieClient::ZombieClient()
	: mRoot(0),
	mResourcesCfg(Ogre::StringUtil::BLANK),
	mPluginsCfg(Ogre::StringUtil::BLANK)
{
//Put constructor stuff here

}

ZombieClient::~ZombieClient(){
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow,this);
	windowClosed(mWindow);
	delete mRoot;
}

bool ZombieClient::go(){
	//set config files
#ifdef _DEBUG
	mResourcesCfg = "resources_d.cfg";
	mPluginsCfg = "plugins_d.cfg";
#else
	mResourcesCfg = "resources.cfg";
	mPluginsCfg = "plugins.cfg";
#endif

	//Parse config file
	Ogre::ConfigFile cf;
	cf.load(mResourcesCfg);

	//Construct root object
	mRoot = new Ogre::Root(mPluginsCfg);

	//Load resources
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
	Ogre::String secName, typeName, archName;
	while(seci.hasMoreElements()){
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for(i = settings->begin();i != settings->end(); i++){
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
					archName, typeName, secName);
		}
	}

	if(!(mRoot->restoreConfig() || mRoot->showConfigDialog())){
		return false;
	}

	mWindow = mRoot->initialise(true, "Zombie Client");

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	mSceneMgr = mRoot->createSceneManager("OctreeSceneManager");

	//Set up a basic scene
	mCamera = mSceneMgr->createCamera("PlayerCam");
	mCamera->setNearClipDistance(0.1);
	mSceneMgr->createSceneNode("PlayerCam")->attachObject(mCamera);

	Ogre::Viewport *vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
	mCamera->setAspectRatio(
			Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	createScene();

	// Initialize OIS for input
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW",&windowHnd);
	windowHndStr << windowHnd;
	pl.insert(make_pair(string("WINDOW"),windowHndStr.str()));

	mInputManager = OIS::InputManager::createInputSystem(pl);

	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, false));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, false));

	//Initialize Bullet for physics
	pBroadphase = new btDbvtBroadphase();

	pCollisionConfiguration = new btDefaultCollisionConfiguration();
	pDispatcher = new btCollisionDispatcher(pCollisionConfiguration);
 
	pSolver = new btSequentialImpulseConstraintSolver;

	pDynamicsWorld = new btDiscreteDynamicsWorld(pDispatcher,pBroadphase,pSolver,pCollisionConfiguration);
	pDynamicsWorld->setGravity(btVector3(0,-9.81,0));

	//Set intial mouse clipping size
	windowResized(mWindow);

	//register as window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow,this);

	//Register frame listener
	mRoot->addFrameListener(this);

	//Start rendering loop
	mRoot->startRendering();

	return true;
}

void ZombieClient::createScene(){
	Ogre::Entity *ogreHead = mSceneMgr->createEntity("Head","Cube.mesh");

	Ogre::SceneNode *headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("Head");
	headNode->attachObject(ogreHead);

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5,0.5,0.5));

	Ogre::Light *light = mSceneMgr->createLight("Light1");
	light->setPosition(20,80,50);
}

bool ZombieClient::frameRenderingQueued(const Ogre::FrameEvent& evt){
	static Ogre::Real mRotate = 0.13;
	Ogre::Real mMove = 10;
	if(mWindow->isClosed()){
		return false;
	}

	mKeyboard->capture();
	mMouse->capture();

	if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)){
		return false;
	}

	//WASD move camera
	Ogre::Vector3 cameraTrans = Ogre::Vector3::ZERO;

	if(mKeyboard->isKeyDown(OIS::KC_LSHIFT)){
		mMove = 100;
	}
	if(mKeyboard->isKeyDown(OIS::KC_W)){
		cameraTrans.z -= mMove;
	}
	if(mKeyboard->isKeyDown(OIS::KC_S)){
		cameraTrans.z += mMove;
	}
	if(mKeyboard->isKeyDown(OIS::KC_A)){
		cameraTrans.x -= mMove;
	}
	if(mKeyboard->isKeyDown(OIS::KC_D)){
		cameraTrans.x += mMove;
	}

	mSceneMgr->getSceneNode("PlayerCam")->translate(cameraTrans * evt.timeSinceLastFrame,
			Ogre::Node::TS_LOCAL);
	mSceneMgr->getSceneNode("PlayerCam")->yaw(
			Ogre::Degree(-mRotate * mMouse->getMouseState().X.rel),Ogre::Node::TS_WORLD);
	mSceneMgr->getSceneNode("PlayerCam")->pitch(
			Ogre::Degree(-mRotate * mMouse->getMouseState().Y.rel),Ogre::Node::TS_LOCAL);

	return true;
}

void ZombieClient::windowResized(Ogre::RenderWindow *rw){
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

void ZombieClient::windowClosed(Ogre::RenderWindow *rw){
	if(rw == mWindow){
		if(mInputManager){
			mInputManager->destroyInputObject(mMouse);
			mInputManager->destroyInputObject(mKeyboard);

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}
	}
}

int main(int argc, char *argv[]){

	ZombieClient game;

	try {
		game.go();
	} catch (Ogre::Exception& e) {
		cerr << "An exception has occured: " << e.getFullDescription().c_str() << endl;
	}
	return 0;
}
