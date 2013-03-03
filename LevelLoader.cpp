#include <rapidxml.hpp>
#include <fstream>
#include <sstream>

#include "main.h"
#include "GameEntity.h"

using namespace rapidxml;
using namespace std;

void ZombieClient::loadLevel(){
	string buffer;
	string buffer2;
	stringstream ss;
	
	string entityTypeStr,physicsShapeStr,lightTypeStr;

	Ogre::Vector3 pos,rot;
	string name,mesh;
	int entityType,physicsShape;
	Ogre::Light::LightTypes lightType;
	float mass;

	ifstream file("level.map",fstream::in);

	if(file.fail()){
		cout << "File couldn't be opened.";
		return;
	}
	
	while(!file.eof()){
		getline(file,buffer);
		buffer2.append(buffer+"\n");
	}
	char *fileStr = new char[100000];
	strcpy(fileStr,buffer2.c_str());

	xml_document<> doc;
	doc.parse<0>(fileStr);
	int entityCount = 0;
	xml_node<> *XMLSceneNode = doc.first_node();
	xml_node<> *XMLEntityNode = XMLSceneNode->first_node("Entity");
	entityCount++;
	int i=0;
	while(XMLEntityNode != 0){
		for(xml_attribute<> *attr = XMLEntityNode->first_attribute();
				attr; attr = attr->next_attribute()){

			ss.str("");
			ss.clear();
			std::cout << "Got " << attr->name() << " with value: " << attr->value() << std::endl;
			std::string atrName = attr->name();
			if(atrName == "name"){
				name = attr->value();
			}
			else if(atrName == "pos"){
				buffer2 = attr->value();
				ss << buffer2;
				ss >> pos.x;

				ss << buffer2;
				ss >> pos.y;

				ss << buffer2;
				ss >> pos.z;
			}
			else if(atrName == "rot"){
				buffer2 = attr->value();
				ss << buffer2;
				ss >> rot.x;

				ss << buffer2;
				ss >> rot.y;

				ss << buffer2;
				ss >> rot.z;
			}
			else if(atrName == "type"){
				entityTypeStr = attr->value();
				if(entityTypeStr == "MESH")
					entityType = ETYPE_MESH;
				else if(entityTypeStr == "LAMP")
					entityType = ETYPE_LAMP;
				else
					continue;
			}
			else if(atrName == "mesh"){
				mesh = attr->value();
				mesh += ".mesh";
			}
			else if(atrName == "pMass"){
				buffer2 = attr->value();
				ss << buffer2;
				ss >> mass;
			}
			else if(atrName == "pShape"){
				physicsShapeStr = attr->value();
				if(physicsShapeStr == "CAPSULE")
					physicsShape = PSHAPE_CAPSULE;
				else if(physicsShapeStr == "BOX")
					physicsShape = PSHAPE_BOX;
				else if(physicsShapeStr == "SPHERE")
					physicsShape = PSHAPE_SPHERE;
				else if(physicsShapeStr == "CYLINDER")
					physicsShape = PSHAPE_CYLINDER;
				else if(physicsShapeStr == "CONE")
					physicsShape = PSHAPE_CONE;
				else if(physicsShapeStr == "CONVEX_HULL")
					physicsShape = PSHAPE_CONVEX_HULL;
				else if(physicsShapeStr == "MESH")
					physicsShape = PSHAPE_MESH;
			}
			else if(atrName == "lType"){
				lightTypeStr = attr->value();
				if(lightTypeStr == "POINT")
					lightType = Ogre::Light::LightTypes::LT_POINT;
				else if(lightTypeStr == "SPOT")
					lightType = Ogre::Light::LightTypes::LT_SPOTLIGHT;
			}
		}
		std::cout << "PUTTING A MESH DOWN!" << std::endl;
		switch(entityType){
		case ETYPE_MESH:
			entities.push_back(new GameObject(pos,rot,name,physicsShape,mass,entityType,mesh));
			break;
		case ETYPE_LAMP:
			entities.push_back(new GameLight(pos,rot,name,lightType));
			break;
		}
		XMLEntityNode = XMLEntityNode->next_sibling();
	}
	GameEntity *ent = entities[0];
}
