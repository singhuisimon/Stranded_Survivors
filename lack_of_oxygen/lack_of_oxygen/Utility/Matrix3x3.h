#include <iostream>
#include <math.h>
#include "Vector2D.h"

#pragma once
namespace lof {

    typedef struct Matrix_Lib {

        float mtx3x3[3][3];
        
        //! Constructors
        Matrix_Lib();
        Matrix_Lib(const float* arr);

        //! Overload Constructor
        Matrix_Lib(float p00, float p01, float p02,
                   float p10, float p11, float p12,
                   float p20, float p21, float p22);

        //! Copy Constructors
        Matrix_Lib(Matrix_Lib const&);

        //! Copy Assignmnet
        Matrix_Lib& operator=(Matrix_Lib const&);

        //! Assignment Operators
        Matrix_Lib& operator *= (const Matrix_Lib& rhs);


     } Matrix3x3, Mtx33;

    //! Matrix Multiplication Operator
    Matrix_Lib operator * (const Matrix_Lib& lhs, const Matrix_Lib& rhs);

    //! Matrix-Vector Multiplication Operator
    CSD1130::Vector2D  operator * (const Matrix_Lib& lhs, const CSD1130::Vector2D& rhs);

    //! Function turns matrix into identity matrix
    void mtx3x3_identity(Matrix_Lib& result);

    //! Function creates a translation matrix from x & y and saves it in result
    void mtx3x3_translate(Matrix_Lib& result, float x, float y);

    //! Function creates a scaling matrix from x & y and saves it in result
    void mtx3x3_scale(Matrix_Lib& result, float x, float y);

    //! Function creates a rotation matrix from "angle" whose value is in radian and saves it in result
    void Mmtx3x3_rot_rad(Matrix_Lib& result, float angle);

    //! Function creates a rotation matrix from "angle" whose value is in degree and saves it in result
    void mtx3x3_rot_deg(Matrix_Lib& result, float angle);

    //! Function creates a transpose matrix of matrix and saves it in result
    void mtx3x3_transpose(Matrix_Lib& result, const Matrix_Lib& matrix);

    // !Function calculates the inverse matrix of matrix and saves it in result
    //! If the matrix inversion fails, result would be set to NULL
    void mtx3x3_inverse(Matrix_Lib* result, float* determinant, const Matrix_Lib& matrix);

}