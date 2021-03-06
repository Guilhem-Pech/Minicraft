#ifndef __YOCTO__ENGINE_TEST__
#define __YOCTO__ENGINE_TEST__

#include "engine/engine.h"

#include "avatar.h"
#include "world.h"

class MEngineMinicraft : public YEngine {

public :
	//Gestion singleton
	static YEngine * getInstance()
	{
		if (Instance == NULL)
			Instance = new MEngineMinicraft();
		return Instance;
	}

	/*HANDLERS GENERAUX*/
	void loadShaders() {
		
	}
	GLuint ShaderCubeDebug;
	YVbo * VboCube;
	void init() 
	{
		YLog::log(YLog::ENGINE_INFO,"Minicraft Started : initialisation");

		Renderer->setBackgroundColor(YColor(0.0f,0.0f,0.0f,1.0f));
		Renderer->Camera->setPosition(YVec3f(10, 10, 10));
		Renderer->Camera->setLookAt(YVec3f());





		
		//Creation du VBO
		VboCube = CreateCube({{1,0,0} , {1,1,0}, {1,1,1},{1,0,1}, {0,0,1}, {0,1,1}, {0,1,0}, {0,0,0}});

		//On envoie le contenu au GPU
		VboCube->createVboGpu();

		//On relache la m�moire CPU
		VboCube->deleteVboCpu();


		ShaderCubeDebug = Renderer->createProgram("shaders/cube_debug");

		//Exemple d'utilisation d'un shader
		
	}


	/**
	 * \brief 
	 * \param object with the 0 element representing the vertex 
	 * \param vertex Vector with the 4 vertices of the face;  
	 * \param normal the normal of the face
	 */
	void AddFace( YVbo * object  ,YVec3<int> vertex[4], YVec3<int> normal, vector<float> uv[4] ,int startVertexNB = 0)
	{
		object->setElementValue(0, startVertexNB, vertex[3].X, vertex[3].Y, vertex[3].Z); //Sommet (li� au layout(0) du shader)
		object->setElementValue(1, startVertexNB, normal.X , normal.Y, normal.Z);   //Normale (li� au layout(1) du shader)
		object->setElementValue(2, startVertexNB, uv[3][0], uv[3][1]);
		++startVertexNB;
		object->setElementValue(0, startVertexNB, vertex[2].X, vertex[2].Y, vertex[2].Z); //Sommet (li� au layout(0) du shader)
		object->setElementValue(1, startVertexNB, normal.X, normal.Y, normal.Z);//Normale (li� au layout(1) du shader)
		object->setElementValue(2, startVertexNB, uv[2][0], uv[1][1]);
		++startVertexNB;
		object->setElementValue(0, startVertexNB, vertex[1].X, vertex[1].Y, vertex[1].Z);
		object->setElementValue(1, startVertexNB, normal.X, normal.Y, normal.Z); //Normale (li� au layout(1) du shader)
		object->setElementValue(2, startVertexNB, uv[1][0], uv[1][1]);
		++startVertexNB;
		object->setElementValue(0, startVertexNB, vertex[1].X, vertex[1].Y, vertex[1].Z);
		object->setElementValue(1, startVertexNB, normal.X, normal.Y, normal.Z); //Normale (li� au layout(1) du shader)
		object->setElementValue(2, startVertexNB, uv[1][0], uv[1][1]);
		++startVertexNB;
		object->setElementValue(0, startVertexNB, vertex[0].X, vertex[0].Y, vertex[0].Z);
		object->setElementValue(1, startVertexNB, normal.X, normal.Y, normal.Z);   //Normale (li� au layout(1) du shader)
		object->setElementValue(2, startVertexNB, uv[0][0], uv[0][1]);
		++startVertexNB;
		object->setElementValue(0, startVertexNB, vertex[3].X, vertex[3].Y, vertex[3].Z);; //Sommet (li� au layout(0) du shader)
		object->setElementValue(1, startVertexNB, normal.X, normal.Y, normal.Z);  //Normale (li� au layout(1) du shader)
		object->setElementValue(2, startVertexNB, uv[3][0], uv[3][1]);

	}

	YVbo * CreateCube(vector<YVec3<int>> vertex)
	{
		YVbo * vboCube = new YVbo(3, 36, YVbo::PACK_BY_ELEMENT_TYPE);
		vboCube->setElementDescription(0, YVbo::Element(3)); //Sommet
		vboCube->setElementDescription(1, YVbo::Element(3)); //Normale
		vboCube->setElementDescription(2, YVbo::Element(2)); //UV
		vboCube->createVboCpu();

		
		YVec3<int> up[4] = { vertex[3], vertex[4], vertex[5], vertex[2] }; // TODO : Clean it, make it in a loop or something
		YVec3<int> down[4] = { vertex[0], vertex[1], vertex[6], vertex[7] };
		YVec3<int> right[4] = { vertex[1], vertex[2], vertex[5], vertex[6] };
		YVec3<int> left[4] = { vertex[7], vertex[4], vertex[3], vertex[0] };
		YVec3<int> front[4] = { vertex[0], vertex[3], vertex[2], vertex[1] };
		YVec3<int> back[4] = { vertex[7], vertex[6], vertex[5], vertex[4] };

		vector<float> uv[4] = { {0,0},{0,0},{0,0},{0,0} }; // TODO Add support for UVs 
		
		AddFace(vboCube, up, { 0,0,1 }, uv, 0);
		AddFace(vboCube, down, { 0,0,1 }, uv, 6);
		AddFace(vboCube, right, { 0,0,1 }, uv, 12);
		AddFace(vboCube, left, { 0,0,1 }, uv, 18);
		AddFace(vboCube, front, { 0,0,1 }, uv, 24);
		AddFace(vboCube, back, { 0,0,1 }, uv, 30);
		
		
		
		return vboCube;
	}
	
	
	void update(float elapsed) 
	{
		
	}

	void renderObjects() 
	{
		
		glUseProgram(0);
		//Rendu des axes
		glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
		glColor3d(1, 0, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(10000, 0, 0);
		glColor3d(0, 1, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 10000, 0);
		glColor3d(0, 0, 1);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, 10000);
		glEnd();		

		glRotatef(this->DeltaTimeCumul / 10.0f * 360, -1, 1, 0);
		glUseProgram(ShaderCubeDebug); //Demande au GPU de charger ces shaders
		Renderer->updateMatricesFromOgl(); //Calcule toute les matrices � partir des deux matrices OGL
		Renderer->sendMatricesToShader(ShaderCubeDebug); //Envoie les matrices au shader
		//glDisable(GL_CULL_FACE);
		//glDisable(GL_DEPTH_TEST);
		
		VboCube->render(); //Demande le rendu du VBO


	}

	void resize(int width, int height) {
	
	}

	/*INPUTS*/

	void keyPressed(int key, bool special, bool down, int p1, int p2) 
	{	

	}

	void mouseWheel(int wheel, int dir, int x, int y, bool inUi)
	{
		
	}

	void mouseClick(int button, int state, int x, int y, bool inUi)
	{
		
	}

	void mouseMove(int x, int y, bool pressed, bool inUi)
	{
		
	}
	
};


#endif