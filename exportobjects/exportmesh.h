//-------------------------------------------------------------------------------------
// ExportMesh.h
//
// Classes representing static and skinned meshes.  Also included is code to optimize
// mesh data and generate export-ready data from a non-indexed triangle list.
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

namespace ATG
{

class ExportVB
{
public:
    ExportVB() 
        : m_uVertexCount( 0 ),
          m_uVertexSizeBytes( 0 )
    {
    }
    ~ExportVB()
    {
    }

    void SetVertexSize( DWORD uByteCount ) { m_uVertexSizeBytes = uByteCount; }
    DWORD GetVertexSize() const { return m_uVertexSizeBytes; }

    void SetVertexCount( size_t uVertexCount ) { m_uVertexCount = uVertexCount; }
    size_t GetVertexCount() const { return m_uVertexCount; }

    void Allocate();

    uint8_t* GetVertex( size_t uIndex );
    const uint8_t* GetVertex( size_t uIndex ) const;

    uint8_t* GetVertexData() { return m_pVertexData.get(); }
    const uint8_t* GetVertexData() const { return m_pVertexData.get(); }
    size_t GetVertexDataSize() const { return m_uVertexSizeBytes * m_uVertexCount; }

    void ByteSwap( const D3DVERTEXELEMENT9* pVertexElements, const size_t dwVertexElementCount );

protected:
    DWORD                       m_uVertexSizeBytes;
    size_t                      m_uVertexCount;
    std::unique_ptr<uint8_t[]>  m_pVertexData;
};

class ExportIB
{
public:
    ExportIB()
        : m_uIndexCount( 0 ),
          m_dwIndexSize( 2 )
    {
    }
    ~ExportIB()
    {
    }

    void SetIndexSize( DWORD dwIndexSize ) { assert( dwIndexSize == 2 || dwIndexSize == 4 ); m_dwIndexSize = dwIndexSize; }
    DWORD GetIndexSize() const { return m_dwIndexSize; }

    void SetIndexCount( size_t uIndexCount ) { m_uIndexCount = uIndexCount; }
    size_t GetIndexCount() const { return m_uIndexCount; }

    void Allocate();

    DWORD GetIndex( size_t uIndex ) const
    {
        if( m_dwIndexSize == 2 )
        {
            auto pIndexData16 = reinterpret_cast<const WORD*>( m_pIndexData.get() );
            return pIndexData16[ uIndex ];
        }
        else
        {
            auto pIndexData32 = reinterpret_cast<const DWORD*>( m_pIndexData.get() );
            return pIndexData32[ uIndex ];
        }
    }
    void SetIndex( size_t uIndex, DWORD dwData ) 
    {
        if( m_dwIndexSize == 2 )
        {
            auto pIndexData16 = reinterpret_cast<WORD*>( m_pIndexData.get() );
            pIndexData16[ uIndex ] = static_cast<WORD>( dwData );
        }
        else
        {
            auto pIndexData32 = reinterpret_cast<DWORD*>( m_pIndexData.get() );
            pIndexData32[ uIndex ] = dwData;
        }
    }
    uint8_t* GetIndexData() { return m_pIndexData.get(); }
    const uint8_t* GetIndexData() const { return m_pIndexData.get(); }
    size_t GetIndexDataSize() const { return m_uIndexCount * m_dwIndexSize; }

