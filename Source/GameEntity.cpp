#include "Game.h"

#include "Loader.h"

#include <vector>
using std::vector;

#include <string>
using std::string;

#include <iostream>
using namespace std;

GameEntity::GameEntity(string meshFile, string textureFile, Mat4 * m, Vec3 * startPosition)
{
	this->mesh = Loader::readMesh(meshFile);
	this->texture = (textureFile.size() > 0) ? Loader::loadTexture(textureFile) : 0;
	this->matrix = m;
	this->coordinates = startPosition;
}

GameEntity::GameEntity(Mesh * mesh, Mat4 * m, unsigned int texture, Vec3 * startPosition)
{
	this->mesh = mesh;
	this->matrix = m;
	this->texture = texture;
	this->coordinates = startPosition;
}

GameEntity::~GameEntity()
{
	delete mesh;
	delete matrix;
	delete coordinates;

}

float GameEntity::centre(int k)
{
	float minK, maxK;

	minK = (*mesh)[0].pos[k];
	maxK = (*mesh)[0].pos[k];

	for (unsigned i=0; i < mesh->size(); i++)
	{
		if ((*mesh)[i].pos[k] < minK)
			minK = (*mesh)[i].pos[k];

		if ((*mesh)[i].pos[k] > maxK)
			maxK = (*mesh)[i].pos[k];
	}

	return (minK + maxK) / 2.0f;
}

float GameEntity::centreY()
{
	return centre(1);
}

float GameEntity::centreZ()
{
	return centre(2);
}

float GameEntity::centreX()
{
	return centre(0);
}

void GameEntity::draw()
{
	glPushMatrix();

	// move our matrix to it's world coordinates
	Mat4 temp = Mat4::translate(*coordinates) * (*matrix);

	glMultMatrixf(temp);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &(*mesh)[0].tx);
	glNormalPointer(GL_FLOAT, sizeof(Vertex), &(*mesh)[0].nx);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexPointer(4, GL_FLOAT, sizeof(Vertex), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_QUADS, 0, mesh->size());
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();

}

void GameEntity::buffer()
{
	if (mesh->size() == 0)
		return;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh->size(), &(*mesh)[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GameEntity::setVBO(unsigned int vbo)
{
	this->vbo = vbo;
}

void GameEntity::setTexture(unsigned int texture)
{
	this->texture = texture;
}

void GameEntity::transform(Mat4 * m)
{
	(*this->matrix) *= (*m);
}

void GameEntity::loadIdentity()
{
	(*this->matrix) = Mat4();
}
