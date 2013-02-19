#ifndef MAIN_H
#define MAIN_H

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <btBulletDynamicsCommon.h>

class ZombieClient : public Ogre::WindowEventListener, public Ogre::FrameListener{
	public:
		ZombieClient();
		virtual ~ZombieClient();
		bool go();
	private:
		Ogre::Root *mRoot;
		Ogre::String mPluginsCfg;
		Ogre::String mResourcesCfg;
		Ogre::RenderWindow *mWindow;
		Ogre::SceneManager *mSceneMgr;
		Ogre::Camera *mCamera;
		OIS::InputManager *mInputManager;
		OIS::Mouse *mMouse;
		OIS::Keyboard *mKeyboard;

		btBroadphaseInterface* pBroadphase;
		btCollisionDispatcher* pDispatcher;
		btDefaultCollisionConfiguration* pCollisionConfiguration;
		btSequentialImpulseConstraintSolver* pSolver;
		btDiscreteDynamicsWorld* pDynamicsWorld;
		btRigidBody *boxBody;

		void createScene();
	protected:
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
		virtual void windowResized(Ogre::RenderWindow *rw);
		virtual void windowClosed(Ogre::RenderWindow *rw);
};

#endif
