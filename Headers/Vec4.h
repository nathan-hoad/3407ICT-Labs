#pragma once
#include <iostream>

class Vec3;

/// 4D vector class
class Vec4
{
	public:
		union {
			struct {
				float x, y, z, w;
			};
			float data[4];
		};

		/**
			Constructor.

			\param x value of first element.
			\param y value of second element.
			\param z value of third element.
			\param w value of last element.
		*/
		Vec4(float x=0, float y=0, float z=0, float w=0);

		/**
		  Constructur.

		  \param data float[4] containing points
		*/
		Vec4(float * data);

		/**
			Single dimensional access to data

			\param x the index to return.
			\return data at position x.
		*/
		float& operator()(int x);

		/**
			Single dimensional access to data

			\param x the index to return.
			\return data at position x.
		*/
		float operator()(int x) const;

		/**
			Subtract one Vector from another.

			\param v the vector to subtract
			\return the result of subtracting v from the current Vec4.
		*/
		Vec4 operator-(const Vec4 & v) const;

		/**
			Cross product of two Vectors.

			\param v the other vector to use for a cross product.
			\return cross product of this Vec4 with v.
		*/
		Vec4 operator*(const Vec4 & v) const;

		/**
			divide vector components by a float.

			\param l the float to divide components on
			\return result of dividing components by l.
		*/
		Vec4 operator/(const float& l) const;

		/**
			returns the length of the vector.

			\return square root of the vector multiplied by itself
		*/
		float length();

		/**
			Normalised form of current Vec4 object.

			\return normalised version of this object.
		*/
		Vec4 normalised();

		/**
			Nice output!
		*/
		friend std::ostream & operator<<(std::ostream & o, const Vec4 & m);

		/**
		 	Conversion operator to turn a Vec4 into a Vec3
		*/
		operator Vec3();
};
