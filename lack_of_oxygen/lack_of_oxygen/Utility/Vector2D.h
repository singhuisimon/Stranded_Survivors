/**
 * @file Vector2D.h
 * @brief Declarations for the construction of struct Vector 2D
 * @author Amanda Leow Boon Suan
 * @date September 20, 2024
 */

#ifndef LOF_VECTOR2D_H
#define LOF_VECTOR2D_H

// Include standard headers
#include <iostream>

namespace lof {

	struct Coord {
		float x;
		float y;
	};

	typedef struct Vec2D : Coord {

		Vec2D();
		Vec2D(float x_val, float y_val);

		Vec2D& operator=(const Vec2D& rhs);
		Vec2D(const Vec2D& rhs);

		Vec2D& operator += (const Vec2D& rhs);
		Vec2D& operator -= (const Vec2D& rhs);
		Vec2D& operator /= (float rhs);
		Vec2D& operator *= (float rhs);

		Vec2D operator -() const;

	} Vector2D, Vec2;

	// Binary operators
	Vec2D operator + (const Vec2D& lhs, const Vec2D& rhs);
	Vec2D operator - (const Vec2D& lhs, const Vec2D& rhs);
	Vec2D operator * (const Vec2D& lhs, float rhs);
	Vec2D operator * (float lhs, const Vec2D& rhs);
	Vec2D operator / (const Vec2D& lhs, float rhs);

	void normalize_vec2d(Vec2D& result, const Vec2D& vec0);
	float length_vec2d(const Vec2D& vec0);
	float square_length_vec2d(const Vec2D& vec0);
	float distance_vec2d(const Vec2D& vec0, const Vec2D& vec1);
	float square_distance_vec2d(const Vec2D& vec0, const Vec2D& vec1);
	float dot_product_vec2d(const Vec2D& vec0, const Vec2D& vec1);
	float cross_product_mag_vec2d(const Vec2D& vec0, const Vec2D& vec1);
}

#endif	// LOF_VECTOR2D_H