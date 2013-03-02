#include "GameEntity.h"
#include "main.h"

/*GameEntity::GameEntity(Ogre::Vector3 pos, Ogre::Vector3 rot, std::string name, int physicsType, int entityType,std::string mesh){
	std::cout << "pos" << pos << " rot" << rot << std::endl;
	this->entityType = entityType;
	this->physicsType = physicsType;
	
	switch (entityType){
	case ETYPE_MESH:
		this->ogreEntity = game.mSceneMgr->createEntity(name,mesh);

		this->ogreSceneNode = game.mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
		this->ogreSceneNode->attachObject(this->ogreEntity);
		this->ogreSceneNode->setPosition(pos);
		this->ogreSceneNode->setOrientation(this->ogreSceneNode->getInitialOrientation());
		this->ogreSceneNode->yaw(Ogre::Degree(rot.z));
		this->ogreSceneNode->pitch(Ogre::Degree(rot.x));
		break;
	case ETYPE_LAMP:
		*this = GameLight(pos,rot,name);
		break;
	}
}*/

GameObject::GameObject(Ogre::Vector3 pos, Ogre::Vector3 rot, std::string name, int physicsType, int physicsShape, int entityType,std::string mesh){
	Ogre::Vector3 size;
	
	this->entityType = ETYPE_MESH;
	this->pos = pos;
	this->rot = rot;

	this->ogreEntity = game.mSceneMgr->createEntity(name,mesh);
	this->ogreSceneNode = game.mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
	this->ogreSceneNode->attachObject(this->ogreEntity);
	this->ogreSceneNode->setPosition(pos);
	this->ogreSceneNode->setOrientation(this->ogreSceneNode->getInitialOrientation());
	this->ogreSceneNode->yaw(Ogre::Degree(rot.z));
	this->ogreSceneNode->pitch(Ogre::Degree(rot.x));

	size = this->ogreEntity->getBoundingBox().getSize();

	switch(physicsShape){
	case PSHAPE_CAPSULE:
		this->colShape = new btCapsuleShape(size.x/2,size.y);
		break;
	case PSHAPE_BOX:
		this->colShape = new btBoxShape(btVector3(size.x/2,size.y/2,size.z/2));
		break;
	case PSHAPE_SPHERE:
		this->colShape = new btSphereShape(size.x/2);
		break;
	case PSHAPE_CYLINDER:
		this->colShape = new btCylinderShape(btVector3(size.x/2,size.y/2,size.z/2));
		break;
	case PSHAPE_CONE:
		this->colShape = new btConeShape(size.x/2,size.y);
		break;
	case PSHAPE_CONVEX_HULL:
		//Not implemented yet; requires research and probably an algorithm to get the vertices
		this->colShape = new btBoxShape(btVector3(size.x/2,size.y/2,size.z/2)); 
		//Still, we wouldn't want to crash, now would we?
		break;
	case PSHAPE_MESH:
		this->colShape = new btBvhTriangleMeshShape(getTriMesh(this->ogreEntity),true);
		break;
	}
}

GameLight::GameLight(Ogre::Vector3 pos, Ogre::Vector3 rot, std::string name, Ogre::Light::LightTypes lightType){
	this->entityType = ETYPE_LAMP;
	this->pos = pos;
	this->rot = rot;

	this->ogreLight = game.mSceneMgr->createLight(name);
	this->ogreLight->setType(lightType);
	this->ogreLight->setPosition(pos);
	this->ogreLight->setDirection(rot);
}