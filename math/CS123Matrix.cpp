/*!
   @file   CS123Matrix.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Provides basic functionality for a templated, arbitrarily-sized matrix.
      You will need to fill this file in for the Camtrans assignment.

**/

#include "CS123Algebra.h"
#include <iostream>

//@name Routines which generate specific-purpose transformation matrices
//@{---------------------------------------------------------------------
// @returns the scale matrix described by the vector
Matrix4x4 getScaleMat(const Vector4 &v) {
    return  Matrix4x4(v.x,0,0,0,
                      0,v.y,0,0,
                      0,0,v.z,0,
                      0,0,0,1);

}

// @returns the translation matrix described by the vector
Matrix4x4 getTransMat(const Vector4 &v) {
    return Matrix4x4(1,0,0,v.x,
                     0,1,0,v.y,
                     0,0,1,v.z,
                     0,0,0,1);

}

// @returns the rotation matrix about the x axis by the specified angle
Matrix4x4 getRotXMat (const REAL radians) {

    REAL cosx = cos(radians);
    REAL sinx = sin(radians);
    return Matrix4x4(1,0,0,0,
                     0,cosx,-sinx,0,
                     0,sinx,cosx,0,
                     0,0,0,1);

}

// @returns the rotation matrix about the y axis by the specified angle
Matrix4x4 getRotYMat (const REAL radians) {

    REAL cosx = cos(radians);
    REAL sinx = sin(radians);
    return Matrix4x4(cosx,0,sinx,0,
                     0,1,0,0,
                     -sinx,0,cosx,0,
                     0,0,0,1);

}

// @returns the rotation matrix about the z axis by the specified angle
Matrix4x4 getRotZMat (const REAL radians) {

    REAL cosx = cos(radians);
    REAL sinx = sin(radians);
    return Matrix4x4(cosx,-sinx,0,0,
                     sinx,cosx,0,0,
                     0,0,1,0,
                     0,0,0,1);

}

// @returns the rotation matrix around the vector and point by the specified angle
Matrix4x4 getRotMat  (const Vector4 &p, const Vector4 &v, const REAL a) {
    REAL theta,phi,lambda;
    theta = atan2(v.z,v.x);
    phi = -atan2(v.y,sqrt(v.x*v.x+v.z*v.z));
    lambda = a;

    Matrix4x4 M1,M2,M3,M1i,M2i;
    M1i = getInvRotYMat(theta);
    M2i = getInvRotZMat(phi);
    M1 = getRotYMat(theta);
    M2 = getRotZMat(phi);
    M3 = getRotXMat(lambda);
    return getTransMat(p)*(M1i*M2i*M3*M2*M1)*getInvTransMat(p);


}


// @returns the inverse scale matrix described by the vector
Matrix4x4 getInvScaleMat(const Vector4 &v) {

    return  Matrix4x4(1.0/v.x,0,0,0,
                      0,1.0/v.y,0,0,
                      0,0,1.0/v.z,0,
                      0,0,0,1);

}

// @returns the inverse translation matrix described by the vector
Matrix4x4 getInvTransMat(const Vector4 &v) {

    return Matrix4x4(1,0,0,-v.x,
                     0,1,0,-v.y,
                     0,0,1,-v.z,
                     0,0,0,1);

}

// @returns the inverse rotation matrix about the x axis by the specified angle
Matrix4x4 getInvRotXMat (const REAL radians) {

    REAL cosx = cos(radians);
    REAL sinx = sin(radians);
    return Matrix4x4(1,0,0,0,
                     0,cosx,sinx,0,
                     0,-sinx,cosx,0,
                     0,0,0,1);

}

// @returns the inverse rotation matrix about the y axis by the specified angle
Matrix4x4 getInvRotYMat (const REAL radians) {

    REAL cosx = cos(radians);
    REAL sinx = sin(radians);
    return Matrix4x4(cosx,0,-sinx,0,
                     0,1,0,0,
                     sinx,0,cosx,0,
                     0,0,0,1);

}

// @returns the inverse rotation matrix about the z axis by the specified angle
Matrix4x4 getInvRotZMat (const REAL radians) {

    REAL cosx = cos(radians);
    REAL sinx = sin(radians);
    return Matrix4x4(cosx,sinx,0,0,
                     -sinx,cosx,0,0,
                     0,0,1,0,
                     0,0,0,1);


}

// @returns the inverse rotation matrix around the vector and point by the specified angle
Matrix4x4 getInvRotMat  (const Vector4 &p, const Vector4 &v, const REAL a) {

    REAL theta,phi,lambda;
    theta = atan2(v.z,v.x);
    phi = -atan2(v.y,sqrt(v.x*v.x+v.z*v.z));
    lambda = a;
    Matrix4x4 M1,M2,M3i,M1i,M2i;
    M1 = getRotYMat(theta);
    M2 = getRotZMat(phi);
    M1i = getInvRotYMat(theta);
    M2i = getInvRotZMat(phi);
    M3i = getInvRotXMat(lambda);
    return getTransMat(p)*(M1i*M2i*M3i*M2*M1)*getInvTransMat(p);
}


//@}---------------------------------------------------------------------

