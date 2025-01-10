/**
 * @file Vector3D.h
 * @brief Defintion for the functions in struct Vector 2D.
 * @author Amanda Leow Boon Suan (100%)
 * @date October 19, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */


#include "Vector3D.h"

namespace lof {
	Vec3D::Vec3D() {
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}

	Vec3D::Vec3D(float x_val, float y_val, float z_val) {
		x = x_val;
		y = y_val;
		z = z_val;
	}

	Vec3D& Vec3D::operator=(const Vec3D& rhs) {
		if (this != &rhs) {
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
		}

		return *this;
	}

	Vec3D::Vec3D(const Vec3D& rhs) {
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
	}

	Vec3D& Vec3D::operator+=(const Vec3D& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;

		return *this;
	}

	Vec3D& Vec3D::operator-=(const Vec3D& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;

		return *this;
	}

	Vec3D& Vec3D::operator/=(float rhs) {
		x /= rhs;
		y /= rhs;
		z /= rhs;

		return *this;
	}

	Vec3D& Vec3D::operator*=(float rhs) {
		x *= rhs;
		y *= rhs;
		z *= rhs;

		return *this;
	}

	Vec3D Vec3D::operator -() {
		return Vec3D(-x, -y, -z);
	}

	Vec3D operator + (const Vec3D& lhs, const Vec3D& rhs) {
		Vec3D result = lhs;
		result += rhs;

		return result;
	}
	Vec3D operator - (const Vec3D& lhs, const Vec3D& rhs) {
		Vec3D result = lhs;
		result -= rhs;

		return result;
	}
	Vec3D operator * (const Vec3D& lhs, float rhs) {
		Vec3D result = lhs;
		result *= rhs;

		return result;
	}
	Vec3D operator * (float lhs, const Vec3D& rhs) {
		Vec3D result = rhs;
		result *= lhs;

		return result;
	}
	Vec3D operator / (const Vec3D& lhs, float rhs) {
		Vec3D result = lhs;
		result /= rhs;

		return result;
	}

	void normalize_vec3d(Vec3D& result, const Vec3D& vec0) {
		float magnitude, pow2x, pow2y, pow2z;
		pow2x = vec0.x * vec0.x;
		pow2y = vec0.y * vec0.y;
		pow2z = vec0.z * vec0.z;
		magnitude = sqrt(pow2x + pow2y + pow2z);
		result.x = vec0.x / magnitude;
		result.y = vec0.y / magnitude;
		result.z = vec0.z / magnitude;
	}

	float length_vec3d(const Vec3D& vec0) {
		float length = sqrt(square_length_vec3d(vec0));
		return length;
	}

	float square_length_vec3d(const Vec3D& vec0) {
		float length, pow2x, pow2y, pow2z;
		pow2x = vec0.x * vec0.x;
		pow2y = vec0.y * vec0.y;
		pow2z = vec0.z * vec0.z;
		length = pow2x + pow2y + pow2z;
		return length;
	}

	float distance_vec3d(const Vec3D& vec0, const Vec3D& vec1) {
		float distance, distx, disty, distz;
		distx = vec1.x - vec0.x;
		disty = vec1.y - vec0.y;
		distz = vec1.z - vec0.z;
		distance = sqrt(distx * distx + disty * disty + distz * distz);
		return distance;
	}

	float square_distance_vec3d(const Vec3D& vec0, const Vec3D& vec1) {
		float distance = distance_vec3d(vec0, vec1);
		return distance * distance;
	}

	float dot_product_vec3d(const Vec3D& vec0, const Vec3D& vec1) {
		float product, prodx, prody, prodz;
		prodx = vec0.x * vec1.x;
		prody = vec0.y * vec1.y;
		prodz = vec0.z * vec1.z;
		product = prodx + prody + prodz;
		return product;
	}

	float cross_product_mag_vec3d(const Vec3D& vec0, const Vec3D& vec1) {
		float crosspro, crossx, crossy, crossz;
		crossx = (vec0.y * vec1.z) - (vec0.z * vec1.y);
		crossy = (vec0.z * vec1.x) - (vec0.x * vec1.z);
		crossz = (vec0.x * vec1.y) - (vec0.y * vec1.x);
		crosspro = sqrt((crossx * crossx) + (crossy * crossy) + (crossz * crossz));
		return crosspro;
	}

	Vec3D vec2d_to_vec3d(const Vec2D& vec2D, float z_value) {
		return Vec3D(vec2D.x, vec2D.y, z_value);
	}
}