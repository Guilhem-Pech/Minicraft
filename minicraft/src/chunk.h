#pragma once

#include "engine/render/renderer.h"
#include "engine/render/vbo.h"
#include "cube.h"

/**
  * On utilise des chunks pour que si on modifie juste un cube, on ait pas
  * besoin de recharger toute la carte dans le buffer, mais juste le chunk en question
  */
class MChunk
{
	public :

		static const int CHUNK_SIZE = 64; ///< Taille d'un chunk en nombre de cubes (n*n*n)
		MCube _Cubes[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE]; ///< Cubes contenus dans le chunk

		YVbo * VboOpaque = NULL;
		YVbo * VboTransparent = NULL;

		MChunk * Voisins[6];

		int _XPos, _YPos, _ZPos; ///< Position du chunk dans le monde

		MChunk(int x, int y, int z)
		{
			memset(Voisins, 0x00, sizeof(void*)* 6);
			_XPos = x;
			_YPos = y;
			_ZPos = z;
		}

		/*
		Creation des VBO
		*/

		//On met le chunk ddans son VBO
		void toVbos(void)
		{
			SAFEDELETE(VboOpaque);
			SAFEDELETE(VboTransparent);

			//Compter les sommets
			int nbVertexOpaque (0);
			int nbVertexTransparent (0);
			foreachVisibleTriangle(true, &nbVertexOpaque, &nbVertexTransparent, VboOpaque, VboTransparent);

			// Définir les VBOs
			VboOpaque = new YVbo(4, nbVertexOpaque, YVbo::PACK_BY_ELEMENT_TYPE);
			VboOpaque->setElementDescription(0, YVbo::Element(3)); //Sommet
			VboOpaque->setElementDescription(1, YVbo::Element(3)); //Normale
			VboOpaque->setElementDescription(2, YVbo::Element(2)); //UV
			VboOpaque->setElementDescription(3, YVbo::Element(1)); //Type
			

			VboTransparent = new YVbo(4, nbVertexTransparent, YVbo::PACK_BY_ELEMENT_TYPE);
			VboTransparent->setElementDescription(0, YVbo::Element(3)); //Sommet
			VboTransparent->setElementDescription(1, YVbo::Element(3)); //Normale
			VboTransparent->setElementDescription(2, YVbo::Element(2)); //UV
			VboTransparent->setElementDescription(3, YVbo::Element(1)); //Type
			
					
			//Créer les VBO
			VboTransparent->createVboCpu();
			VboOpaque->createVboCpu();

			//Remplir les VBO
			foreachVisibleTriangle(false, &nbVertexOpaque, &nbVertexTransparent, VboOpaque, VboTransparent);

			VboOpaque->createVboGpu();
			VboOpaque->deleteVboCpu();
			
			VboTransparent->createVboGpu();
			VboTransparent->deleteVboCpu();
		}
		const vector<float> uv[4] = { {0,0},{0,0},{0,0},{0,0} }; // TODO Add support for UVs
	//Ajoute un quad du cube. Attention CCW
		int addQuadToVbo(YVbo * vbo, int iVertice, YVec3f & a, YVec3f & b, YVec3f & c, YVec3f & d, float type) {

			
			YVec3 normal = (b - a).cross(d - a).normalize();
			vbo->setElementValue(0, iVertice, a.X, a.Y, a.Z); //Sommet (lié au layout(0) du shader)
			vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);   //Normale (lié au layout(1) du shader)
			vbo->setElementValue(2, iVertice, 0, 0);
			vbo->setElementValue(3, iVertice, type);
			++iVertice;
			vbo->setElementValue(0, iVertice, b.X, b.Y, b.Z); //Sommet (lié au layout(0) du shader)
			vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);//Normale (lié au layout(1) du shader)
			vbo->setElementValue(2, iVertice, 0, 0);
			vbo->setElementValue(3, iVertice, type);
			++iVertice;
			vbo->setElementValue(0, iVertice, c.X, c.Y, c.Z);
			vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z); //Normale (lié au layout(1) du shader)
			vbo->setElementValue(2, iVertice, 0, 0);
			vbo->setElementValue(3, iVertice, type);
			++iVertice;
			vbo->setElementValue(0, iVertice, c.X, c.Y, c.Z);
			vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z); //Normale (lié au layout(1) du shader)
			vbo->setElementValue(2, iVertice, 0, 0);
			vbo->setElementValue(3, iVertice, type);
			++iVertice;
			vbo->setElementValue(0, iVertice, d.X, d.Y, d.Z);
			vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);   //Normale (lié au layout(1) du shader)
			vbo->setElementValue(2, iVertice, 0, 0);
			vbo->setElementValue(3, iVertice, type);
			++iVertice;
			vbo->setElementValue(0, iVertice, a.X, a.Y, a.Z);; //Sommet (lié au layout(0) du shader)
			vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);  //Normale (lié au layout(1) du shader)
			vbo->setElementValue(2, iVertice, 0, 0);
			vbo->setElementValue(3, iVertice, type);
			
			return 6;
		}

		//Permet de compter les triangles ou des les ajouter aux VBO
		void foreachVisibleTriangle(bool countOnly, int * nbVertOpaque, int * nbVertTransp, YVbo * VboOpaque, YVbo * VboTrasparent) {
			int curOpaqueVertices(0);
			int curTransVertices(0);
			
			for (int x = 0; x < CHUNK_SIZE; ++x)
				for (int y = 0; y < CHUNK_SIZE; ++y)
					for (int z = 0; z < CHUNK_SIZE; ++z)
					{
						MCube* cube = &_Cubes[x][y][z]; 
						if(cube->getDraw())

							if(cube->isOpaque())
							{						
								
								if(countOnly)
								{
									*nbVertOpaque += 36;
								} else
								{
									const float xf = float(x);
									const float yf = float(y);
									const float zf = float(z);
									
									YVec3f v[] = { {1.f + xf, yf, zf} , {xf + 1.f,yf + 1.f,zf}, {xf + 1.f,yf + 1,zf + 1},{xf + 1,yf,zf + 1}, {xf,yf,zf + 1}, {xf,yf + 1,zf + 1}, {xf,yf + 1, zf}, {xf,yf,zf} };						
									curOpaqueVertices += addQuadToVbo(VboOpaque, curOpaqueVertices, v[0] , v[1], v[2], v[3] , cube->getType()); 
									curOpaqueVertices += addQuadToVbo(VboOpaque, curOpaqueVertices, v[1] , v[6], v[5], v[2] , cube->getType()); 
									curOpaqueVertices += addQuadToVbo(VboOpaque, curOpaqueVertices, v[6], v[7], v[4], v[5], cube->getType());
									curOpaqueVertices += addQuadToVbo(VboOpaque, curOpaqueVertices, v[7], v[0], v[3], v[4], cube->getType());
									curOpaqueVertices += addQuadToVbo(VboOpaque, curOpaqueVertices, v[3], v[2], v[5], v[4], cube->getType());
									curOpaqueVertices += addQuadToVbo(VboOpaque, curOpaqueVertices, v[0], v[7], v[6], v[1], cube->getType());
								}
							} else if (cube->isTransparent())
							{
								if (countOnly)
								{
									*nbVertTransp += 36;
								}
								else
								{
									const float xf = float(x);
									const float yf = float(y);
									const float zf = float(z);

									YVec3f v[] = { {1.f + xf, yf, zf} , {xf + 1.f,yf + 1.f,zf}, {xf + 1.f,yf + 1,zf + 1},{xf + 1,yf,zf + 1}, {xf,yf,zf + 1}, {xf,yf + 1,zf + 1}, {xf,yf + 1, zf}, {xf,yf,zf} };
									curTransVertices += addQuadToVbo(VboTrasparent, curTransVertices, v[0], v[1], v[2], v[3], cube->getType());
									curTransVertices += addQuadToVbo(VboTrasparent, curTransVertices, v[1], v[6], v[5], v[2], cube->getType());
									curTransVertices += addQuadToVbo(VboTrasparent, curTransVertices, v[6], v[7], v[4], v[5], cube->getType());
									curTransVertices += addQuadToVbo(VboTrasparent, curTransVertices, v[7], v[0], v[3], v[4], cube->getType());
									curTransVertices += addQuadToVbo(VboTrasparent, curTransVertices, v[3], v[2], v[5], v[4], cube->getType());
									curTransVertices += addQuadToVbo(VboTrasparent, curTransVertices, v[0], v[7], v[6], v[1], cube->getType());
								}
							}
					}
			cout << "CurOpaque: " << curOpaqueVertices << "  total  " << *nbVertOpaque << endl;
			cout << "CurOpaque: " << curTransVertices << "  total  " << *nbVertTransp << endl;
		}

		/*
		Gestion du chunk
		*/

		void reset(void)
		{
			for(int x=0;x<CHUNK_SIZE;x++)
				for(int y=0;y<CHUNK_SIZE;y++)
					for(int z=0;z<CHUNK_SIZE;z++)
					{
						_Cubes[x][y][z].setDraw(false);
						_Cubes[x][y][z].setType(MCube::CUBE_AIR);
					}
		}

		void setVoisins(MChunk * xprev, MChunk * xnext, MChunk * yprev, MChunk * ynext, MChunk * zprev, MChunk * znext)
		{
			Voisins[0] = xprev;
			Voisins[1] = xnext;
			Voisins[2] = yprev;
			Voisins[3] = ynext;
			Voisins[4] = zprev;
			Voisins[5] = znext;
		}

		void get_surrounding_cubes(int x, int y, int z, MCube ** cubeXPrev, MCube ** cubeXNext,
			MCube ** cubeYPrev, MCube ** cubeYNext,
			MCube ** cubeZPrev, MCube ** cubeZNext)
		{

			*cubeXPrev = NULL;
			*cubeXNext = NULL;
			*cubeYPrev = NULL;
			*cubeYNext = NULL;
			*cubeZPrev = NULL;
			*cubeZNext = NULL;

			if (x == 0 && Voisins[0] != NULL)
				*cubeXPrev = &(Voisins[0]->_Cubes[CHUNK_SIZE - 1][y][z]);
			else if (x > 0)
				*cubeXPrev = &(_Cubes[x - 1][y][z]);

			if (x == CHUNK_SIZE - 1 && Voisins[1] != NULL)
				*cubeXNext = &(Voisins[1]->_Cubes[0][y][z]);
			else if (x < CHUNK_SIZE - 1)
				*cubeXNext = &(_Cubes[x + 1][y][z]);

			if (y == 0 && Voisins[2] != NULL)
				*cubeYPrev = &(Voisins[2]->_Cubes[x][CHUNK_SIZE - 1][z]);
			else if (y > 0)
				*cubeYPrev = &(_Cubes[x][y - 1][z]);

			if (y == CHUNK_SIZE - 1 && Voisins[3] != NULL)
				*cubeYNext = &(Voisins[3]->_Cubes[x][0][z]);
			else if (y < CHUNK_SIZE - 1)
				*cubeYNext = &(_Cubes[x][y + 1][z]);

			if (z == 0 && Voisins[4] != NULL)
				*cubeZPrev = &(Voisins[4]->_Cubes[x][y][CHUNK_SIZE - 1]);
			else if (z > 0)
				*cubeZPrev = &(_Cubes[x][y][z - 1]);

			if (z == CHUNK_SIZE - 1 && Voisins[5] != NULL)
				*cubeZNext = &(Voisins[5]->_Cubes[x][y][0]);
			else if (z < CHUNK_SIZE - 1)
				*cubeZNext = &(_Cubes[x][y][z + 1]);
		}

		void render(bool transparent)
		{
			if (transparent)
				VboTransparent->render();
			else
				VboOpaque->render();
		}

		/**
		  * On verifie si le cube peut être vu
		  */
		bool test_hidden(int x, int y, int z)
		{
			MCube * cubeXPrev = NULL; 
			MCube * cubeXNext = NULL; 
			MCube * cubeYPrev = NULL; 
			MCube * cubeYNext = NULL; 
			MCube * cubeZPrev = NULL; 
			MCube * cubeZNext = NULL; 

			if(x == 0 && Voisins[0] != NULL)
				cubeXPrev = &(Voisins[0]->_Cubes[CHUNK_SIZE-1][y][z]);
			else if(x > 0)
				cubeXPrev = &(_Cubes[x-1][y][z]);

			if(x == CHUNK_SIZE-1 && Voisins[1] != NULL)
				cubeXNext = &(Voisins[1]->_Cubes[0][y][z]);
			else if(x < CHUNK_SIZE-1)
				cubeXNext = &(_Cubes[x+1][y][z]);

			if(y == 0 && Voisins[2] != NULL)
				cubeYPrev = &(Voisins[2]->_Cubes[x][CHUNK_SIZE-1][z]);
			else if(y > 0)
				cubeYPrev = &(_Cubes[x][y-1][z]);

			if(y == CHUNK_SIZE-1 && Voisins[3] != NULL)
				cubeYNext = &(Voisins[3]->_Cubes[x][0][z]);
			else if(y < CHUNK_SIZE-1)
				cubeYNext = &(_Cubes[x][y+1][z]);

			if(z == 0 && Voisins[4] != NULL)
				cubeZPrev = &(Voisins[4]->_Cubes[x][y][CHUNK_SIZE-1]);
			else if(z > 0)
				cubeZPrev = &(_Cubes[x][y][z-1]);

			if(z == CHUNK_SIZE-1 && Voisins[5] != NULL)
				cubeZNext = &(Voisins[5]->_Cubes[x][y][0]);
			else if(z < CHUNK_SIZE-1)
				cubeZNext = &(_Cubes[x][y][z+1]);

			if( cubeXPrev == NULL || cubeXNext == NULL ||
				cubeYPrev == NULL || cubeYNext == NULL ||
				cubeZPrev == NULL || cubeZNext == NULL )
				return false;

			if (cubeXPrev->isOpaque() == true && //droite
				cubeXNext->isOpaque() == true && //gauche
				cubeYPrev->isOpaque() == true && //haut
				cubeYNext->isOpaque() == true && //bas
				cubeZPrev->isOpaque() == true && //devant
				cubeZNext->isOpaque() == true)  //derriere
				return true;
			return false;
		}

		void disableHiddenCubes(void)
		{
			cout << "hide" << endl;
			for(int x=0;x<CHUNK_SIZE;x++)
				for(int y=0;y<CHUNK_SIZE;y++)
					for(int z=0;z<CHUNK_SIZE;z++)
					{
						_Cubes[x][y][z].setDraw(true);
						if(!_Cubes[x][y][z].isPickable() || test_hidden(x,y,z))
							_Cubes[x][y][z].setDraw(false);
					}
		}


};