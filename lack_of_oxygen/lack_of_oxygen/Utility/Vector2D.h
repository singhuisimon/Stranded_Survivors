/**
 * @file Vector2D.h
 * @brief Declarations for the construction of struct Vector 2D
 * @author Amanda Leow Boon Suan (100%)
 * @date September 20, 2024 
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#ifndef LOF_VECTOR2D_H
#define LOF_VECTOR2D_H

// Include standard headers
#include <iostream>

namespace lof {

	/**
	 * @struct Coord
	 * @brief A simple struct representing 2D coordinates with x and y values.
	 */
	struct Coord {
		float x;
		float y;
	};

	/**
	 * @struct Vec2D
	 * @brief Contains the constructor for Vec2D as well as operator functions
	 */
	typedef struct Vec2D : Coord {

		/**
		 * @brief Default constructor for Vec2D
		 */
		Vec2D();

		/**
		 * @brief Parameterized constructor for Vec2D, initializing the vector with provided x and y values.
		 * @param x_val	The x-coordinate of the vector.
		 * @param y_val The y-coordinate of the vector.
		 */
		Vec2D(float x_val, float y_val);

		/**
		 * @brief Assignment Operator, assign the values of one Vec2D to another.
		 * @param rhs The right-hand side Vec2D to copy values from.
		 * @return A reference to the assigned Vec2D.
		 */
		Vec2D& operator=(const Vec2D& rhs);

		/**
		 * @brief Copy Constructor, creates a Vec2D by copying the values from another Vec2D
		 * @param rhs The Vec2D to copy values from.
		 */
		Vec2D(const Vec2D& rhs);

		/**
		 * @brief Compond addition operator, adds another Vec2D to current vector.
		 * @param rhs The Vec2D to add.
		 * @return A reference to the Vec2D that witholds the final value.
		 */
		Vec2D& operator += (const Vec2D& rhs);

		/**
		 * @brief Compond subtraction operator, subtracts another Vec2D from the current vector.
		 * @param rhs The Vec2D use to subtract.
		 * @return A reference to the Vec2D that witholds the final value.
		 */
		Vec2D& operator -= (const Vec2D& rhs);

		/**
		 * @brief Division operator, divides the vector's components by a scalar value.
		 * @param rhs The scalar value to divide by.
		 * @return A reference to the Vec2D that witholds the final value.
		 */
		Vec2D& operator /= (float rhs);

		/**
		 * @brief Multiplication operator, multiplies the vector's component by a scalar value.
		 * @param rhs The scalar value to divide by.
		 * @return A reference to the Vec2D that witholds the final value.
		 */
		Vec2D& operator *= (float rhs);

		/**
		 * @brief Unary negation Operator, returns the negative of the vector.
		 * @return A Vec2D representing the negated vector.
		 */
		Vec2D operator -() const;

	} Vector2D, Vec2;

	// Binary operators

	/**
	* @brief Binary addition operator, adds two Vec2D vectors.
	* @param lhs The left-hand side Vec2D.
	* @param rhs The right-hand side Vec2D.
	* @return A Vec2D containing the sum of the two vector.
	*/
	Vec2D operator + (const Vec2D& lhs, const Vec2D& rhs);

	/**
	* @brief Binary subtraction operator, subtract one Vec2D from another.
	* @param lhs The left-hand side Vec2D.
	* @param rhs The right-hand side Vec2D.
	* @return A Vec2D containing the final value.
	*/
	Vec2D operator - (const Vec2D& lhs, const Vec2D& rhs);

	/**
	* @brief Scalar multiplication operator, multiplies a Vec2D by a scalar.
	* @param lhs The Vec2D to multiply.
	* @param rhs The scalar value.
	* @return A Vec2D containing the product of the vector and scalar.
	*/
	Vec2D operator * (const Vec2D& lhs, float rhs);

	/**
	* @brief Scalar multiplication operator, multiplies a scalar by a Vec2D.
	* @param lhs The scalar value.
	* @param rhs The Vec2D to multiple.
	* @return A Vec2D containing the product of the scalar and vector.
	*/
	Vec2D operator * (float lhs, const Vec2D& rhs);

	/**
	* @brief Scalar division operator, divides a Vec2D by a scalar
	* @param lhs The Vec2D to divide.
	* @param rhs The scalar value.
	* @return A Vec2D containing the final value.
	*/
	Vec2D operator / (const Vec2D& lhs, float rhs);

	/**
	* @brief Normalizes the provided vector
	* @param result The vector in which the normalized vector is stored.
	* @param vec0 The vector to normalize.
	*/
	void normalize_vec2d(Vec2D& result, const Vec2D& vec0);

	/**
	* @brief Calculates the length (magnitude) of a vector.
	* @param vec0 The vector to calculate the length of.
	* @return The length of the vector.
	*/
	float length_vec2d(const Vec2D& vec0);

	/**
	* @brief Calculates the square length of a vector.
	* @param vec0 The vector to calculate the square length of.
	* @return The square of the vector's length.
	*/
	float square_length_vec2d(const Vec2D& vec0);

	/**
	* @brief Calculates the distance between two vectors.
	* @param vec0 The first vector.
	* @param vec1 The second vector.
	* @return The distance between the two vectors.
	*/
	float distance_vec2d(const Vec2D& vec0, const Vec2D& vec1);

	/**
	* @brief Calculates the square distance between two vectors.
	* @param vec0 The first vector.
	* @param vec1 The second vector.
	* @return The square distance between the two vectors.
	*/
	float square_distance_vec2d(const Vec2D& vec0, const Vec2D& vec1);

	/**
	* @brief Calculates the dot product of two vectors.
	* @param vec0 The first vector.
	* @param vec1 The second vector.
	* @return The dot product of the two vectors.
	*/
	float dot_product_vec2d(const Vec2D& vec0, const Vec2D& vec1);

	/**
	* @brief Calculates the magnitude of the cross product of two vectors.
	* @param vec0 The first vector.
	* @param vec1 The second vector.
	* @return The magnitude of the product.
	*/
	float cross_product_mag_vec2d(const Vec2D& vec0, const Vec2D& vec1);
}

#endif	// LOF_VECTOR2D_H