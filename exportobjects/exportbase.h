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
    bool Initialize( CXMMATRIX matrix );
    bool Initialize( XMFLOAT4X4 matrix );
    const XMFLOAT4X4& Matrix() const { return m_Matrix; }
    XMFLOAT4X4& Matrix() { return m_Matrix; }
    void Multiply( const XMFLOAT4X4& Matrix );
    void Normalize();
    const XMFLOAT3& Position() const { return m_Position; }
    const XMFLOAT4& Orientation() const { return m_Orientation; }
    const XMFLOAT3& Scale() const { return m_Scale; }

protected:
    bool DecomposeMatrix();

protected:
    XMFLOAT4X4 m_Matrix;
    XMFLOAT3   m_Position;
    XMFLOAT4   m_Orientation;
    XMFLOAT3   m_Scale;
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
