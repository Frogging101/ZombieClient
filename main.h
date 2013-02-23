#ifndef MAIN_H
#define MAIN_H

#include <vector>

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <btBulletDynamicsCommon.h>

#include "GameEntity.h"

class ZombieClient : public Ogre::WindowEventListener, public Ogre::FrameListener{
	public:
		ZombieClient();
		virtual ~ZombieClient();
		bool go();

		Ogre::SceneManager *mSceneMgr;
		std::vector<GameEntity*> entities;
		std::vector<GameEntity*> physicsEntities;
	private:
		Ogre::Root *mRoot;
		Ogre::String mPluginsCfg;
		Ogre::String mResourcesCfg;
		Ogre::RenderWindow *mWindow;
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
		void loadLevel();
	protected:
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
		virtual void windowResized(Ogre::RenderWindow *rw);
		virtual void windowClosed(Ogre::RenderWindow *rw);
};

extern ZombieClient game;

#endif
