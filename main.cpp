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
	mSceneMgr->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_MODULATIVE);

	//Initialize Bullet for physics
	pBroadphase = new btDbvtBroadphase();

	pCollisionConfiguration = new btDefaultCollisionConfiguration();
	pDispatcher = new btCollisionDispatcher(pCollisionConfiguration);
 
	pSolver = new btSequentialImpulseConstraintSolver;

	pDynamicsWorld = new btDiscreteDynamicsWorld(pDispatcher,pBroadphase,pSolver,pCollisionConfiguration);
	pDynamicsWorld->setGravity(btVector3(0,-9.81,0));

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

	Ogre::Vector3 size;

	Ogre::Entity *room = mSceneMgr->createEntity("Room","Cube.mesh");
	Ogre::Entity *cube = mSceneMgr->createEntity("Cube","Cube.001.mesh");
	room->setCastShadows(false);
	cube->setCastShadows(true);

	Ogre::SceneNode *roomNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("Room");
	Ogre::SceneNode *cubeNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("Cube");
	roomNode->attachObject(room);
	cubeNode->attachObject(cube);

	//mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2,0.2,0.2));

	size = cube->getBoundingBox().getSize();
	btVector3 boxVector(size.x,size.y,size.z);
	btCollisionShape *boxShape = new btBoxShape(boxVector);
	btDefaultMotionState* boxMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)));
	boxBody = new btRigidBody(btScalar(1),boxMotionState,boxShape,btVector3(0,0,0));
	pDynamicsWorld->addRigidBody(boxBody);

	Ogre::MeshPtr meshPtr = room->getMesh();
	Ogre::VertexData *vertexData = meshPtr->sharedVertexData;
	
	const Ogre::VertexDeclaration* vd= vertexData->vertexDeclaration;
	const Ogre::VertexElement *posElem = vd->findElementBySemantic(Ogre::VES_POSITION);
	Ogre::HardwareVertexBufferSharedPtr vbuf = vertexData->vertexBufferBinding->getBuffer(posElem->getSource());
	
	Ogre::Vector3 *triangles[100][3];
	int triangleCount = 0;
	float something[9];
	int offset = 0;
	int trianglesmax = (vbuf->getSizeInBytes()/vbuf->getVertexSize()/3);

	for(int i=0;i<trianglesmax;i++){
		//memset(something,0,3);

		vbuf->readData(offset,vbuf->getVertexSize(),&something);
		triangles[i][0] = new Ogre::Vector3(something[0],something[1],something[2]);
		//memset(something,0,3);
		offset += vbuf->getVertexSize();

		vbuf->readData(0,vbuf->getVertexSize(),&something);
		triangles[i][1] = new Ogre::Vector3(something[0],something[1],something[2]);
		//memset(something,0,3);
		offset += vbuf->getVertexSize();

		vbuf->readData(0,vbuf->getVertexSize(),&something);
		triangles[i][2] = new Ogre::Vector3(something[0],something[1],something[2]);
		//memset(something,0,3);
		offset += vbuf->getVertexSize();
		triangleCount++;
	}

	btTriangleMesh *trimesh = new btTriangleMesh();
	btVector3 *vertex1;
	btVector3 *vertex2;
	btVector3 *vertex3;

	for(int i=0;i<triangleCount;i++){
		vertex1 = new btVector3(triangles[i][0]->x,triangles[i][0]->y,triangles[i][0]->z);
		vertex2 = new btVector3(triangles[i][1]->x,triangles[i][1]->y,triangles[i][1]->z);
		vertex3 = new btVector3(triangles[i][2]->x,triangles[i][2]->y,triangles[i][2]->z);

		trimesh->addTriangle(*vertex1,*vertex2,*vertex3);
	}

	btBvhTriangleMeshShape *trimeshShape = new btBvhTriangleMeshShape(trimesh,true);
	btDefaultMotionState* triMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)));
	btRigidBody *trimeshBody = new btRigidBody(0,triMotionState,trimeshShape,btVector3(0,0,0));
	pDynamicsWorld->addRigidBody(trimeshBody);

	Ogre::Light *light = mSceneMgr->createLight("Light1");
	light->setPosition(10,5,5);
	light->setDiffuseColour(1,1,1);
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
	
	pDynamicsWorld->stepSimulation(evt.timeSinceLastFrame,NULL,NULL);
	btTransform trans;
	boxBody->getMotionState()->getWorldTransform(trans);
	mSceneMgr->getSceneNode("Cube")->setPosition(trans.getOrigin().x(),trans.getOrigin().y(),trans.getOrigin().z());

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