    void ByteSwap();

protected:
    DWORD                       m_dwIndexSize;
    size_t                      m_uIndexCount;
    std::unique_ptr<uint8_t[]>  m_pIndexData;
};

class ExportIBSubset : 
    public ExportBase
{
public:
    enum PrimitiveType
    {
        TriangleList = 0,
        TriangleStrip,
        QuadList
    };
    ExportIBSubset()
        : m_uStartIndex( 0 ),
          m_uIndexCount( 0 ),
          m_PrimitiveType( TriangleList )
    {
    }
    void SetStartIndex( UINT uStartIndex ) { m_uStartIndex = uStartIndex; }
    void IncrementIndexCount( UINT uSize ) { m_uIndexCount += uSize; }
    void SetIndexCount( UINT uIndexCount ) { m_uIndexCount = uIndexCount; }
    UINT GetStartIndex() const { return m_uStartIndex; }
    UINT GetIndexCount() const { return m_uIndexCount; }
    void SetPrimitiveType( PrimitiveType NewPT ) { m_PrimitiveType = NewPT; }
    PrimitiveType GetPrimitiveType() const { return m_PrimitiveType; }
protected:
    UINT            m_uStartIndex;
    UINT            m_uIndexCount;
    PrimitiveType   m_PrimitiveType;
};

struct ByteVector4
{
public:
    BYTE    x;
    BYTE    y;
    BYTE    z;
    BYTE    w;
};

class ExportMaterial;

struct ExportMeshVertex
{
public:
    ExportMeshVertex()
    {
        Initialize();
    }
    void Initialize()
    {
        ZeroMemory( this, sizeof( ExportMeshVertex ) );
        BoneWeights.x = 1.0f;
    }
    UINT            DCCVertexIndex;
    D3DXVECTOR3     Position;
    D3DXVECTOR3     Normal;
    D3DXVECTOR3     SmoothNormal;
    D3DXVECTOR3     Tangent;
    D3DXVECTOR3     Binormal;
    ByteVector4     BoneIndices;
    D3DXVECTOR4     BoneWeights;
    D3DXVECTOR4     TexCoords[8];
    D3DXVECTOR4     Color;
    ExportMeshVertex* pNextDuplicateVertex;
    bool            Equals( const ExportMeshVertex* pOtherVertex ) const;
};

typedef std::vector< ExportMeshVertex* > ExportMeshVertexArray;

struct ExportMeshTriangle
{
public:
    ExportMeshTriangle()
        : SubsetIndex( 0 ),
          PolygonIndex( -1 )
    {
    }
    void Initialize()
    {
        SubsetIndex = 0;
        Vertex[0].Initialize();
        Vertex[1].Initialize();
        Vertex[2].Initialize();
    }
    ExportMeshVertex    Vertex[3];
    INT                 SubsetIndex;
    INT                 PolygonIndex;
};

typedef std::vector< ExportMeshTriangle* > ExportMeshTriangleArray;

class ExportMeshTriangleAllocator
{
public:
    ExportMeshTriangleAllocator()
        : m_uAllocatedCount( 0 ),
          m_uTotalCount( 0 )
    {
    }
    ~ExportMeshTriangleAllocator()
    {
        Terminate();
    }
    void Initialize() { SetSizeHint( 50000 ); }
    void Terminate();
    void SetSizeHint( UINT uAnticipatedSize );
    ExportMeshTriangle* GetNewTriangle();
    void ClearAllTriangles();
private:
    struct AllocationBlock
    {
        ExportMeshTriangle* pTriangleArray;
        UINT m_uTriangleCount;
    };
    typedef std::list< AllocationBlock > AllocationBlockList;
    AllocationBlockList m_AllocationBlocks;
    UINT m_uTotalCount;
    UINT m_uAllocatedCount;
};

extern ExportMeshTriangleAllocator g_MeshTriangleAllocator;

struct ExportVertexFormat
{
public:
    ExportVertexFormat()
        : m_bPosition( true ),
          m_bNormal( true ),
          m_bSkinData( false ),
          m_bTangent( false ),
          m_bBinormal( false ),
          m_bVertexColor( true ),
          m_uUVSetCount( 1 ),
          m_uUVSetSize( 2 )
    {
    }
    bool        m_bPosition;
    bool        m_bNormal;
    bool        m_bTangent;
    bool        m_bBinormal;
    bool        m_bSkinData;
    bool        m_bVertexColor;
    UINT        m_uUVSetCount;
    UINT        m_uUVSetSize;
};


class ExportMeshBase :
    public ExportBase
{
public:
    ExportMeshBase( ExportString name );
    ~ExportMeshBase();

    enum MeshType
    {
        PolyMesh = 0
    };

    enum BoundsType
    {
        SphereBound = 0,
        AxisAlignedBoxBound = 1,
        OrientedBoxBound = 2
    };

    virtual MeshType GetMeshType() const = 0;

    void AddSubset( ExportIBSubset* pSubset ) { m_vSubsets.push_back( pSubset ); }
    size_t GetSubsetCount() const { return m_vSubsets.size(); }
    ExportIBSubset* GetSubset( size_t uIndex ) { return m_vSubsets[ uIndex ]; }
    ExportIBSubset* FindSubset( const ExportString Name );

    Sphere& GetBoundingSphere() { return m_BoundingSphere; }
    AxisAlignedBox& GetBoundingAABB() { return m_BoundingAABB; }
    OrientedBox& GetBoundingOBB() { return m_BoundingOBB; }
    BoundsType GetSmallestBound() const { return m_SmallestBound; }

    virtual void AddInfluence( ExportString InfluenceName ) { m_InfluenceNames.push_back( InfluenceName ); }
    size_t GetInfluenceCount() const { return m_InfluenceNames.size(); }
    ExportString GetInfluence( size_t uIndex ) const { return m_InfluenceNames[uIndex]; }

protected:
    Sphere                          m_BoundingSphere;
    AxisAlignedBox                  m_BoundingAABB;
    OrientedBox                     m_BoundingOBB;
    BoundsType                      m_SmallestBound;
    std::vector< ExportIBSubset* >  m_vSubsets;
    std::vector< ExportString >     m_InfluenceNames;
};


class ExportSubDProcessMesh;

class ExportMesh :
    public ExportMeshBase
{
public:
    enum OptimizationFlags
    {
        COMPRESS_VERTEX_DATA = 1,
        FLIP_TRIANGLES = 2,
        FORCE_SUBD_CONVERSION = 4,
    };

    ExportMesh( ExportString name );
    ~ExportMesh();

    virtual MeshType GetMeshType() const override { return ExportMeshBase::PolyMesh; }

    void SetVertexUVCount( UINT uCount ) { m_VertexFormat.m_uUVSetCount = uCount; }
    void SetVertexUVDimension( UINT uDimension ) { m_VertexFormat.m_uUVSetSize = uDimension; }
    void SetVertexNormalCount( UINT uCount );
    void SetVertexColorCount( UINT uCount ) { m_VertexFormat.m_bVertexColor = ( uCount > 0 ); }

    size_t GetVertexDeclElementCount() const { return m_VertexElements.size(); }
    const D3DVERTEXELEMENT9& GetVertexDeclElement( size_t uIndex ) const { return m_VertexElements[ uIndex ]; }

    void AddRawTriangle( ExportMeshTriangle* pTriangle );
    void Optimize( DWORD dwFlags );
    void ByteSwap();

    ExportVB* GetVB() { return m_pVB; }
    ExportIB* GetIB() { return m_pIB; }

    ExportSubDProcessMesh* GetSubDMesh() { return m_pSubDMesh; }

    size_t GetTriangleCount() const { return m_TriangleToPolygonMapping.size(); }
    INT GetPolygonForTriangle( size_t dwTriangleIndex ) const { return m_TriangleToPolygonMapping[dwTriangleIndex]; }

    virtual void AddInfluence( ExportString InfluenceName ) override { m_InfluenceNames.push_back( InfluenceName ); m_VertexFormat.m_bSkinData = true; }

protected:
    void BuildVertexBuffer( ExportMeshVertexArray& VertexArray, DWORD dwFlags );
    void ClearRawTriangles();
    ID3DXMesh* CreateD3DXMesh();
    void CopyD3DXMeshIntoMesh( ID3DXMesh* pMesh );
    HRESULT ComputeVertexTangentSpacesD3DX( ID3DXMesh** ppMesh );
    HRESULT ComputeUVAtlas( ID3DXMesh** ppMesh );
    void ComputeBoneSubsetGroups();
    void SortRawTrianglesBySubsetIndex();
    void ComputeBounds();

protected:
    ExportVB*                           m_pVB;
    ExportIB*                           m_pIB;
    ExportMeshTriangleArray             m_RawTriangles;
    std::vector< INT >                  m_TriangleToPolygonMapping;
    ExportVertexFormat                  m_VertexFormat;
    std::vector< D3DVERTEXELEMENT9 >    m_VertexElements;
    UINT                                m_uDCCVertexCount;
    ExportSubDProcessMesh*              m_pSubDMesh;
};

class ExportMaterialSubsetBinding
{
public:
    ExportMaterialSubsetBinding()
    {
        ZeroMemory( this, sizeof( ExportMaterialSubsetBinding ) );
    }
    ExportString        SubsetName;
    ExportMaterial*     pMaterial;
};

typedef std::vector< ExportMaterialSubsetBinding* > ExportMaterialSubsetBindingArray;

class ExportModel
{
public:
    ExportModel( ExportMeshBase* pMesh )
        : m_pMesh( pMesh ),
          m_bCastsShadows( true ),
          m_bReceivesShadows( true )
    {
    }
    ~ExportModel();
    ExportMeshBase* GetMesh() { return m_pMesh; }
    bool SetSubsetBinding( ExportString SubsetName, ExportMaterial* pMaterial, bool bSkipValidation = false );
    size_t GetBindingCount() const { return m_vBindings.size(); }
    ExportMaterialSubsetBinding* GetBinding( size_t i ) { return m_vBindings[i]; }

    bool IsShadowCaster() const { return m_bCastsShadows; }
    bool IsShadowReceiver() const { return m_bReceivesShadows; }
    void SetCastsShadows( bool bValue ) { m_bCastsShadows = bValue; }
    void SetReceivesShadows( bool bValue ) { m_bReceivesShadows = bValue; }

protected:
    ExportMeshBase*                     m_pMesh;
    ExportMaterialSubsetBindingArray    m_vBindings;
    bool                                m_bCastsShadows;
    bool                                m_bReceivesShadows;
};

DWORD MakeCompressedVector( const D3DXVECTOR3& Vec3 );
void NormalizeBoneWeights( BYTE* pWeights );

};

