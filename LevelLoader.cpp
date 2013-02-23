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
	
	string entityTypeStr;
	string physicsTypeStr;

	Ogre::Vector3 pos;
	Ogre::Vector3 rot;
	string name;
	string mesh;
	int entityType;
	int physicsType = 0;

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
	//while((XMLEntityNode = XMLEntityNode->next_sibling()) != 0){
	while(XMLEntityNode != 0){
		mesh = "Cube.mesh";
		for(xml_attribute<> *attr = XMLEntityNode->first_attribute();
				attr; attr = attr->next_attribute()){

			ss.str("");
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
			}
		}
		std::cout << "PUTTING A MESH DOWN!" << std::endl;
		entities.push_back(new GameEntity(pos,rot,name,physicsType,entityType,mesh));
		XMLEntityNode = XMLEntityNode->next_sibling();
	}
	GameEntity *ent = entities[0];
}
