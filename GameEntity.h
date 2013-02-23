#ifndef GAMEENTITY_H
#define GAMEENTITY_H

#include <OgreEntity.h>

#include <btBulletDynamicsCommon.h>

enum physicsTypes{
	PTYPE_NONE,
	PTYPE_RIGIDBODY,
	PTYPE_STATIC
};

enum entityTypes{
	ETYPE_NONE,
	ETYPE_MESH,
	ETYPE_LAMP
};

class GameEntity{
public:
	GameEntity(Ogre::Vector3 pos, Ogre::Vector3 rot, std::string name, int physicsType, int entityType,std::string mesh);

	Ogre::Entity *ogreEntity;
	Ogre::Light *ogreLight;
	Ogre::SceneNode *ogreSceneNode;
	btCollisionShape *colShape;
	int entityType;
	int physicsType;
};

#endif
