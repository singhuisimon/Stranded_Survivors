/**
 * @file Vector3D.cpp
 * @brief Declarations for the construction of struct Vector 3D
 * @author Amanda Leow Boon Suan (100%)
 * @date October 19, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#ifndef LOF_VECTOR3D_H
#define LOF_VECTOR3D_H

#include "Vector2D.h"

namespace lof {

	/**
	* @struct	Coord_3D
	* @brief	Inherits Coord location in Vec2D file that contains x and y. Adds in a z
	*			coordinate to allow it be Vec3D.
	*/
	struct Coord_3D : Coord {
		float z;
	};

	/**
	 * @struct Vec3D
	 * @brief Contains the constructor for Vec3D as well as operator functions
	 */
	typedef struct Vec3D : Coord_3D {

		/**
		* @brief Constructor for Vec3D
		*/
		Vec3D();

		/**
		 * @brief Parameterized constructor for Vec3D, initializing the vector with provided x and y values.
		 * @param x_val	The x-coordinate of the vector.
		 * @param y_val The y-coordinate of the vector.
		 * @param z_val The z-coordinate of the vector
		 */
		Vec3D(float x_val, float y_val, float z_val);

		/**
		 * @brief Assignment Operator, assign the values of one Vec3D to another.
		 * @param rhs The right-hand side Vec3D to copy values from.
		 * @return A reference to the assigned Vec3D.
		 */
		Vec3D& operator=(const Vec3D& rhs);

		/**
		 * @brief Copy Constructor, creates a Vec3D by copying the values from another Vec3D
		 * @param rhs The Vec3D to copy values from.
		 */
		Vec3D(const Vec3D& rhs);

		/**
		 * @brief Compond addition operator, adds another Vec3D to current vector.
		 * @param rhs The Vec3D to add.
		 * @return A reference to the Vec3D that witholds the final value.
		 */
		Vec3D& operator+=(const Vec3D& rhs);

		/**
		 * @brief Compond subtraction operator, subtracts another Vec3D from the current vector.
		 * @param rhs The Vec3D use to subtract.
		 * @return A reference to the Vec3D that witholds the final value.
		 */
		Vec3D& operator-=(const Vec3D& rhs);

		/**
		 * @brief Division operator, divides the vector's components by a scalar value.
		 * @param rhs The scalar value to divide by.
		 * @return A reference to the Vec3D that witholds the final value.
		 */
		Vec3D& operator/=(float rhs);

		/**
		 * @brief Multiplication operator, multiplies the vector's component by a scalar value.
		 * @param rhs The scalar value to divide by.
		 * @return A reference to the Vec3D that witholds the final value.
		 */
		Vec3D& operator*=(float rhs);

		/**
		 * @brief Unary negation Operator, returns the negative of the vector.
		 * @return A Vec3D representing the negated vector.
		 */
		Vec3D operator -();

	}Vector3D, Vec3;

	/**
	* @brief Binary addition operator, adds two Vec3D vectors.
	* @param lhs The left-hand side Vec3D.
	* @param rhs The right-hand side Vec3D.
	* @return A Vec3D containing the sum of the two vector.
	*/
	Vec3D operator + (const Vec3D& lhs, const Vec3D& rhs);

	/**
	* @brief Binary subtraction operator, subtract one Vec3D from another.
	* @param lhs The left-hand side Vec3D.
	* @param rhs The right-hand side Vec3D.
	* @return A Vec3D containing the final value.
	*/
	Vec3D operator - (const Vec3D& lhs, const Vec3D& rhs);

	/**
	* @brief Scalar multiplication operator, multiplies a Vec3D by a scalar.
	* @param lhs The Vec3D to multiply.
	* @param rhs The scalar value.
	* @return A Vec3D containing the product of the vector and scalar.
	*/
	Vec3D operator * (const Vec3D& lhs, float rhs);

	/**
	* @brief Scalar multiplication operator, multiplies a scalar by a Vec3D.
	* @param lhs The scalar value.
	* @param rhs The Vec3D to multiple.
	* @return A Vec3D containing the product of the scalar and vector.
	*/
	Vec3D operator * (float lhs, const Vec3D& rhs);

	/**
	* @brief Scalar division operator, divides a Vec3D by a scalar
	* @param lhs The Vec3D to divide.
	* @param rhs The scalar value.
	* @return A Vec3D containing the final value.
	*/
	Vec3D operator / (const Vec3D& lhs, float rhs);

	/**
	* @brief Normalizes the provided vector
	* @param result The vector in which the normalized vector is stored.
	* @param vec0 The vector to normalize.
	*/
	void normalize_vec3d(Vec3D& result, const Vec3D& vec0);

	/**
	* @brief Calculates the length (magnitude) of a vector.
	* @param vec0 The vector to calculate the length of.
	* @return The length of the vector.
	*/
	float length_vec3d(const Vec3D& vec0);

	/**
	* @brief Calculates the square length of a vector.
	* @param vec0 The vector to calculate the square length of.
	* @return The square of the vector's length.
	*/
	float square_length_vec3d(const Vec3D& vec0);

	/**
	* @brief Calculates the distance between two vectors.
	* @param vec0 The first vector.
	* @param vec1 The second vector.
	* @return The distance between the two vectors.
	*/
	float distance_vec3d(const Vec3D& vec0, const Vec3D& vec1);

	/**
	* @brief Calculates the square distance between two vectors.
	* @param vec0 The first vector.
	* @param vec1 The second vector.
	* @return The square distance between the two vectors.
	*/
	float square_distance_vec3d(const Vec3D& vec0, const Vec3D& vec1);

	/**
	* @brief Calculates the dot product of two vectors.
	* @param vec0 The first vector.
	* @param vec1 The second vector.
	* @return The dot product of the two vectors.
	*/
	float dot_product_vec3d(const Vec3D& vec0, const Vec3D& vec1);

	/**
	* @brief Calculates the magnitude of the cross product of two vectors.
	* @param vec0 The first vector.
	* @param vec1 The second vector.
	* @return The magnitude of the product.
	*/
	float cross_product_mag_vec3d(const Vec3D& vec0, const Vec3D& vec1);

	/**
	* @brief Converts Vec2D to Vec3D
	* @param vec2D The vector 2D to be converted into vector 3D
	* @oaram z Fixed to be value at 0.0 and is the new z coordinate for the conversion
	*		   unless otherwise stated.
	*/
	Vec3D vec2d_to_vec3d(const Vec2D& vec2D, float z_value = 0.0f);
}

#endif