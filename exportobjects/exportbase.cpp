//-------------------------------------------------------------------------------------
// ExportBase.cpp
//
// Advanced Technology Group (ATG)
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------

#include "stdafx.h"
#include "exportbase.h"
#include "ExportScene.h"

using namespace DirectX;
using namespace ATG;

extern ATG::ExportScene* g_pScene;

void ExportTransform::SetIdentity()
{
    XMStoreFloat4x4(&m_Matrix, XMMatrixIdentity());
    m_Position = XMFLOAT3(0, 0, 0);
    XMStoreFloat4(&m_Orientation, XMQuaternionIdentity());
    m_Scale = XMFLOAT3(1, 1, 1);
}

bool ExportTransform::Initialize(CXMMATRIX matrix)
{
    XMStoreFloat4x4(&m_Matrix, matrix);

    g_pScene->GetDCCTransformer()->TransformMatrix(&m_Matrix, &m_Matrix);
    return DecomposeMatrix();
}

bool ExportTransform::Initialize(XMFLOAT4X4 matrix)
{
    m_Matrix = matrix;
    g_pScene->GetDCCTransformer()->TransformMatrix(&m_Matrix, &m_Matrix);
    return DecomposeMatrix();
}

bool ExportTransform::DecomposeMatrix()
{
    const XMMATRIX matrix = XMLoadFloat4x4(&m_Matrix);

    const float fScaleX = XMVectorGetX(XMVector3LengthSq(matrix.r[0]));
    const float fScaleY = XMVectorGetX(XMVector3LengthSq(matrix.r[1]));
    const float fScaleZ = XMVectorGetX(XMVector3LengthSq(matrix.r[2]));

    const float fDiffXY = fabs(fScaleX - fScaleY);
    const float fDiffYZ = fabs(fScaleY - fScaleZ);
    const float fDiffXZ = fabs(fScaleX - fScaleZ);
    bool bUniformScale = true;
    if (fDiffXY > 0.001f || fDiffYZ > 0.001f || fDiffXZ > 0.001f)
        bUniformScale = false;

    XMVECTOR scale, rot, trans;
    XMMatrixDecompose(&scale, &rot, &trans, matrix);

    XMStoreFloat3(&m_Scale, scale);
    XMStoreFloat4(&m_Orientation, rot);
    XMStoreFloat3(&m_Position, trans);

    return bUniformScale;
}

void ExportTransform::Multiply(const XMFLOAT4X4& Matrix)
{
    XMMATRIX m = XMLoadFloat4x4(&m_Matrix);
    const XMMATRIX m1 = XMLoadFloat4x4(&Matrix);

    m = XMMatrixMultiply(m, m1);
    XMStoreFloat4x4(&m_Matrix, m);
}

void ExportTransform::Normalize()
{
    XMMATRIX m = XMLoadFloat4x4(&m_Matrix);
    m = XMMatrixTranspose(m);

    XMVECTOR vec = XMVector3Normalize(m.r[0]);
    m.r[0] = XMVectorSelect(m.r[0], vec, g_XMSelect1110);

    vec = XMVector3Normalize(m.r[1]);
    m.r[1] = XMVectorSelect(m.r[1], vec, g_XMSelect1110);

    vec = XMVector3Normalize(m.r[2]);
    m.r[2] = XMVectorSelect(m.r[2], vec, g_XMSelect1110);

    m = XMMatrixTranspose(m);
    XMStoreFloat4x4(&m_Matrix, m);
}

ExportBase::~ExportBase()
{
}
