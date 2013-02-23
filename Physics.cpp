	#include <OgreSubMesh.h>
	#include <OgreEntity.h>
	
	#include "Physics.h"

btTriangleMesh *getTriMesh(Ogre::Entity *entity){
	//We need a pointer to the hardware vertex buffer.
	//In order to do this, we have to
	//get the mesh from the entity, vertex data from the mesh, vertex declaration from the data,
	//vertex element from the declaration, hardware vertex buffer pointer from the element.
	
	Ogre::MeshPtr meshPtr = entity->getMesh();
	Ogre::VertexData *vertexData = meshPtr->sharedVertexData;
	//We need to GO DEEPER
	const Ogre::VertexDeclaration* vd = vertexData->vertexDeclaration;
	const Ogre::VertexElement *posElem = vd->findElementBySemantic(Ogre::VES_POSITION);
	Ogre::HardwareVertexBufferSharedPtr vbuf = vertexData->vertexBufferBinding->getBuffer(posElem->getSource());
	//We now have a hardware vertex buffer pointer

	Ogre::HardwareIndexBufferSharedPtr ibuf; //A pointer to the hardware index buffer
	uint16_t currentIndex; //This is used in the loop below to store data temporarily
	uint16_t indices[2048]; //Array of vertex indices
	int trianglesmax = 0; //The number of triangles

	//Loop through submeshes, getting indices
	for(int i=0,j=0;i<meshPtr->getNumSubMeshes();i++){
		int offset = 0;
		ibuf = meshPtr->getSubMesh(i)->indexData->indexBuffer; //Get the index buffer for the current submesh

		//Loop through the index buffer of the current submesh
		for(int k=0;k<ibuf->getNumIndexes();k++){
			ibuf->readData(offset,ibuf->getIndexSize(),&currentIndex);
			indices[j] = currentIndex; //Take the current index and put it into the index array
			offset += ibuf->getIndexSize(); //Increase the offset by 1 index
			trianglesmax++;
			j++;
		}
	}

	Ogre::Vector3 *triangles[2048][3]; //Array of triangles, each triangle has 3 vertices
	int triangleCount = 0;
	float currentVertex[3]; //Used in the loop below to temporary store the current vertex position

	for(int i=0,j=0,offset=0;i<trianglesmax && j<trianglesmax;i++){
		offset = indices[j]*vbuf->getVertexSize();
		j++;

		vbuf->readData(offset,vbuf->getVertexSize(),&currentVertex); //Read one vertex
		triangles[i][0] = new Ogre::Vector3(currentVertex[0],currentVertex[1],currentVertex[2]); //Store it
		offset = indices[j]*vbuf->getVertexSize(); //Set the offset to the current index's vertex
		j++; //Increment the... hmm... "index index"

		vbuf->readData(offset,vbuf->getVertexSize(),&currentVertex);
		triangles[i][1] = new Ogre::Vector3(currentVertex[0],currentVertex[1],currentVertex[2]);
		offset = indices[j]*vbuf->getVertexSize();
		j++;

		vbuf->readData(offset,vbuf->getVertexSize(),&currentVertex);
		triangles[i][2] = new Ogre::Vector3(currentVertex[0],currentVertex[1],currentVertex[2]);
		triangleCount++;

		//At the end of this, 3 vertices have been stored in one slot of the triangle array
	}

	btTriangleMesh *trimesh = new btTriangleMesh();

	//3 vertices that make up a triangle
	btVector3 *vertex1;
	btVector3 *vertex2;
	btVector3 *vertex3;

	for(int i=0;i<triangleCount;i++){
		//Set all 3 vertex values to the values from the triangle array, one axis at a time
		vertex1 = new btVector3(triangles[i][0]->x,triangles[i][0]->y,triangles[i][0]->z);
		vertex2 = new btVector3(triangles[i][1]->x,triangles[i][1]->y,triangles[i][1]->z);
		vertex3 = new btVector3(triangles[i][2]->x,triangles[i][2]->y,triangles[i][2]->z);

		trimesh->addTriangle(*vertex1,*vertex2,*vertex3); //Create a triangle from these vertices, add it to the trimesh
		
		//Don't waste memory
		delete vertex1;
		delete vertex2;
		delete vertex3;
	}
	return trimesh;
}