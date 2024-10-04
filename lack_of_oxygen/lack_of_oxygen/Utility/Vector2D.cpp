/**
 * @file Vector2D.cpp
 * @brief Defintion for the functions in struct Vector 2D.
 * @author Amanda Leow Boon Suan (100%)
 * @date September 20, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#include "Vector2D.h"

namespace lof {
	//! Constructor
	Vec2D::Vec2D() {
		x = 0.0;
		y = 0.0;
	}

	//!
	Vec2D::Vec2D(float x_val, float y_val) {
		x = x_val;
		y = y_val;
	}

	//!
	Vec2D& Vec2D::operator=(const Vec2D& rhs) {
		if (this != &rhs) {
			x = rhs.x;
			y = rhs.y;
		}

		return *this;
	}

	//!
	Vec2D::Vec2D(const Vec2D& rhs) {
		x = rhs.x;
		y = rhs.y;
	}

	//! Assignment Operator +=
	Vec2D& Vec2D::operator += (const Vec2D& rhs) {
		x += rhs.x;
		y += rhs.y;

		return *this;
	}

	//! Assignment Operator -=
	Vec2D& Vec2D::operator -= (const Vec2D& rhs) {
		x -= rhs.x;
		y -= rhs.y;

		return *this;
	}

	//! Assignment Operator /=
	Vec2D& Vec2D::operator /= (float rhs) {
		x /= rhs;
		y /= rhs;

		return *this;
	}

	//! Assignment Operator *=
	Vec2D& Vec2D::operator *= (float rhs) {
		x *= rhs;
		y *= rhs;

		return *this;
	}

	//! Unary Operator
	Vec2D Vec2D::operator -() const {
		return Vec2D(-x, -y);
	}

	//! Binary + operator
	Vec2D operator + (const Vec2D& lhs, const Vec2D& rhs) {
		Vec2D result = lhs;
		result += rhs;

		return result;
	}

	//! Binary - operator
	Vec2D operator - (const Vec2D& lhs, const Vec2D& rhs) {
		Vec2D result = lhs;
		result -= rhs;

		return result;
	}

	//! Binary * operator
	Vec2D operator * (const Vec2D& lhs, float rhs) {
		Vec2D result = lhs;
		result *= rhs;

		return result;
	}

	//! Binary * operator
	Vec2D operator * (float lhs, const Vec2D& rhs) {
		Vector2D result = rhs;
		result *= lhs;

		return result;
	}

	//! Binary / operator
	Vec2D operator / (const Vec2D& lhs, float rhs) {
		Vector2D result = lhs;
		result /= rhs;

		return result;
	}

	//! result vector is the normalize vector of vec0
	void normalize_vec2d(Vec2D& result, const Vec2D& vec0) {
		float magnitude;
		float pow2x, pow2y;
		pow2x = vec0.x * vec0.x;
		pow2y = vec0.y * vec0.y;
		magnitude = sqrt(pow2x + pow2y);
		result.x = vec0.x / magnitude;
		result.y = vec0.y / magnitude;
	}

	//! find the length of the vector
	float length_vec2d(const Vec2D& vec0) {
		float length;
		float pow2x, pow2y;
		pow2x = vec0.x * vec0.x;
		pow2y = vec0.y * vec0.y;
		length = sqrt(pow2x + pow2y);

		return length;
	}

	//! find the squared length of the vector
	float square_length_vec2d(const Vec2D& vec0) {
		float length;
		float pow2x, pow2y;
		pow2x = vec0.x * vec0.x;
		pow2y = vec0.y * vec0.y;
		length = pow2x + pow2y;

		return length;
	}

	//! find the distance between vec0 and vec1
	float distance_vec2d(const Vec2D& vec0, const Vec2D& vec1) {
		float distance;
		float distx, disty;

		distx = vec1.x - vec0.x;
		disty = vec1.y - vec0.y;
		distx = distx * distx;
		disty = disty * disty;
		distance = sqrt(distx + disty);

		return distance;
	}

	//! find the square distance between vec0 and vec1
	float square_distance_vec2d(const Vec2D& vec0, const Vec2D& vec1) {
		float distance;
		float distx, disty;

		distx = vec1.x - vec0.x;
		disty = vec1.y - vec0.y;
		distx = distx * distx;
		disty = disty * disty;
		distance = distx + disty;

		return distance;
	}

	//! find the dot product of vec0 and vec1
	float dot_product_vec2d(const Vec2D& vec0, const Vec2D& vec1) {
		float product;
		float prodx, prody;

		prodx = vec0.x * vec1.x;
		prody = vec0.y * vec1.y;

		product = prodx + prody;

		return product;
	}

	//! find the cross product magnitude of vec0 and vec1
	float cross_product_mag_vec2d(const Vec2D& vec0, const Vec2D& vec1) {
		float crosspro;
		float crossx, crossy, crossz;
		float z = 0;

		crossx = (vec0.y * z) - (z * vec1.y);
		crossy = -((z * vec1.x) - (vec0.x * z));
		crossz = (vec0.x * vec1.y) - (vec0.y * vec1.x);

		crosspro = crossx + crossy + crossz;

		return crosspro;
	}
}