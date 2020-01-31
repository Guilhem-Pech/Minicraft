#ifndef __AVATAR__
#define __AVATAR__

#include "engine/utils/types_3d.h"
#include "engine/render/camera.h"
#include "engine/utils/timer.h"
#include "world.h"

class MAvatar
{
public:
	YVec3f Position;
	float mass = 1;
	YVec3f speed = { 0,0,0 };
	
	bool Move;
	bool Jump;
	float Height;
	float CurrentHeight;
	float Width;
	bool avance;
	bool recule;
	bool gauche;
	bool droite;
	bool Standing;
	bool InWater;
	bool Crouch;
	bool Run;

	YCamera * Cam;
	MWorld * World;

	YTimer _TimerStanding;
	YVbo * VboOpaque;
	YVec3f forces;

	static const int AVATAR_HEIGHT	= 2;
	static const int AVATAR_WIDTH	= 1;
	
	MAvatar(YCamera * cam, MWorld * world)
	{
		//Position = YVec3f((MWorld::MAT_SIZE_METERS) / 2, (MWorld::MAT_SIZE_METERS) / 2, (MWorld::MAT_HEIGHT_METERS));
		Position = { 0,0,64 };
		Height = 1.8f;
		CurrentHeight = Height;
		Width = 0.3f;
		Cam = cam;
		avance = false;
		recule = false;
		gauche = false;
		droite = false;
		Standing = false;
		Jump = false;
		World = world;
		InWater = false;
		Crouch = false;
		Run = false;
		speed = { 0,0,0 };
		forces = { 0,0,0 };
	}
									// A0		B1				C2			D3				E4			F5				G6			H7		
	vector<YVec3f> vertices = {{1,0,0},{1,1,0},{0,1,0},{0,1,2},{1,1,2},{1,0,2},{0,0,2}, {0,0,0} }; 

	void toVBO()
	{
		SAFEDELETE(VboOpaque);
		int nbVertexOpaque(0);
		
		VboOpaque = new YVbo(4, 36, YVbo::PACK_BY_ELEMENT_TYPE);
		VboOpaque->setElementDescription(0, YVbo::Element(3)); //Sommet
		VboOpaque->setElementDescription(1, YVbo::Element(3)); //Normale
		VboOpaque->setElementDescription(2, YVbo::Element(2)); //UV
		VboOpaque->setElementDescription(3, YVbo::Element(1)); //Type
		
		VboOpaque->createVboCpu();
		
		vector<YVec3f> &v = vertices;
		nbVertexOpaque += addQuadToVbo(VboOpaque, nbVertexOpaque, v[0], v[7], v[2], v[1], 10); // Down
		nbVertexOpaque += addQuadToVbo(VboOpaque, nbVertexOpaque, v[5], v[4], v[3], v[6], 10); // UP
		nbVertexOpaque += addQuadToVbo(VboOpaque, nbVertexOpaque, v[1], v[2], v[3], v[4], 10); // RIGHT
		nbVertexOpaque += addQuadToVbo(VboOpaque, nbVertexOpaque, v[0], v[5], v[6], v[7], 10); // LEFT
		nbVertexOpaque += addQuadToVbo(VboOpaque, nbVertexOpaque, v[0], v[1], v[4], v[5], 10); // FRONT
		nbVertexOpaque += addQuadToVbo(VboOpaque, nbVertexOpaque, v[7], v[6], v[3], v[2], 10); // BACK

		VboOpaque->createVboGpu();
		VboOpaque->deleteVboCpu();		
	}
	
	void render()
	{
		VboOpaque->render(); 
	}

	
	void update(float elapsed)
	{
		if (elapsed > 1.0f / 60.0f)
			elapsed = 1.0f / 60.0f;

		forces /= mass;
		YVec3f dSpeed(speed + forces);
		Position = Position + dSpeed * elapsed;
		speed = speed * 0.8f;
		forces = { 0,0, -9.81 };
	}
};

#endif