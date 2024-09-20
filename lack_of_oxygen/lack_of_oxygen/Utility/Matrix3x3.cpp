#include "Matrix3x3.h"
#include "Vector2D.h"
//#include "main.h"

namespace lof
{
	//! Default Constructors
	Matrix_Lib::Matrix_Lib()
	{
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				mtx3x3[i][j] = 0;
			}
		}
	}

	///! Constructor for pointers
	Matrix_Lib::Matrix_Lib(const float* arr)
	{
		if (arr != nullptr) {
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j) {
					mtx3x3[i][j] = *arr;
					arr++;
				}
			}
		}
	}

	//! Overload Constructor
	Matrix_Lib::Matrix_Lib(float _00, float _01, float _02,
		float _10, float _11, float _12,
		float _20, float _21, float _22)
	{
		mtx3x3[0][0] = _00;
		mtx3x3[0][1] = _01;
		mtx3x3[0][2] = _02;
		mtx3x3[1][0] = _10;
		mtx3x3[1][1] = _11;
		mtx3x3[1][2] = _12;
		mtx3x3[2][0] = _20;
		mtx3x3[2][1] = _21;
		mtx3x3[2][2] = _22;

	}

	//! Copy Constructors
	Matrix_Lib::Matrix_Lib(Matrix_Lib const& rhs) {

		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				mtx3x3[i][j] = rhs.mtx3x3[i][j];
			}
		}
	}

	//! Copy Assignmnet
	Matrix_Lib& Matrix_Lib::operator=(const Matrix_Lib& rhs)
	{
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				mtx3x3[i][j] = rhs.mtx3x3[i][j];
			}
		}

		return *this;
	}


	//! Assignment Operators
	Matrix_Lib& Matrix_Lib::operator *= (const Matrix_Lib& rhs)
	{
		Matrix_Lib stored;
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				stored.mtx3x3[i][j] = mtx3x3[i][j];
			}
		}

		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				mtx3x3[i][j] = stored.mtx3x3[i][0] * rhs.mtx3x3[0][j]
					+ stored.mtx3x3[i][1] * rhs.mtx3x3[1][j]
					+ stored.mtx3x3[i][2] * rhs.mtx3x3[2][j];
			}
		}

		return *this;
	}

	//! Matrix Multiplication Operator
	Matrix_Lib operator * (const Matrix_Lib& lhs, const Matrix_Lib& rhs)
	{
		Matrix_Lib result;
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				result.mtx3x3[i][j] = lhs.mtx3x3[i][0] * rhs.mtx3x3[0][j]
					+ lhs.mtx3x3[i][1] * rhs.mtx3x3[1][j]
					+ lhs.mtx3x3[i][2] * rhs.mtx3x3[2][j];
			}
		}

		return result;
	}

	//! Matrix-Vector Multiplication Operator
	Vector2D operator * (const Matrix_Lib& lhs, const Vector2D& rhs)
	{
		float rhs_zcoord = 1; //sets the vector object's z values to 1 to facilitate multiplying a 2x2 vector with a 3x3 matrix
		Vector2D result;
		result.x = lhs.mtx3x3[0][0] * rhs.x + lhs.mtx3x3[0][1] * rhs.y + lhs.mtx3x3[0][2] * rhs_zcoord;
		result.y = lhs.mtx3x3[1][0] * rhs.x + lhs.mtx3x3[1][1] * rhs.y + lhs.mtx3x3[1][2] * rhs_zcoord;
		return result;
	}

	//! Function turns matrix into identity matrix
	void mtx3x3_identity(Matrix_Lib& result)
	{
		result.mtx3x3[0][0] = 1;
		result.mtx3x3[0][1] = 0;
		result.mtx3x3[0][2] = 0;

		result.mtx3x3[1][0] = 0;
		result.mtx3x3[1][1] = 1;
		result.mtx3x3[1][2] = 0;

		result.mtx3x3[2][0] = 0;
		result.mtx3x3[2][1] = 0;
		result.mtx3x3[2][2] = 1;
	}

	//! Function creates a translation matrix from x & y and saves it in result
	void mtx3x3_translate(Matrix_Lib& result, float x, float y)
	{
		result.mtx3x3[0][0] = 1;
		result.mtx3x3[0][1] = 0;
		result.mtx3x3[0][2] = x;

		result.mtx3x3[1][0] = 0;
		result.mtx3x3[1][1] = 1;
		result.mtx3x3[1][2] = y;

		result.mtx3x3[2][0] = 0;
		result.mtx3x3[2][1] = 0;
		result.mtx3x3[2][2] = 1;
	}

	//! Function creates a scaling matrix from x & y and saves it in result
	void mtx3x3_scale(Matrix_Lib& result, float x, float y)
	{
		result.mtx3x3[0][0] = x;
		result.mtx3x3[0][1] = 0;
		result.mtx3x3[0][2] = 0;

		result.mtx3x3[1][0] = 0;
		result.mtx3x3[1][1] = y;
		result.mtx3x3[1][2] = 0;

		result.mtx3x3[2][0] = 0;
		result.mtx3x3[2][1] = 0;
		result.mtx3x3[2][2] = 1;
	}

	//! Function creates a rotation matrix from "angle" whose value is in radian and saves it in result
	void Mmtx3x3_rot_rad(Matrix_Lib& result, float angle)
	{
		result.mtx3x3[0][2] = 0;
		result.mtx3x3[1][2] = 0;
		result.mtx3x3[2][0] = 0;
		result.mtx3x3[2][1] = 0;
		result.mtx3x3[2][2] = 1; //sets the object's z values to 1 as per the rotation matrix

		result.mtx3x3[0][0] = cos(angle);
		result.mtx3x3[0][1] = -sin(angle);
		result.mtx3x3[1][0] = sin(angle);
		result.mtx3x3[1][1] = cos(angle);
	}

	//! Function creates a rotation matrix from "angle" whose value is in degree and saves it in result
	void mtx3x3_rot_deg(Matrix_Lib& result, float angle)
	{
		result.mtx3x3[0][2] = 0;
		result.mtx3x3[1][2] = 0;
		result.mtx3x3[2][0] = 0;
		result.mtx3x3[2][1] = 0;
		result.mtx3x3[2][2] = 1; //sets the object's z values to 1 as per the rotation matrix

		float angle_rad = angle * PI / 180; //converts the value of the angle from degree to radian
		result.mtx3x3[0][0] = cos(angle_rad);
		result.mtx3x3[0][1] = -sin(angle_rad);
		result.mtx3x3[1][0] = sin(angle_rad);
		result.mtx3x3[1][1] = cos(angle_rad);
	}

	//! Function creates a transpose matrix of matrix and saves it in result
	void mtx3x3_transpose(Matrix_Lib& result, const Matrix_Lib& pMtx)
	{
		float store1 = pMtx.mtx3x3[0][1]; //to go to m10
		float store2 = pMtx.mtx3x3[0][2]; //to go to m20
		float store3 = pMtx.mtx3x3[1][0]; //to go to m01

		float store4 = pMtx.mtx3x3[1][2]; //to go to m21
		float store5 = pMtx.mtx3x3[2][0]; //to go to m02
		float store6 = pMtx.mtx3x3[2][1]; //to go to m12

		result.mtx3x3[0][0] = pMtx.mtx3x3[0][0];
		result.mtx3x3[0][1] = store3;
		result.mtx3x3[0][2] = store5;

		result.mtx3x3[1][0] = store1;
		result.mtx3x3[1][1] = pMtx.mtx3x3[1][1];
		result.mtx3x3[1][2] = store6;

		result.mtx3x3[2][0] = store2;
		result.mtx3x3[2][1] = store4;
		result.mtx3x3[2][2] = pMtx.mtx3x3[2][2];

	}


	// !Function calculates the inverse matrix of matrix and saves it in result
	//! If the matrix inversion fails, result would be set to NULL
	void mtx3x3_inverse(Matrix_Lib* result, float* determinant, const Matrix_Lib& matrix)
	{
		//finding determinent, first calculating the minor of the three elements in the top row
		float minor_o1 = (matrix.mtx3x3[1][1] * matrix.mtx3x3[2][2]) - (matrix.mtx3x3[1][2] * matrix.mtx3x3[2][1]);
		float minor_o2 = (matrix.mtx3x3[1][0] * matrix.mtx3x3[2][2]) - (matrix.mtx3x3[1][2] * matrix.mtx3x3[2][0]);
		float minor_o3 = (matrix.mtx3x3[1][0] * matrix.mtx3x3[2][1]) - (matrix.mtx3x3[1][1] * matrix.mtx3x3[2][0]);
		*determinant = matrix.mtx3x3[0][0] * minor_o1 - (matrix.mtx3x3[0][1] * minor_o2) + (matrix.mtx3x3[0][2] * minor_o3);

		//if determinent is zero, return
		if (*determinant == 0) {
			result = NULL;
			return;
		}

		//calculating the cofactor of each element
		float cofactor_01 = minor_o1;
		float cofactor_02 = -minor_o2;
		float cofactor_03 = minor_o3;

		float cofactor_11 = -((matrix.mtx3x3[0][1] * matrix.mtx3x3[2][2]) - (matrix.mtx3x3[0][2] * matrix.mtx3x3[2][1]));
		float cofactor_12 = (matrix.mtx3x3[0][0] * matrix.mtx3x3[2][2]) - (matrix.mtx3x3[0][2] * matrix.mtx3x3[2][0]);
		float cofactor_13 = -((matrix.mtx3x3[0][0] * matrix.mtx3x3[2][1]) - (matrix.mtx3x3[0][1] * matrix.mtx3x3[2][0]));

		float cofactor_21 = (matrix.mtx3x3[0][1] * matrix.mtx3x3[1][2]) - (matrix.mtx3x3[0][2] * matrix.mtx3x3[1][1]);
		float cofactor_22 = -((matrix.mtx3x3[0][0] * matrix.mtx3x3[1][2]) - (matrix.mtx3x3[0][2] * matrix.mtx3x3[1][0]));
		float cofactor_23 = (matrix.mtx3x3[0][0] * matrix.mtx3x3[1][1]) - (matrix.mtx3x3[0][1] * matrix.mtx3x3[1][0]);

		Matrix_Lib cof_result;
		Matrix_Lib adj_result;

		//assigns values to a cofactor matrix
		cof_result.mtx3x3[0][0] = cofactor_01;
		cof_result.mtx3x3[0][1] = cofactor_02;
		cof_result.mtx3x3[0][2] = cofactor_03;
		cof_result.mtx3x3[1][0] = cofactor_11;
		cof_result.mtx3x3[1][1] = cofactor_12;
		cof_result.mtx3x3[1][2] = cofactor_13;
		cof_result.mtx3x3[2][0] = cofactor_21;
		cof_result.mtx3x3[2][1] = cofactor_22;
		cof_result.mtx3x3[2][2] = cofactor_23;

		//transposes the values and stores them in Adj_pResult
		mtx3x3_transpose(adj_result, cof_result);


		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				result->mtx3x3[i][j] = adj_result.mtx3x3[i][j] * (1.0f / *determinant);
			}
		}


	}
}