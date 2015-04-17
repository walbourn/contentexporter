//-------------------------------------------------------------------------------------
// ExportBase.h
//
// Base functionality for all of the export objects.
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
#pragma once

#include "ExportString.h"

namespace ATG
{

class ExportTransform
{
public:
    void SetIdentity();
    bool InitializeFromFloatsTransposed( float* pSixteenFloats );
    bool InitializeFromFloats( float* pSixteenFloats );
    const D3DXMATRIX& Matrix() const { return m_Matrix; }
    D3DXMATRIX& Matrix() { return m_Matrix; }
    void Multiply( const D3DXMATRIX& Matrix );
    void Normalize();
    const D3DXVECTOR3& Position() const { return m_Position; }
    const D3DXQUATERNION& Orientation() const { return m_Orientation; }
    const D3DXVECTOR3& Scale() const { return m_Scale; }

protected:
    bool DecomposeMatrix();

protected:
    D3DXMATRIX      m_Matrix;
    D3DXVECTOR3     m_Position;
    D3DXQUATERNION  m_Orientation;
    D3DXVECTOR3     m_Scale;
};

class ExportBase
{
public:
    ExportBase() : m_pDCCObject(nullptr) {}
    ExportBase( ExportString name ) : m_Name( name ), m_pDCCObject(nullptr) {}
    virtual ~ExportBase();

    ExportString GetName() const { return m_Name; }
    void SetName( ExportString newName ) { m_Name = newName; }

    void SetDCCObject( void* pDCCObject ) { m_pDCCObject = pDCCObject; }
    void* GetDCCObject() const { return m_pDCCObject; }
protected:
    ExportString        m_Name;
    void*               m_pDCCObject;
};

};
