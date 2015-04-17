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

class FBXTransformer : public ATG::IDCCTransformer
{
public:
    FBXTransformer()
        : m_fUnitScale( 1.0f )
    { }

    void Initialize( FbxScene* pScene );

    virtual void TransformMatrix( D3DXMATRIX* pDestMatrix, const D3DXMATRIX* pSrcMatrix ) const override;
    virtual void TransformPosition( D3DXVECTOR3* pDestPosition, const D3DXVECTOR3* pSrcPosition ) const override;
    virtual void TransformDirection( D3DXVECTOR3* pDestDirection, const D3DXVECTOR3* pSrcDirection ) const override;
    virtual float TransformLength( float fInputLength ) const override;

    // Sets unit scale for exporting all geometry - works with characters too.
    void SetUnitScale( const float fScale )
    {
        m_fUnitScale = fScale;
    }

protected:
    float m_fUnitScale;
    bool  m_bMaxConversion;
};

class FBXImport
{
public:
    static HRESULT Initialize();
    static void ClearScene();

    static HRESULT ImportFile( const CHAR* strFileName );

private:
};
