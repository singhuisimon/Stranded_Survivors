#ifndef LOF_VECTOR3D_H
#define LOF_VECTOR3D_H

#include "Vector2D.h"

namespace lof {
	struct Coord_3D : Coord {
		float z;
		/*union {
			struct {
				float z;
			};
			float coords[3];
		};*/
	};

	typedef struct Vec3D : Coord_3D {
		Vec3D();
		Vec3D(float x_val, float y_val, float z_val);
		Vec3D& operator=(const Vec3D& rhs);
		Vec3D(const Vec3D& rhs);
		Vec3D& operator+=(const Vec3D& rhs);
		Vec3D& operator-=(const Vec3D& rhs);
		Vec3D& operator/=(float rhs);
		Vec3D& operator*=(float rhs);

		Vec3D operator -();

	}Vector3D, Vec3;

	Vec3D operator + (const Vec3D& lhs, const Vec3D& rhs);
	Vec3D operator - (const Vec3D& lhs, const Vec3D& rhs);
	Vec3D operator * (const Vec3D& lhs, float rhs);
	Vec3D operator * (float lhs, const Vec3D& rhs);
	Vec3D operator / (const Vec3D& lhs, float rhs);

	void normalize_vec3d(Vec3D& result, const Vec3D& vec0);
	float length_vec3d(const Vec3D& vec0);
	float square_length_vec3d(const Vec3D& vec0);
	float distance_vec3d(const Vec3D& vec0, const Vec3D& vec1);
	float square_distance_vec3d(const Vec3D& vec0, const Vec3D& vec1);
	float dot_product_vec3d(const Vec3D& vec0, const Vec3D& vec1);
	float cross_product_mag_vec3d(const Vec3D& vec0, const Vec3D& vec1);
	Vec3D vec2d_to_vec3d(const Vec2D& vec2D, float z_value = 0.0f);
}

#endif