#pragma once

float static lerp(float a, float b, float alpha) {
	float alphaSmooth = alpha * (3 * alpha - 2 * alpha*alpha);
	return (1 - alphaSmooth)*a + alphaSmooth * b;
}

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