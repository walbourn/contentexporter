//-------------------------------------------------------------------------------------
// FBXImportMain.h
//
// Entry points for FBX scene parsing.
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

using namespace ATG;

class FBXTransformer : public IDCCTransformer
{
public:
    FBXTransformer()
        : m_fUnitScale( 1.0f )
    { }

    VOID Initialize( FbxScene* pScene );

    virtual VOID TransformMatrix( D3DXMATRIX* pDestMatrix, CONST D3DXMATRIX* pSrcMatrix ) CONST;
    virtual VOID TransformPosition( D3DXVECTOR3* pDestPosition, CONST D3DXVECTOR3* pSrcPosition ) CONST;
    virtual VOID TransformDirection( D3DXVECTOR3* pDestDirection, CONST D3DXVECTOR3* pSrcDirection ) CONST;
    virtual FLOAT TransformLength( FLOAT fInputLength ) CONST;

    // Sets unit scale for exporting all geometry - works with characters too.
    void SetUnitScale( const FLOAT fScale )
    {
        m_fUnitScale = fScale;
    }

protected:
    FLOAT m_fUnitScale;
    BOOL  m_bMaxConversion;
};

class FBXImport
{
public:
    static HRESULT Initialize();
    static VOID ClearScene();

    static HRESULT ImportFile( const CHAR* strFileName );

private:
};
