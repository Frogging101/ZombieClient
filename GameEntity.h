#ifndef GAMEENTITY_H
#define GAMEENTITY_H

#include <OgreEntity.h>
#include <OgreLight.h>

#include <btBulletDynamicsCommon.h>

#include "Physics.h"

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

enum physicsShapes{
	PSHAPE_CAPSULE,
	PSHAPE_BOX,
	PSHAPE_SPHERE,
	PSHAPE_CYLINDER,
	PSHAPE_CONE,
	PSHAPE_CONVEX_HULL,
	PSHAPE_MESH
};

class GameEntity{
public:
	//GameEntity(Ogre::Vector3 pos, Ogre::Vector3 rot, std::string name, int physicsType, int physicsShape, int entityType,std::string mesh);

	Ogre::Vector3 pos;
	Ogre::Vector3 rot;
	std::string name;

	int entityType;
};

class GameLight: public GameEntity{
public:
	GameLight(Ogre::Vector3 pos, Ogre::Vector3 rot, std::string name, Ogre::Light::LightTypes lightType);
	Ogre::Light *ogreLight;
};

class GameObject: public GameEntity{
public:
	GameObject(Ogre::Vector3 pos, Ogre::Vector3 rot, std::string name, int physicsType, int physicsShape, int entityType, std::string mesh);

	Ogre::SceneNode *ogreSceneNode;
	btCollisionShape *colShape;
	Ogre::Entity *ogreEntity;
	int physicsType;
};

#endif
