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
	GLuint ShaderSun;
	
	YVbo * VboCube;
	MWorld* World;

	GLuint ShaderCube;

	void init() 
	{
		YLog::log(YLog::ENGINE_INFO,"Minicraft Started : initialisation");

		Renderer->setBackgroundColor(YColor(0.0f,0.0f,0.0f,1.0f));
		Renderer->Camera->setPosition(YVec3f(100, 100, 100));
		Renderer->Camera->setLookAt(YVec3f());

		
			   			   		
		//Creation du VBO
		VboCube = CreateCube({{1,0,0} , {1,1,0}, {1,1,1},{1,0,1}, {0,0,1}, {0,1,1}, {0,1,0}, {0,0,0}});

		//On envoie le contenu au GPU
		VboCube->createVboGpu();

		//On relache la mémoire CPU
		VboCube->deleteVboCpu();

		

		//Pour créer le monde
		World = new MWorld();
		World->init_world(0);

				
		ShaderCubeDebug = Renderer->createProgram("shaders/cube_debug");
		ShaderSun = Renderer->createProgram("shaders/sun");
		ShaderCube = Renderer->createProgram("shaders/cube");
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
		object->setElementValue(0, startVertexNB, vertex[3].X, vertex[3].Y, vertex[3].Z); //Sommet (lié au layout(0) du shader)
		object->setElementValue(1, startVertexNB, normal.X , normal.Y, normal.Z);   //Normale (lié au layout(1) du shader)
		object->setElementValue(2, startVertexNB, uv[3][0], uv[3][1]);
		++startVertexNB;
		object->setElementValue(0, startVertexNB, vertex[2].X, vertex[2].Y, vertex[2].Z); //Sommet (lié au layout(0) du shader)
		object->setElementValue(1, startVertexNB, normal.X, normal.Y, normal.Z);//Normale (lié au layout(1) du shader)
		object->setElementValue(2, startVertexNB, uv[2][0], uv[1][1]);
		++startVertexNB;
		object->setElementValue(0, startVertexNB, vertex[1].X, vertex[1].Y, vertex[1].Z);
		object->setElementValue(1, startVertexNB, normal.X, normal.Y, normal.Z); //Normale (lié au layout(1) du shader)
		object->setElementValue(2, startVertexNB, uv[1][0], uv[1][1]);
		++startVertexNB;
		object->setElementValue(0, startVertexNB, vertex[1].X, vertex[1].Y, vertex[1].Z);
		object->setElementValue(1, startVertexNB, normal.X, normal.Y, normal.Z); //Normale (lié au layout(1) du shader)
		object->setElementValue(2, startVertexNB, uv[1][0], uv[1][1]);
		++startVertexNB;
		object->setElementValue(0, startVertexNB, vertex[0].X, vertex[0].Y, vertex[0].Z);
		object->setElementValue(1, startVertexNB, normal.X, normal.Y, normal.Z);   //Normale (lié au layout(1) du shader)
		object->setElementValue(2, startVertexNB, uv[0][0], uv[0][1]);
		++startVertexNB;
		object->setElementValue(0, startVertexNB, vertex[3].X, vertex[3].Y, vertex[3].Z);; //Sommet (lié au layout(0) du shader)
		object->setElementValue(1, startVertexNB, normal.X, normal.Y, normal.Z);  //Normale (lié au layout(1) du shader)
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
		processMovments();
	}


	float moveSpeed = 0.8f;
	void processMovments() {
		if (forwardDown)
			Renderer->Camera->move({ moveSpeed,0,0 });

		if (backwardDown)
			Renderer->Camera->move({ -moveSpeed,0,0 });

		if (leftDown)
			Renderer->Camera->move({ 0,moveSpeed,0 });

		if (rightDown)
			Renderer->Camera->move({ 0,-moveSpeed,0 });
	}

	YVec3<float> SunDirection;

	YVec3<float> SunPosition;

	YColor SunColor;

	YColor SkyColor;
	
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

		//glRotatef(this->DeltaTimeCumul / 10.0f * 360, -1, 1, 0);
		updateLights(boostDown);
		glPushMatrix();
		glUseProgram(ShaderSun);
		GLuint var = glGetUniformLocation(ShaderSun, "sun_color");
		glUniform3f(var, SunColor.R, SunColor.V, SunColor.B);
		glTranslatef(SunPosition.X, SunPosition.Y, SunPosition.Z);
		//glScalef(10, 10, 10);
		Renderer->updateMatricesFromOgl();
		Renderer->sendMatricesToShader(ShaderSun);
		VboCube->render();
		glPopMatrix();


		//Pour dessiner le monde (fonction à coder ensuite)
		glPushMatrix();
		glUseProgram(ShaderCube);
		Renderer->updateMatricesFromOgl();
		Renderer->sendMatricesToShader(ShaderCube);
		World->render_world_basic(ShaderCube,VboCube);
		//World->render_world_vbo(true,true);
		glPopMatrix();
		
	}

	bool getSunDirFromDayTime(YVec3f & sunDir, float mnLever, float mnCoucher, float boostTime)
	{
		bool nuit = false;

		SYSTEMTIME t;
		GetLocalTime(&t);

		//On borne le tweak time à une journée (cyclique)
		while (boostTime > 24 * 60)
			boostTime -= 24 * 60;

		//Temps écoulé depuis le début de la journée
		float fTime = (float)(t.wHour * 60 + t.wMinute);
		fTime += boostTime;
		while (fTime > 24 * 60)
			fTime -= 24 * 60;

		//Si c'est la nuit
		if (fTime < mnLever || fTime > mnCoucher)
		{
			nuit = true;
			if (fTime < mnLever)
				fTime += 24 * 60;
			fTime -= mnCoucher;
			fTime /= (mnLever + 24 * 60 - mnCoucher);
			fTime *= (float)M_PI;
		}
		else
		{
			//c'est le jour
			nuit = false;
			fTime -= mnLever;
			fTime /= (mnCoucher - mnLever);
			fTime *= (float)M_PI;
		}

		//Direction du soleil en fonction de l'heure
		sunDir.X = cos(fTime);
		sunDir.Y = 0.2f;
		sunDir.Z = sin(fTime);
		sunDir.normalize();

		return nuit;
	}


	

	void updateLights(float boostTime = 0)
	{
		//On recup la direction du soleil
		bool nuit = getSunDirFromDayTime(SunDirection, 6.0f * 60.0f, 19.0f * 60.0f, boostTime);
		SunPosition = SunDirection * 2; //Renderer->Camera->Position + SunDirection * 500.0f;

		//Pendant la journée
		if (!nuit)
		{
			//On definit la couleur
			SunColor = YColor(1.0f, 1.0f, 0.8f, 1.0f);
			SkyColor = YColor(0.0f, 181.f / 255.f, 221.f / 255.f, 1.0f);
			YColor downColor(0.9f, 0.5f, 0.1f, 1);

			SunColor = SunColor.interpolate(downColor, (abs(SunDirection.X)));
			SkyColor = SkyColor.interpolate(downColor, (abs(SunDirection.X)));
		}
		else
		{
			//La nuit : lune blanche et ciel noir
			SunColor = YColor(1, 1, 1, 1);
			SkyColor = YColor(0, 0, 0, 1);
		}

		Renderer->setBackgroundColor(SkyColor);
	}


	
	void resize(int width, int height) {
	
	}

	/*INPUTS*/
	float boostDown = 1.f;
	bool  ctrlDown = false;
	

	bool forwardDown = false;
	bool backwardDown = false;
	bool leftDown = false;
	bool rightDown = false;


	struct KeyMap {
		int forward = 'z';
		int backward = 's';
		int left = 'q';
		int right = 'd';
	};

	KeyMap frKeyboard = { 'w','s','a','d' }; // Forward, Backward, Left, Right

	void keyPressed(int key, bool special, bool down, int p1, int p2) 
	{	
		if (key == 'g' && down)
			boostDown += 30.f;

		if (key == frKeyboard.forward)
			forwardDown = down;
		
		if (key == frKeyboard.backward)
			backwardDown = down;

		if (key == frKeyboard.left)
			leftDown = down;

		if (key == frKeyboard.right)
			rightDown = down; 

		if (key == GLUT_KEY_CTRL_L)
			ctrlDown = down;
	}

	void mouseWheel(int wheel, int dir, int x, int y, bool inUi)
	{
		if(dir < 0)
		{
			Renderer->Camera->move({ -1.f,0,0 });
		} else
		{
			Renderer->Camera->move({ 1.f,0,0 });
		}
		
	}

	bool rightClickDown = false;
	bool middleClickDown = false;
	void mouseClick(int button, int state, int x, int y, bool inUi)
	{
		switch (button) {
			case GLUT_LEFT_BUTTON:
				break;
			case GLUT_RIGHT_BUTTON:
				rightClickDown = state == GLUT_DOWN;
				break;
			case GLUT_MIDDLE_BUTTON :
				middleClickDown = state == GLUT_DOWN;
				break;
		}		
	}

	float prevMouseX;

	float prevMouseY;


	const float Pi = M_PI;

	float degToRad(const float x) const
	{
		return x / 180.f * Pi;
	}

	float radToDeg(const float x) const
	{
		return x / Pi * 180.f;
	}


	float smooth = 0.3f;
	
	void mouseMove(int x, int y, bool pressed, bool inUi)
	{
		if(rightClickDown)
		{
			float xDelta = -degToRad(x - prevMouseX);
			float yDelta =	degToRad(y - prevMouseY);
			if(ctrlDown)
			{
				Renderer->Camera->rotateUpAround(yDelta);
				Renderer->Camera->rotateAround(xDelta);
			}
			else
			{
				Renderer->Camera->rotateUp( yDelta);
				Renderer->Camera->rotate(xDelta);
			}			
		}
		else if (middleClickDown)
		{
			if(ctrlDown)
			{
				Renderer->Camera->moveByWorld({ 0,(x - prevMouseX)*smooth, (y - prevMouseY)*smooth });
			} else
			{
				Renderer->Camera->move({ 0,(x - prevMouseX)*smooth, (y-prevMouseY)*smooth });
			}
		}

		
		prevMouseX = x;
		prevMouseY = y;
	}
	
};


#endif