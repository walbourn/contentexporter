//-------------------------------------------------------------------------------------
// FBXImportMain.h
//
// Entry points for FBX scene parsing.
//
// Advanced Technology Group (ATG)
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------
#pragma once

class FBXTransformer : public ATG::IDCCTransformer
{
public:
    FBXTransformer()
        : m_fUnitScale(1.0f),
        m_bMaxConversion(false),
        m_bFlipZ(true)
    {}

    void Initialize(FbxScene* pScene);

    void TransformMatrix(DirectX::XMFLOAT4X4* pDestMatrix, const DirectX::XMFLOAT4X4* pSrcMatrix) const override;
    void TransformPosition(DirectX::XMFLOAT3* pDestPosition, const DirectX::XMFLOAT3* pSrcPosition) const override;
    void TransformDirection(DirectX::XMFLOAT3* pDestDirection, const DirectX::XMFLOAT3* pSrcDirection) const override;
    float TransformLength(float fInputLength) const override;

    // Sets unit scale for exporting all geometry - works with characters too.
    void SetUnitScale(const float fScale)
    {
        m_fUnitScale = fScale;
    }

    void SetZFlip(const bool bFlip)
    {
        m_bFlipZ = bFlip;
    }

protected:
    float m_fUnitScale;
    bool  m_bMaxConversion;
    bool  m_bFlipZ;
};

class FBXImport
{
public:
    static HRESULT Initialize();
    static void ClearScene();

    static HRESULT ImportFile(const CHAR* strFileName);

private:
};
