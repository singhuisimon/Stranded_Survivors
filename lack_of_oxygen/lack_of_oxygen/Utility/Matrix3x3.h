/**
 * @file Matrix3x3.h
 * @brief Declarations of memeber and non-member functions of struct Matrix_Lib
 * @author Liliana Hanawardani (100%)
 * @date September 20, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#include <iostream>
#include <math.h>
#include "Vector2D.h"

#pragma once

namespace lof {

    /**
    * @brief Struct of Matrix_Lib; Manages matrixes and calculations involving them
    */
    typedef struct Matrix_Lib {

        // Set the size of the matrix to 3x3
        float mtx3x3[3][3];

        /**
        * @brief Default constructor for Matrix_Lib object
        */
        Matrix_Lib();

        /**
        * @brief Single argument constructor of Matrix_Lib for a const float* variable 
        * @param arr Const float variable holding values for the matrix object
        */
        Matrix_Lib(const float* arr);

        /**
        * @brief Constructor of Matrix_Lib for a multiple float variables
        * @param Individual float variables whose values are to be assigned to the Matrix's corresponding indexes 
        */
        Matrix_Lib(float p00, float p01, float p02,
                   float p10, float p11, float p12,
                   float p20, float p21, float p22);

        /**
        * @brief Copy constructor for Matrix_Lib
        * @param rhs Const reference to another Matrix object whose values are to be copied to the constructed object
        */
        Matrix_Lib(Matrix_Lib const& rhs);

        /**
        * @brief Overloaded operator=/Copy assignment operator for Matrix_Lib
        * @param rhs Const reference to another Matrix object whose values are to be copied to the current object
        * @return Reference to the current object whose values are reassigned with those of the parameter
        */
        Matrix_Lib& operator=(Matrix_Lib const& rhs);

        /**
        * @brief Overloaded operator*= for Matrix_Lib
        * @param rhs Const reference to another Matrix object whose values are to be used in calculation
        * @return Reference to the current object whose values are reassigned with the new calculated values
        */
        Matrix_Lib& operator *= (const Matrix_Lib& rhs);


     } Matrix3x3, Mtx33;
    
    /**
    * @brief Overloaded operator* for multiplication of two matrix objects
    * @param lhs Const reference to lhs Matrix object whose values are to be multiplied in calculation
    * @param rhs Const reference to rhs Matrix object whose values are to be multiplied in calculation
    * @return Newly constructed object that are assigned the multiplied values
    */
    Matrix_Lib operator * (const Matrix_Lib& lhs, const Matrix_Lib& rhs);

    /**
    * @brief Overloaded operator* for multiplication of a matrix object and a Vec2D object
    * @param lhs Const reference to a Matrix object whose values are to be multiplied in calculation
    * @param rhs Const reference to a Vec2D object whose values are to be multiplied in calculation
    * @return Newly constructed Vec2D object that are assigned the multiplied values
    */
    Vec2D  operator * (const Matrix_Lib& lhs, const Vec2D& rhs);

    /**
    * @brief Turns matrix into identity matrix
    * @param result Reference to the Matrix object to be turned into an identity matrix
    */
    void mtx3x3_identity(Matrix_Lib& result);
    
    /**
    * @brief Creates a translation matrix from x & y and saves it in the matrix object in the parameter
    * @param result Reference to the Matrix object to be turned into a translation matrix
    * @param x Value of x that the matrix will translate by
    * @param y Value of y that the matrix will translate by
    */
    void mtx3x3_translate(Matrix_Lib& result, float x, float y);

    /**
    * @brief Creates a scaling matrix from x & y and saves it in the matrix object in the parameter
    * @param result Reference to the Matrix object to be turned into a scaling matrix
    * @param x Value of x that the matrix will scale by
    * @param y Value of y that the matrix will scale by
    */
    void mtx3x3_scale(Matrix_Lib& result, float x, float y);

    /**
    * @brief Creates a rotation matrix from "angle" whose value is in radian and saves it in the matrix object
    * @param result Reference to the Matrix object to be turned into a rotation matrix
    * @param angle Value of the angle in radian 
    */
    void Mmtx3x3_rot_rad(Matrix_Lib& result, float angle);

    /**
    * @brief Creates a rotation matrix from "angle" whose value is in degree and saves it in the matrix object
    * @param result Reference to the Matrix object to be turned into a rotation matrix
    * @param angle Value of the angle in degree
    */
    void mtx3x3_rot_deg(Matrix_Lib& result, float angle);

    /**
    * @brief Creates a transpose matrix of "matrix" and saves it in "result"
    * @param result Reference to the Matrix object to be turned into a transpose matrix  
    * @param matrix Reference to the Matrix object whose values are used in calculation
    */
    void mtx3x3_transpose(Matrix_Lib& result, const Matrix_Lib& matrix);

    /**
    * @brief Calculates the inverse matrix of "matrix" and saves it in "result".
    * @param result Pointer to the Matrix object to be turned into an inverse matrix. If the matrix inversion fails, result would be set to NULL
    * @param determinant Pointer to a float variable to store the determinant used in calculation
    * @param matrix Const reference to the Matrix object whose values will be used in calculation
    */
    void mtx3x3_inverse(Matrix_Lib* result, float* determinant, const Matrix_Lib& matrix);

}