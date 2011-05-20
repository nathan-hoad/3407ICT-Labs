/**
 * Game classes.
 * Author: Nathan Hoad.
 * Date: 20th May 2011
 *
 * Description: This is a base set of classes use for a game engine.
 */
#pragma once

#include "Mat4.h"
#include "Primitives.h"
#include "Vec3.h"

#include <vector>

class GameEntity;

/**
 * "Game" class representing a game, containing all the entities and player, shader, etc.
 * All methods do nothing unless otherwise stated
 */
class Game
{
	private:
		std::vector<GameEntity *> gameEntities;
		GameEntity * player;
		int shader;

	public:
		/// Constructor. Does nothing, override initialise instead.
		Game();

		/**
		 * Destructor. Deletes all game entities, and the player. If you overload
		 * this make sure you call Game::~Game()
		 */
		virtual ~Game();

		/**
		 * Initialise the game. You should create your player and game entities here.
		 */
		virtual void initialise();

		/**
		 * Update the games parameters. This could be handling User input, moving objects, collisions, anything.
		 */
		virtual void update();

		/**
		  draw the Heads up display to the user.
		*/
		virtual void drawHUD();

		/**
		 * Draw all of the game entities, and the player.
		 *
		 * You shouldn't really need to override this, but if you do, call Game::drawGameEntities()
		 */
		virtual void drawGameEntities();

		/**
		 * links and compiles the vertex and fragment shaders for this game
		 *
		 * \param vertexShaderScript filepath to the vertex shader script to load
		 * \param fragmentShaderScript filepath to the vertex shader script to load
		 */
		void loadShader(std::string vertexShaderScript, std::string fragmentShaderScript);
};

class GameEntity
{
	private:
		/**
			Returns the centre point for an axis.

			\param k the axis to get the centre point of, using either X_AXIS, Y_AXIS or Z_AXIS.
			\return centre point of axis k.
		*/
		float centre(int k);

		Mesh * mesh;
		Mat4 * matrix;
		Vec3 * coordinates;

		unsigned int vbo, texture;

	public:

		/**
		 * Constructor. Loads a mesh and texture from file. Mesh will NOT be loaded to a VBO.
		 *
		 * \param meshFile filepath to the mesh
		 * \param textureFile filepath to the texture
		 * \param m pointer to this objects transformation matrix
		 * \param startPosition the starting position of this object
		 */
		GameEntity(std::string meshFile, std::string textureFile, Mat4 * m, Vec3 * startPosition);

		/**
		 * Constructor. Takes a pointer to a preloaded mesh and preloaded transformation matrix
		 */
		GameEntity(Mesh * mesh, Mat4 * m);

		/// Destructor
		virtual ~GameEntity();

		/**
			return centre point of cube on the x axis.

			\return x axis centroid.
		*/
		float centreX();

		/**
			return centre point of cube on the y axis.

			\return y axis centroid.
		*/
		float centreY();

		/**
			return centre point of cube on the z axis.

			\return z axis centroid.
		*/
		float centreZ();

		/**
		 * Draw this object. Makes appropriate glPush, glMult, various pointers and
		 * bindings, and calls glDrawArrays with a glPopMatrix to finish it all off.
		 * Unless you're doing something fancy, I wouldn't recommend overloading this
		 */
		virtual void draw();

		/**
		 * Send the mesh to a VBO
		 */
		void buffer();

		/**
		 * Set the VBO. This is ONLY to be used when you're loading multiple objects
		 * that use the same mesh, and want to save space.
		 *
		 * \param vbo the new vbo identifier to set for this class
		 */
		void setVBO(unsigned int vbo);

		/**
		 * Set the texture ID. This is ONLY to be used when you're loading the same
		 * texture onto multiple objects and want to save space.
		 *
		 * \param texture the new texture identifier to set for this class
		 */
		void setTexture(unsigned int texture);

		/**
		 * Transform this objects current transformation matrix by m. You should NOT
		 * store the coordinates in this matrix, they're computed by draw() using the coordinates vector.
		 *
		 * Stores current_matrix * m
		 *
		 * \param m pointer to the transformation matrix to multiply current matrix by.
		 */
		void transform(Mat4 * m);

		/**
		 * Reset the transformation matrix to an identity matrix
		 */
		void loadIdentity();
};
