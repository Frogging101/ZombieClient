#include "GameEntity.h"
#include "main.h"

GameEntity::GameEntity(Ogre::Vector3 pos, Ogre::Vector3 rot, std::string name, int physicsType, int entityType){
	this->entityType = entityType;
	this->physicsType = physicsType;
	
	switch (entityType){
	case ETYPE_MESH:
		this->ogreEntity = game.mSceneMgr->createEntity(name,name+".mesh");

		this->ogreSceneNode = game.mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
		this->ogreSceneNode->attachObject(this->ogreEntity);
		this->ogreSceneNode->setPosition(pos);
		this->ogreSceneNode->setOrientation(this->ogreSceneNode->getInitialOrientation());
		this->ogreSceneNode->yaw(Ogre::Degree(rot.z));
		this->ogreSceneNode->pitch(Ogre::Degree(rot.x));
		break;
	case ETYPE_LAMP:
		this->ogreLight = game.mSceneMgr->createLight(name);
		this->ogreLight->setPosition(pos);
		this->ogreLight->setDirection(rot);
		break;
	}
}