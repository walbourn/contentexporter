//-------------------------------------------------------------------------------------
// ExportBase.cpp
//  
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//  
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------

#include "stdafx.h"
#include "exportbase.h"
#include "ExportScene.h"

extern ATG::ExportScene* g_pScene;

namespace ATG
{

void ExportTransform::SetIdentity()
{
    D3DXMatrixIdentity( &m_Matrix );
    m_Position = D3DXVECTOR3( 0, 0, 0 );
    D3DXQuaternionIdentity( &m_Orientation );
    m_Scale = D3DXVECTOR3( 1, 1, 1 );
}

bool ExportTransform::InitializeFromFloatsTransposed( float* pSixteenFloats )
{
    m_Matrix = D3DXMATRIX( pSixteenFloats );
    D3DXMatrixTranspose( &m_Matrix, &m_Matrix );
    g_pScene->GetDCCTransformer()->TransformMatrix( &m_Matrix, &m_Matrix );
    return DecomposeMatrix();
}

bool ExportTransform::InitializeFromFloats( float* pSixteenFloats )
{
    m_Matrix = D3DXMATRIX( pSixteenFloats );
    g_pScene->GetDCCTransformer()->TransformMatrix( &m_Matrix, &m_Matrix );
    return DecomposeMatrix();
}

bool ExportTransform::DecomposeMatrix()
{
    D3DXVECTOR3 vAxisX( m_Matrix._11, m_Matrix._12, m_Matrix._13 );
    D3DXVECTOR3 vAxisY( m_Matrix._21, m_Matrix._22, m_Matrix._23 );
    D3DXVECTOR3 vAxisZ( m_Matrix._31, m_Matrix._32, m_Matrix._33 );
    float fScaleX = D3DXVec3LengthSq( &vAxisX );
    float fScaleY = D3DXVec3LengthSq( &vAxisY );
    float fScaleZ = D3DXVec3LengthSq( &vAxisZ );
    float fDiffXY = fabs( fScaleX - fScaleY );
    float fDiffYZ = fabs( fScaleY - fScaleZ );
    float fDiffXZ = fabs( fScaleX - fScaleZ );
    bool bUniformScale = true;
    if( fDiffXY > 0.001f || fDiffYZ > 0.001f || fDiffXZ > 0.001f )
        bUniformScale = false;
    D3DXMatrixDecompose( &m_Scale, &m_Orientation, &m_Position, &m_Matrix );
    return bUniformScale;
}

void ExportTransform::Multiply( const D3DXMATRIX& Matrix )
{
    D3DXMatrixMultiply( &m_Matrix, &m_Matrix, &Matrix );
}

void ExportTransform::Normalize()
{
    D3DXVECTOR3 Vec( m_Matrix._11, m_Matrix._21, m_Matrix._31 );
    D3DXVec3Normalize( &Vec, &Vec );
    m_Matrix._11 = Vec.x;
    m_Matrix._21 = Vec.y;
    m_Matrix._31 = Vec.z;

    Vec = D3DXVECTOR3( m_Matrix._12, m_Matrix._22, m_Matrix._32 );
    D3DXVec3Normalize( &Vec, &Vec );
    m_Matrix._12 = Vec.x;
    m_Matrix._22 = Vec.y;
    m_Matrix._32 = Vec.z;

    Vec = D3DXVECTOR3( m_Matrix._13, m_Matrix._23, m_Matrix._33 );
    D3DXVec3Normalize( &Vec, &Vec );
    m_Matrix._13 = Vec.x;
    m_Matrix._23 = Vec.y;
    m_Matrix._33 = Vec.z;
}

ExportBase::~ExportBase()
{
}

};
