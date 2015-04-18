//-------------------------------------------------------------------------------------
// ExportMesh.cpp
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
#include "exportmesh.h"

#define SAFE_RELEASE(x) { if( (x) != nullptr ) { (x)->Release(); (x) = nullptr; } }

extern ATG::ExportScene* g_pScene;

namespace ATG
{

INT GetElementSizeFromDeclType(DWORD Type);

ExportMeshTriangleAllocator g_MeshTriangleAllocator;

ExportMeshBase::ExportMeshBase( ExportString name )
: ExportBase( name )
{ }

ExportMeshBase::~ExportMeshBase()
{ }

ExportIBSubset* ExportMeshBase::FindSubset( const ExportString Name )
{
    size_t dwSubsetCount = GetSubsetCount();
    for( size_t i = 0; i < dwSubsetCount; ++i )
    {
        ExportIBSubset* pSubset = GetSubset( i );
        if( pSubset->GetName() == Name )
            return pSubset;
    }
    return nullptr;
}

ExportMesh::ExportMesh( ExportString name )
: ExportMeshBase( name ),
  m_pVB( nullptr ),
  m_pIB( nullptr ),
  m_uDCCVertexCount( 0 ),
  m_pSubDMesh( nullptr )
{
    m_BoundingSphere.Center = XMFLOAT3( 0, 0, 0 );
    m_BoundingSphere.Radius = 0;
}

ExportMesh::~ExportMesh()
{
    delete m_pVB;
    delete m_pIB;
    ClearRawTriangles();
}

void ExportMesh::ClearRawTriangles()
{
    if( m_RawTriangles.size() > 0 )
    {
        g_MeshTriangleAllocator.ClearAllTriangles();
        m_RawTriangles.clear();
    }
}

void ExportMesh::ByteSwap()
{
    if( m_pIB )
        m_pIB->ByteSwap();
    if( m_pVB && m_VertexElements.size() > 0 )
        m_pVB->ByteSwap( &m_VertexElements[0], GetVertexDeclElementCount() );
    if( m_pSubDMesh )
    {
        m_pSubDMesh->ByteSwap();
    }
}

void ExportVB::Allocate()
{
    size_t uSize = GetVertexDataSize();
    m_pVertexData.reset( new uint8_t[ uSize ] );
    ZeroMemory( m_pVertexData.get(), uSize );
}

uint8_t* ExportVB::GetVertex( size_t uIndex )
{
    if( !m_pVertexData )
        return nullptr;
    if( uIndex >= m_uVertexCount )
        return nullptr;
    return m_pVertexData.get() + ( uIndex * m_uVertexSizeBytes );
}

const uint8_t* ExportVB::GetVertex( size_t uIndex ) const
{
    if( !m_pVertexData )
        return nullptr;
    if( uIndex >= m_uVertexCount )
        return nullptr;
    return m_pVertexData.get() + ( uIndex * m_uVertexSizeBytes );
}

void ExportVB::ByteSwap( const D3DVERTEXELEMENT9* pVertexElements, const size_t dwVertexElementCount )
{
    for( size_t dwVertexIndex = 0; dwVertexIndex < m_uVertexCount; dwVertexIndex++ )
    {
        auto pVB = GetVertex( dwVertexIndex );
        for( size_t i = 0; i < dwVertexElementCount; i++ )
        {
            auto pElement = reinterpret_cast<DWORD*>( pVB + pVertexElements[i].Offset );
            switch( pVertexElements[i].Type )
            {
            case D3DDECLTYPE_FLOAT4:
                *pElement = _byteswap_ulong( *pElement );
                pElement++;
            case D3DDECLTYPE_FLOAT3:
                *pElement = _byteswap_ulong( *pElement );
                pElement++;
            case D3DDECLTYPE_FLOAT2:
                *pElement = _byteswap_ulong( *pElement );
                pElement++;
            case D3DDECLTYPE_FLOAT1:
            case D3DDECLTYPE_D3DCOLOR:
            case D3DDECLTYPE_UBYTE4:
            case D3DDECLTYPE_UBYTE4N:
            case D3DDECLTYPE_DEC3N:
            case D3DDECLTYPE_UDEC3:
                *pElement = _byteswap_ulong( *pElement );
                break;
            case D3DDECLTYPE_SHORT4:
            case D3DDECLTYPE_SHORT4N:
            case D3DDECLTYPE_USHORT4N:
            case D3DDECLTYPE_FLOAT16_4:
                {
                    auto pWord = reinterpret_cast<WORD*>( pElement );
                    *pWord = _byteswap_ushort( *pWord );
                    pWord++;
                    *pWord = _byteswap_ushort( *pWord );
                    pElement++;
                }
            case D3DDECLTYPE_SHORT2:
            case D3DDECLTYPE_SHORT2N:
            case D3DDECLTYPE_USHORT2N:
            case D3DDECLTYPE_FLOAT16_2:
                {
                    auto pWord = reinterpret_cast<WORD*>( pElement );
                    *pWord = _byteswap_ushort( *pWord );
                    pWord++;
                    *pWord = _byteswap_ushort( *pWord );
                    pElement++;
                    break;
                }
            }
        }
    }
}

void ExportIB::ByteSwap()
{
    if( m_dwIndexSize == 2 )
    {
        auto pIndexData16 = reinterpret_cast<WORD*>( m_pIndexData.get() );
        for( size_t i = 0; i < m_uIndexCount; i++ )
        {
            WORD wIndex = _byteswap_ushort( pIndexData16[ i ] );
            pIndexData16[ i ] = wIndex;
        }
    }
    else
    {
        auto pIndexData32 = reinterpret_cast<DWORD*>( m_pIndexData.get() );
        for( size_t i = 0; i < m_uIndexCount; i++ )
        {
            DWORD dwIndex = _byteswap_ulong( pIndexData32[ i ] );
            pIndexData32[ i ] = dwIndex;
        }
    }
}

void ExportIB::Allocate()
{
    if( m_dwIndexSize == 2 )
    {
        m_pIndexData.reset( reinterpret_cast<uint8_t*>( new WORD[ m_uIndexCount ] ) );
        ZeroMemory( m_pIndexData.get(), m_uIndexCount * sizeof( WORD ) );
    }
    else
    {
        m_pIndexData.reset( reinterpret_cast<uint8_t*>( new DWORD[ m_uIndexCount ] ) );
        ZeroMemory( m_pIndexData.get(), m_uIndexCount * sizeof( DWORD ) );
    }
}

void ExportMeshTriangleAllocator::Terminate()
{
    AllocationBlockList::iterator iter = m_AllocationBlocks.begin();
    AllocationBlockList::iterator end = m_AllocationBlocks.end();
    while( iter != end )
    {
        AllocationBlock& block = *iter;
        delete[] block.pTriangleArray;
        ++iter;
    }
    m_AllocationBlocks.clear();
    m_uTotalCount = 0;
    m_uAllocatedCount = 0;
}

void ExportMeshTriangleAllocator::SetSizeHint( UINT uAnticipatedSize )
{
    if( uAnticipatedSize <= m_uTotalCount )
        return;
    UINT uNewCount = std::max<UINT>( uAnticipatedSize - m_uTotalCount, 10000 );
    AllocationBlock NewBlock;
    NewBlock.m_uTriangleCount = uNewCount;
    NewBlock.pTriangleArray = new ExportMeshTriangle[ uNewCount ];
    m_AllocationBlocks.push_back( NewBlock );
    m_uTotalCount += uNewCount;
}

ExportMeshTriangle* ExportMeshTriangleAllocator::GetNewTriangle()
{
    if( m_uAllocatedCount == m_uTotalCount )
        SetSizeHint( m_uTotalCount + 10000 );
    UINT uIndex = m_uAllocatedCount;
    m_uAllocatedCount++;
    AllocationBlockList::iterator iter = m_AllocationBlocks.begin();
    AllocationBlockList::iterator end = m_AllocationBlocks.end();
    while( iter != end )
    {
        AllocationBlock& block = *iter;
        if( uIndex < block.m_uTriangleCount )
        {
            ExportMeshTriangle* pTriangle = &block.pTriangleArray[ uIndex ];
            pTriangle->Initialize();
            return pTriangle;
        }
        uIndex -= block.m_uTriangleCount;
        ++iter;
    }
    assert( false );
    return nullptr;
}

void ExportMeshTriangleAllocator::ClearAllTriangles()
{
    m_uAllocatedCount = 0;
}

bool ExportMeshVertex::Equals( const ExportMeshVertex* pOtherVertex ) const
{
    if ( !pOtherVertex )
        return false;

    if( pOtherVertex == this )
        return true;

    XMVECTOR v0 = XMLoadFloat3( &Position );
    XMVECTOR v1 = XMLoadFloat3( &pOtherVertex->Position );
    if ( XMVector3NotEqual( v0, v1 ) )
        return false;

    v0 = XMLoadFloat3( &Normal );
    v1 = XMLoadFloat3( &pOtherVertex->Normal );
    if ( XMVector3NotEqual( v0, v1 ) )
        return false;

    for( size_t i = 0; i < 8; i++ )
    {
        v0 = XMLoadFloat4( &TexCoords[i] );
        v1 = XMLoadFloat4( &pOtherVertex->TexCoords[i] );
        if ( XMVector4NotEqual( v0, v1 ) )
            return false;
    }

    v0 = XMLoadFloat4( &Color );
    v1 = XMLoadFloat4( &pOtherVertex->Color );
    if ( XMVector4NotEqual( v0, v1 ) )
        return false;

    return true;
}

UINT FindOrAddVertex( ExportMeshVertexArray& ExistingVertexArray, ExportMeshVertex* pTestVertex )
{
    for( size_t i = 0; i < ExistingVertexArray.size(); i++ )
    {
        ExportMeshVertex* pVertex = ExistingVertexArray[i];
        if( pVertex->Equals( pTestVertex ) )
            return static_cast<UINT>( i );
    }
    UINT index = static_cast<UINT>( ExistingVertexArray.size() );
    ExistingVertexArray.push_back( pTestVertex );
    return index;
}

void ExportMesh::AddRawTriangle( ExportMeshTriangle* pTriangle )
{
    m_RawTriangles.push_back( pTriangle );
    m_uDCCVertexCount = std::max<UINT>( m_uDCCVertexCount, pTriangle->Vertex[0].DCCVertexIndex + 1 );
    m_uDCCVertexCount = std::max<UINT>( m_uDCCVertexCount, pTriangle->Vertex[1].DCCVertexIndex + 1 );
    m_uDCCVertexCount = std::max<UINT>( m_uDCCVertexCount, pTriangle->Vertex[2].DCCVertexIndex + 1 );
}

UINT FindOrAddVertexFast( ExportMeshVertexArray& ExistingVertexArray, ExportMeshVertex* pTestVertex )
{
    UINT uIndex = pTestVertex->DCCVertexIndex;
    assert( uIndex < ExistingVertexArray.size() );
    ExportMeshVertex* pVertex = ExistingVertexArray[ uIndex ];
    if( pVertex )
    {
        ExportMeshVertex* pLastVertex = nullptr;
        while( pVertex )
        {
            uIndex = pVertex->DCCVertexIndex;
            if( pVertex->Equals( pTestVertex ) )
                return uIndex;
            pLastVertex = pVertex;
            pVertex = pVertex->pNextDuplicateVertex;
        }
        assert( pVertex == nullptr );
        uIndex = static_cast<UINT>( ExistingVertexArray.size() );
        ExistingVertexArray.push_back( pTestVertex );
        pTestVertex->DCCVertexIndex = uIndex;
        if( pLastVertex )
        {
            pLastVertex->pNextDuplicateVertex = pTestVertex;
        }
    }
    else
    {
        ExistingVertexArray[ uIndex ] = pTestVertex;
    }
    return uIndex;
}

void ExportMesh::SetVertexNormalCount( UINT uCount )
{
    m_VertexFormat.m_bNormal = ( uCount > 0 );
    m_VertexFormat.m_bTangent = ( uCount > 1 );
    m_VertexFormat.m_bBinormal = ( uCount > 2 );
}

bool SubsetLess( ExportMeshTriangle* pA, ExportMeshTriangle* pB )
{
    return pA->SubsetIndex < pB->SubsetIndex;
}

void ExportMesh::SortRawTrianglesBySubsetIndex()
{
    if( m_RawTriangles.empty() )
        return;

    std::stable_sort( m_RawTriangles.begin(), m_RawTriangles.end(), SubsetLess );
}

void ExportMesh::Optimize( DWORD dwFlags )
{
    if( m_RawTriangles.empty() )
        return;

    ExportLog::LogMsg( 4, "Optimizing mesh \"%s\" with %Iu triangles.", GetName().SafeString(), m_RawTriangles.size() );
    
    SortRawTrianglesBySubsetIndex();

    ExportIBSubset* pCurrentIBSubset = nullptr;
    INT iCurrentSubsetIndex = -1;
    std::vector<UINT> IndexData;
    IndexData.reserve( m_RawTriangles.size() * 3 );
    ExportMeshVertexArray VertexData;
    VertexData.resize( m_uDCCVertexCount, nullptr );

    bool bFlipTriangles = g_pScene->Settings().bFlipTriangles;
    if( dwFlags & FLIP_TRIANGLES )
        bFlipTriangles = !bFlipTriangles;

    // loop through raw triangles
    const size_t dwTriangleCount = m_RawTriangles.size();
    m_TriangleToPolygonMapping.clear();
    m_TriangleToPolygonMapping.reserve( dwTriangleCount );

    for( size_t i = 0; i < dwTriangleCount; i++ )
    {
        ExportMeshTriangle* pTriangle = m_RawTriangles[i];
        // create a new subset if one is encountered
        // note: subset index will be monotonically increasing
        assert( pTriangle->SubsetIndex >= iCurrentSubsetIndex );
        if( pTriangle->SubsetIndex > iCurrentSubsetIndex )
        {
            pCurrentIBSubset = new ExportIBSubset();
            pCurrentIBSubset->SetName( "Default" );
            pCurrentIBSubset->SetStartIndex( static_cast<UINT>( IndexData.size() ) );
            m_vSubsets.push_back( pCurrentIBSubset );
            iCurrentSubsetIndex = pTriangle->SubsetIndex;
        }
        // collapse the triangle verts into the final vertex list
        // this removes unnecessary duplicates, and retains necessary duplicates
        UINT uIndexA = FindOrAddVertexFast( VertexData, &pTriangle->Vertex[0] );
        UINT uIndexB = FindOrAddVertexFast( VertexData, &pTriangle->Vertex[1] );
        UINT uIndexC = FindOrAddVertexFast( VertexData, &pTriangle->Vertex[2] );
        // record final indices into the index list
        IndexData.push_back( uIndexA );
        if( bFlipTriangles )
        {
            IndexData.push_back( uIndexC );
            IndexData.push_back( uIndexB );
        }
        else
        {
            IndexData.push_back( uIndexB );
            IndexData.push_back( uIndexC );
        }
        m_TriangleToPolygonMapping.push_back( pTriangle->PolygonIndex );
        pCurrentIBSubset->IncrementIndexCount( 3 );
    }
    ExportLog::LogMsg( 3, "Triangle list mesh: %Iu verts, %Iu indices, %Iu subsets", VertexData.size(), IndexData.size(), m_vSubsets.size() );
    if( VertexData.size() > 16777215 )
    {
        ExportLog::LogError( "Mesh \"%s\" has more than 16777215 vertices.  Index buffer is invalid.", GetName().SafeString() );
    }

    // Create real index buffer from index list
    m_pIB = new ExportIB();
    m_pIB->SetIndexCount( IndexData.size() );
    if( VertexData.size() > 65535 || g_pScene->Settings().bForceIndex32Format )
    {
        m_pIB->SetIndexSize( 4 );
    }
    else
    {
        m_pIB->SetIndexSize( 2 );
    }
    m_pIB->Allocate();
    for( size_t i = 0; i < IndexData.size(); i++ )
    {
        m_pIB->SetIndex( i, IndexData[i] );
    }

    INT iUVAtlasTexCoordIndex = g_pScene->Settings().iGenerateUVAtlasOnTexCoordIndex;
    bool bComputeUVAtlas = ( iUVAtlasTexCoordIndex >= 0 );
    if( bComputeUVAtlas )
    {
        if( iUVAtlasTexCoordIndex < static_cast<INT>( m_VertexFormat.m_uUVSetCount ) )
        {
            ExportLog::LogWarning( "UV atlas being generated in existing texture coordinate set %d, which will overwrite its contents.", iUVAtlasTexCoordIndex );
        }
        else
        {
            // Save the index of a new UV set
            iUVAtlasTexCoordIndex = static_cast<INT>( m_VertexFormat.m_uUVSetCount );
            // add another UV set, it will be empty
            ++m_VertexFormat.m_uUVSetCount;
            ExportLog::LogMsg( 4, "Adding a new texture coordinate set for the UV atlas." );
        }
    }

    // Convert vertex data to final format
    BuildVertexBuffer( VertexData, dwFlags );

    // Check if we need to remap the UV atlas texcoord index
    if( bComputeUVAtlas && ( iUVAtlasTexCoordIndex != g_pScene->Settings().iGenerateUVAtlasOnTexCoordIndex ) )
    {
        // Scan the decl elements
        for( size_t i = 0; i < m_VertexElements.size(); ++i )
        {
            if( m_VertexElements[i].Usage == D3DDECLUSAGE_TEXCOORD && m_VertexElements[i].UsageIndex == iUVAtlasTexCoordIndex )
            {
                // Change the decl usage index to the desired usage index
                m_VertexElements[i].UsageIndex = static_cast<BYTE>( g_pScene->Settings().iGenerateUVAtlasOnTexCoordIndex );
                iUVAtlasTexCoordIndex = g_pScene->Settings().iGenerateUVAtlasOnTexCoordIndex;
            }
        }
    }

    bool bComputeTangentSpace = false;

    // Compute vertex tangent space data
    if( m_VertexFormat.m_bTangent || m_VertexFormat.m_bBinormal )
    {
        bComputeTangentSpace = true;
    }

    if( bComputeTangentSpace || bComputeUVAtlas )
    {
        ID3DXMesh* pMesh = CreateD3DXMesh();
        if( pMesh )
        {
            if( bComputeTangentSpace )
            {
                ComputeVertexTangentSpacesD3DX( &pMesh );
            }
            if( bComputeUVAtlas )
            {
                ComputeUVAtlas( &pMesh );
            }
            CopyD3DXMeshIntoMesh( pMesh );
            pMesh->Release();
        }
    }

    ClearRawTriangles();
    ComputeBounds();

    ExportLog::LogMsg( 3, "Vertex size: %u bytes; VB size: %Iu bytes", m_pVB->GetVertexSize(), m_pVB->GetVertexDataSize() );

    if( ExportLog::GetLogLevel() >= 4 )
    {
        size_t dwDeclSize = GetVertexDeclElementCount();
        
        static const CHAR* strDeclUsages[] = { "Position", "BlendWeight", "BlendIndices", "Normal", "PSize", "TexCoord", "Tangent", "Binormal", "TessFactor", "PositionT", "Color", "Fog", "Depth", "Sample" };
        C_ASSERT( ARRAYSIZE(strDeclUsages) == ( MAXD3DDECLUSAGE + 1 ) );

        static const CHAR* strDeclTypes[] = { "Float1", "Float2", "Float3", "Float4", "D3DColor", "UByte", "Short2", "Short4", "UByte4N", "Short2N", "Short4N", "UShort2N", "UShort4N", "UDec3", "Dec3N", "Float16_2", "Float16_4", "Unused" };
        C_ASSERT( ARRAYSIZE(strDeclTypes) == ( MAXD3DDECLTYPE + 1 ) );

        for( size_t i = 0; i < dwDeclSize; ++i )
        {
            const D3DVERTEXELEMENT9& Element = GetVertexDeclElement( i );

            ExportLog::LogMsg( 4, "Element %2Iu Stream %2u Offset %2u: %12s.%-2u Type %s (%d bytes)", i, Element.Stream, Element.Offset, strDeclUsages[Element.Usage], Element.UsageIndex, strDeclTypes[Element.Type], GetElementSizeFromDeclType( Element.Type ) );
        }
    }

    ExportLog::LogMsg( 4, "DCC stored %u verts; final vertex count is %Iu due to duplication.", m_uDCCVertexCount, m_pVB->GetVertexCount() );

    if( dwFlags & FORCE_SUBD_CONVERSION )
    {
        ExportLog::LogMsg( 2, "Converting mesh \"%s\" to a subdivision surface mesh.", GetName().SafeString() );
        m_pSubDMesh = new ExportSubDProcessMesh();
        m_pSubDMesh->Initialize( this );
    }
}

IDirect3DDevice9* g_pd3dDevice = nullptr;

void ExportMesh::Initialize()
{
    if ( g_pd3dDevice )
        return;

    ExportLog::LogMsg( 5, "Initializing D3D device..." );

    IDirect3D9* pD3D = Direct3DCreate9( D3D_SDK_VERSION );
    if( !pD3D )
        return;

    D3DDISPLAYMODE Mode;
    pD3D->GetAdapterDisplayMode(0, &Mode);

    HWND hDCCWindow = GetDesktopWindow();

    D3DPRESENT_PARAMETERS pp;
    ZeroMemory( &pp, sizeof(D3DPRESENT_PARAMETERS) );
    pp.BackBufferWidth  = 1;
    pp.BackBufferHeight = 1;
    pp.BackBufferFormat = Mode.Format;
    pp.BackBufferCount  = 1;
    pp.SwapEffect       = D3DSWAPEFFECT_COPY;
    pp.Windowed         = true;

    HRESULT hr;
    hr = pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, hDCCWindow, 
                             D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE | D3DCREATE_FPU_PRESERVE, &pp, &g_pd3dDevice );

    pD3D->Release();

    assert( SUCCEEDED( hr ) );

    ExportLog::LogMsg( 5, "D3D device initialized." );
}

void ExportMesh::Terminate()
{
    if( !g_pd3dDevice )
        return;

    g_pd3dDevice->Release();
    g_pd3dDevice = nullptr;
    ExportLog::LogMsg( 5, "D3D device released." );
}

ID3DXMesh* ExportMesh::CreateD3DXMesh()
{
    // D3DXMesh requires a valid D3D device...
    if ( !g_pd3dDevice )
        return nullptr;

    D3DXDebugMute( false );

    DWORD dwFaceCount = static_cast<DWORD>( m_pIB->GetIndexCount() / 3 );

    // D3DXMesh requires 32-bit index buffers if the face count is over 65534.
    bool bD3DXWorkaround = ( m_pIB->GetIndexSize() == 2 ) && ( dwFaceCount > 65534 );

    DWORD dwMeshOptions = D3DXMESH_SYSTEMMEM;
    if( bD3DXWorkaround || m_pIB->GetIndexSize() == 4 )
        dwMeshOptions |= D3DXMESH_32BIT;

    // Create a decl element list with D3DDECL_END at the end
    std::vector< D3DVERTEXELEMENT9 > DeclElements = m_VertexElements;
    D3DVERTEXELEMENT9 DeclEnd = D3DDECL_END();
    DeclElements.push_back( DeclEnd );

    auto pElements = reinterpret_cast<D3DVERTEXELEMENT9*>( &DeclElements.front() );

    // Create D3DXMesh
    ID3DXMesh* pInputD3DXMesh = nullptr;
    HRESULT hr = D3DXCreateMesh( dwFaceCount, static_cast<DWORD>( m_pVB->GetVertexCount() ), dwMeshOptions, pElements, g_pd3dDevice, &pInputD3DXMesh );
    if( FAILED( hr ) )
    {
        ExportLog::LogError( "Could not create D3DX mesh for mesh \"%s\".", GetName().SafeString() );
        return nullptr;
    }

    // copy VB data into D3DXMesh
    {
        BYTE* pVBData = nullptr;
        pInputD3DXMesh->LockVertexBuffer( 0, reinterpret_cast<void**>( &pVBData ) );
        memcpy( pVBData, m_pVB->GetVertexData(), m_pVB->GetVertexDataSize() );
        pInputD3DXMesh->UnlockVertexBuffer();
    }

    // copy IB data into D3DXMesh
    {
        BYTE* pIBData = nullptr;
        pInputD3DXMesh->LockIndexBuffer( 0, reinterpret_cast<void**>( &pIBData ) );
        if( bD3DXWorkaround )
        {
            size_t dwIndexCount = m_pIB->GetIndexCount();
            auto pIBDataDW = reinterpret_cast<DWORD*>( pIBData );
            for( size_t i = 0; i < dwIndexCount; ++i )
            {
                pIBDataDW[i] = m_pIB->GetIndex( i );
            }
        }
        else
        {
            memcpy( pIBData, m_pIB->GetIndexData(), m_pIB->GetIndexDataSize() );
        }
        pInputD3DXMesh->UnlockIndexBuffer();
    }

    // copy subset data into D3DXMesh
    {
        size_t dwSubsetCount = GetSubsetCount();
        std::unique_ptr<D3DXATTRIBUTERANGE[]> pMeshAttributeRanges( new D3DXATTRIBUTERANGE[ dwSubsetCount ] );
        for( size_t i = 0; i < dwSubsetCount; ++i )
        {
            pMeshAttributeRanges[i].AttribId = static_cast<DWORD>( i );
            const ExportIBSubset* pSubset = GetSubset( i );
            pMeshAttributeRanges[i].FaceStart = pSubset->GetStartIndex() / 3;
            pMeshAttributeRanges[i].FaceCount = pSubset->GetIndexCount() / 3;
            pMeshAttributeRanges[i].VertexStart = 0;
            pMeshAttributeRanges[i].VertexCount = static_cast<DWORD>( m_pVB->GetVertexCount() );
        }
        pInputD3DXMesh->SetAttributeTable( pMeshAttributeRanges.get(), static_cast<DWORD>( dwSubsetCount ) );
    }

    return pInputD3DXMesh;
}


void ExportMesh::CopyD3DXMeshIntoMesh( ID3DXMesh* pMesh )
{
    if( pMesh->GetNumVertices() != m_pVB->GetVertexCount() )
    {
        // check that the D3DXMesh didn't resize the vertex struct
        assert( pMesh->GetNumBytesPerVertex() == m_pVB->GetVertexSize() );
        // resize vertex buffer
        ExportVB* pNewVB = new ExportVB();
        pNewVB->SetVertexCount( pMesh->GetNumVertices() );
        pNewVB->SetVertexSize( pMesh->GetNumBytesPerVertex() );
        pNewVB->Allocate();
        ExportLog::LogMsg( 4, "D3DX mesh operations increased vertex count from %Iu to %Iu.", m_pVB->GetVertexCount(), pNewVB->GetVertexCount() );
        delete m_pVB;
        m_pVB = pNewVB;
    }

    // copy D3DXMesh VB data back into m_pVB
    {
        BYTE* pVBData = nullptr;
        pMesh->LockVertexBuffer( 0, reinterpret_cast<void**>( &pVBData ) );
        memcpy( m_pVB->GetVertexData(), pVBData, m_pVB->GetVertexDataSize() );
        pMesh->UnlockVertexBuffer();
    }

    // D3DX mesh operations should never change the index count (but the index data may change)
    assert( pMesh->GetNumFaces() == ( m_pIB->GetIndexCount() / 3 ) );

    // Sometimes the destination D3DXMesh will have exceeded 64K verts after computing
    // the tangent space.  In this case, we have to upgrade the index buffer to 32bit.
    bool bNeed32BitIB = ( m_pVB->GetVertexCount() > 65535 ) && ( m_pIB->GetIndexSize() == 2 );
    if( bNeed32BitIB )
    {
        ExportIB* pNewIB = new ExportIB();
        pNewIB->SetIndexCount( m_pIB->GetIndexCount() );
        pNewIB->SetIndexSize( 4 );
        pNewIB->Allocate();
        delete m_pIB;
        m_pIB = pNewIB;
    }

    // copy D3DXMesh IB data back into m_pIB
    {
        IDirect3DIndexBuffer9* pMeshIB = nullptr;
        pMesh->GetIndexBuffer( &pMeshIB );
        D3DINDEXBUFFER_DESC IBDesc;
        pMeshIB->GetDesc( &IBDesc );
        pMeshIB->Release();
        bool bConvert32To16 = false;
        if( IBDesc.Format == D3DFMT_INDEX32 && m_pIB->GetIndexSize() == 2 )
            bConvert32To16 = true;

        BYTE* pIBData = nullptr;
        pMesh->LockIndexBuffer( 0, reinterpret_cast<void**>( &pIBData ) );
        if( bConvert32To16 )
        {
            size_t dwIndexCount = m_pIB->GetIndexCount();
            auto pIBDataDW = reinterpret_cast<DWORD*>( pIBData );
            for( size_t i = 0; i < dwIndexCount; ++i )
            {
                m_pIB->SetIndex( i, pIBDataDW[i] );
            }
        }
        else
        {
            memcpy( m_pIB->GetIndexData(), pIBData, m_pIB->GetIndexDataSize() );
        }
        pMesh->UnlockIndexBuffer();
    }
}


HRESULT ExportMesh::ComputeVertexTangentSpacesD3DX( ID3DXMesh** ppMesh )
{
    // We must be at least computing a tangent vector here.  Binormal is optional.
    assert( m_VertexFormat.m_bTangent );

    ID3DXMesh* pSrcMesh = *ppMesh;

    // Compute tangent space, results will be in pDestMesh
    DWORD dwTangentCreationOptions = 0;
    ID3DXMesh* pDestMesh = nullptr;
    ID3DXBuffer* pMappingBuffer = nullptr;
    HRESULT hr = D3DXComputeTangentFrameEx(
        pSrcMesh,
        D3DDECLUSAGE_TEXCOORD,
        0,
        D3DDECLUSAGE_TANGENT,
        0,
        m_VertexFormat.m_bBinormal ? D3DDECLUSAGE_BINORMAL : D3DX_DEFAULT,
        0,
        D3DDECLUSAGE_NORMAL,
        0,
        dwTangentCreationOptions,
        nullptr,
        0.01f, 0.25f, 0.01f,
        &pDestMesh,
        &pMappingBuffer);

    if( SUCCEEDED( hr ) )
    {
        assert( pDestMesh != nullptr );
        pSrcMesh->Release();
        pMappingBuffer->Release();
        *ppMesh = pDestMesh;
    }
    else if( pDestMesh )
    {
        pDestMesh->Release();
    }

    return hr;
}


HRESULT ExportMesh::ComputeUVAtlas( ID3DXMesh** ppMesh )
{
    ID3DXMesh* pSrcMesh = *ppMesh;
    assert( pSrcMesh != nullptr );

    ExportLog::LogMsg( 4, "Generating UV atlas..." );

    INT iDestUVIndex = g_pScene->Settings().iGenerateUVAtlasOnTexCoordIndex;
    assert( iDestUVIndex >= 0 && iDestUVIndex < 8 );

    std::unique_ptr<DWORD[]> pAdjacency( new DWORD[ 3 * pSrcMesh->GetNumFaces() ] );
    pSrcMesh->GenerateAdjacency( 0.001f, pAdjacency.get() );

    ID3DXMesh* pDestMesh = nullptr;
    UINT dwNumCharts = 0;
    float fMaxStretch = 0;
    ID3DXBuffer* pPartitioningBuffer = nullptr;
    ID3DXBuffer* pVertexRemapBuffer = nullptr;

    HRESULT hr = D3DXUVAtlasCreate(
        pSrcMesh,
        0,
        g_pScene->Settings().fUVAtlasMaxStretch,
        g_pScene->Settings().iUVAtlasTextureSize,
        g_pScene->Settings().iUVAtlasTextureSize,
        g_pScene->Settings().fUVAtlasGutter,
        iDestUVIndex,
        pAdjacency.get(),
        nullptr,
        nullptr,
        nullptr,
        1.0f,
        nullptr,
        D3DXUVATLAS_DEFAULT,
        &pDestMesh,
        &pPartitioningBuffer,
        &pVertexRemapBuffer,
        &fMaxStretch,
        &dwNumCharts );

    SAFE_RELEASE( pPartitioningBuffer );
    SAFE_RELEASE( pVertexRemapBuffer );

    if( SUCCEEDED( hr ) )
    {
        ExportLog::LogMsg( 4, "Created UV atlas with %u charts in texcoord %d.", dwNumCharts, iDestUVIndex );
        assert( pDestMesh != nullptr );
        pSrcMesh->Release();
        *ppMesh = pDestMesh;
        return S_OK;
    }
    else if( pDestMesh )
    {
        pDestMesh->Release();
    }

    ExportLog::LogError( "UV atlas creation failed for mesh \"%s\".", GetName().SafeString() );

    return hr;
}

void NormalizeBoneWeights( BYTE* pWeights )
{
    DWORD dwSum = static_cast<DWORD>( pWeights[0] ) + static_cast<DWORD>( pWeights[1] ) + static_cast<DWORD>( pWeights[2] ) + static_cast<DWORD>( pWeights[3] );
    if( dwSum == 255 )
        return;

    INT iDifference = 255 - static_cast<INT>( dwSum );
    for( DWORD i = 0; i < 4; ++i )
    {
        if( pWeights[i] == 0 )
            continue;
        INT iValue = static_cast<INT>( pWeights[i] );
        if( iValue + iDifference > 255 )
        {
            iDifference -= ( 255 - iValue );
            iValue = 255;
        }
        else
        {
            iValue += iDifference;
            iDifference = 0;
        }
        pWeights[i] = static_cast<BYTE>( iValue );
    }

    dwSum = static_cast<DWORD>( pWeights[0] ) + static_cast<DWORD>( pWeights[1] ) + static_cast<DWORD>( pWeights[2] ) + static_cast<DWORD>( pWeights[3] );
    assert( dwSum == 255 );
}


__inline INT GetElementSizeFromDeclType(DWORD Type)
{
    switch (Type)
    {
    case D3DDECLTYPE_FLOAT1:
        return 4;
    case D3DDECLTYPE_FLOAT2:
        return 8;
    case D3DDECLTYPE_FLOAT3:
        return 12;
    case D3DDECLTYPE_FLOAT4:
        return 16;
    case D3DDECLTYPE_D3DCOLOR:
        return 4;
    case D3DDECLTYPE_UBYTE4:
        return 4;
    case D3DDECLTYPE_SHORT2:
        return 4;
    case D3DDECLTYPE_SHORT4:
        return 8;
    case D3DDECLTYPE_UBYTE4N:
        return 4;
    case D3DDECLTYPE_SHORT2N:
        return 4;
    case D3DDECLTYPE_SHORT4N:
        return 8;
    case D3DDECLTYPE_USHORT2N:
        return 4;
    case D3DDECLTYPE_USHORT4N:
        return 8;
    case D3DDECLTYPE_UDEC3:
        return 4;
    case D3DDECLTYPE_DEC3N:
        return 4;
    case D3DDECLTYPE_FLOAT16_2:
        return 4;
    case D3DDECLTYPE_FLOAT16_4:
        return 8;
    case D3DDECLTYPE_UNUSED:
        return 0;
    default:
        assert(false);
        return 0;
    }
}


void TransformAndWriteVector( BYTE* pDest, const XMFLOAT3& Src, DWORD dwDestFormat )
{
    XMFLOAT3 SrcTransformed;
    g_pScene->GetDCCTransformer()->TransformDirection( &SrcTransformed, &Src );
    switch( dwDestFormat )
    {
    case D3DDECLTYPE_FLOAT3:
        {
            *reinterpret_cast<XMFLOAT3*>(pDest) = SrcTransformed;
            break;
        }
    case D3DDECLTYPE_DEC3N:
        {
            *reinterpret_cast<XMXDECN4*>(pDest) = XMXDECN4( SrcTransformed.x, SrcTransformed.y, SrcTransformed.z, 1 );
            break;
        }
    case D3DDECLTYPE_UBYTE4N:
        {
            XMVECTOR v = XMLoadFloat3( &SrcTransformed );
            v = v * g_XMOneHalf;
            v += g_XMOneHalf;
            v = XMVectorSelect( g_XMOne, v, g_XMSelect1110 );

            XMUBYTEN4 UB4;
            XMStoreUByteN4( &UB4, v );
            
            *reinterpret_cast<XMUBYTEN4*>(pDest) = UB4;
            break;
        }
    case D3DDECLTYPE_SHORT4N:
        {
            *reinterpret_cast<XMSHORTN4*>(pDest) = XMSHORTN4( SrcTransformed.x, SrcTransformed.y, SrcTransformed.z, 1 );
            break;
        }
    case D3DDECLTYPE_FLOAT16_4:
        {
            *reinterpret_cast<XMHALF4*>(pDest) = XMHALF4( SrcTransformed.x, SrcTransformed.y, SrcTransformed.z, 1 );
            break;
        }
    }
}


void ExportMesh::BuildVertexBuffer( ExportMeshVertexArray& VertexArray, DWORD dwFlags )
{
    UINT uVertexSize = 0;
    INT iCurrentVertexOffset = 0;
    INT iPositionOffset = -1;
    INT iNormalOffset = -1;
    INT iTangentOffset = -1;
    INT iBinormalOffset = -1;
    INT iSkinDataOffset = -1;
    INT iColorOffset = -1;
    INT iUVOffset = -1;

    // create a vertex element struct and set default values
    D3DVERTEXELEMENT9 VertexElement;
    ZeroMemory( &VertexElement, sizeof( D3DVERTEXELEMENT9 ) );
    VertexElement.Method = D3DDECLMETHOD_DEFAULT;

    bool bCompressVertexData = ( dwFlags & COMPRESS_VERTEX_DATA );

    DWORD dwNormalType = D3DDECLTYPE_FLOAT3;
    if( bCompressVertexData )
    {
        dwNormalType = g_pScene->Settings().dwNormalCompressedType;
    }

    m_VertexElements.clear();

    // check each vertex format option, and create a corresponding decl element
    if( m_VertexFormat.m_bPosition )
    {
        iPositionOffset = iCurrentVertexOffset;
        VertexElement.Offset = static_cast<WORD>( iCurrentVertexOffset );
        VertexElement.Usage = D3DDECLUSAGE_POSITION;
        VertexElement.Type = D3DDECLTYPE_FLOAT3;
        m_VertexElements.push_back( VertexElement );
        iCurrentVertexOffset += GetElementSizeFromDeclType( VertexElement.Type );
    }
    if( ( m_VertexFormat.m_bSkinData && g_pScene->Settings().bExportSkinWeights ) || g_pScene->Settings().bForceExportSkinWeights )
    {
        iSkinDataOffset = iCurrentVertexOffset;
        VertexElement.Offset = static_cast<WORD>( iCurrentVertexOffset );
        VertexElement.Usage = D3DDECLUSAGE_BLENDWEIGHT;
        VertexElement.Type = D3DDECLTYPE_UBYTE4N;
        m_VertexElements.push_back( VertexElement );
        iCurrentVertexOffset += GetElementSizeFromDeclType( VertexElement.Type );
        VertexElement.Offset = static_cast<WORD>( iCurrentVertexOffset );
        VertexElement.Usage = D3DDECLUSAGE_BLENDINDICES;
        VertexElement.Type = D3DDECLTYPE_UBYTE4;
        m_VertexElements.push_back( VertexElement );
        iCurrentVertexOffset += GetElementSizeFromDeclType( VertexElement.Type );
    }
    if( m_VertexFormat.m_bNormal )
    {
        iNormalOffset = iCurrentVertexOffset;
        VertexElement.Offset = static_cast<WORD>( iCurrentVertexOffset );
        VertexElement.Usage = D3DDECLUSAGE_NORMAL;
        VertexElement.Type = static_cast<BYTE>( dwNormalType );
        m_VertexElements.push_back( VertexElement );
        iCurrentVertexOffset += GetElementSizeFromDeclType( VertexElement.Type );
    }
    if( m_VertexFormat.m_bVertexColor )
    {
        iColorOffset = iCurrentVertexOffset;
        VertexElement.Offset = static_cast<WORD>( iCurrentVertexOffset );
        VertexElement.Usage = D3DDECLUSAGE_COLOR;
        VertexElement.Type = D3DDECLTYPE_D3DCOLOR;
        m_VertexElements.push_back( VertexElement );
        iCurrentVertexOffset += GetElementSizeFromDeclType( VertexElement.Type );
    }
    if( m_VertexFormat.m_uUVSetCount > 0 )
    {
        iUVOffset = iCurrentVertexOffset;
        for( UINT t = 0; t < m_VertexFormat.m_uUVSetCount; t++ )
        {
            VertexElement.Offset = static_cast<WORD>( iCurrentVertexOffset );
            VertexElement.Usage = D3DDECLUSAGE_TEXCOORD;
            switch( m_VertexFormat.m_uUVSetSize )
            {
            case 1:
                VertexElement.Type = D3DDECLTYPE_FLOAT1;
                break;
            case 2:
                if( bCompressVertexData )
                {
                    VertexElement.Type = D3DDECLTYPE_FLOAT16_2;
                }
                else
                {
                    VertexElement.Type = D3DDECLTYPE_FLOAT2;
                }
                break;
            case 3:
                if( bCompressVertexData )
                {
                    VertexElement.Type = D3DDECLTYPE_FLOAT16_4;
                }
                else
                {
                    VertexElement.Type = D3DDECLTYPE_FLOAT3;
                }
                break;
            case 4:
                if( bCompressVertexData )
                {
                    VertexElement.Type = D3DDECLTYPE_FLOAT16_4;
                }
                else
                {
                    VertexElement.Type = D3DDECLTYPE_FLOAT4;
                }
            default:
                continue;
            }
            VertexElement.UsageIndex = static_cast<BYTE>( t );
            m_VertexElements.push_back( VertexElement );
            iCurrentVertexOffset += GetElementSizeFromDeclType( VertexElement.Type );
        }
        VertexElement.UsageIndex = 0;
    }
    if( m_VertexFormat.m_bTangent )
    {
        iTangentOffset = iCurrentVertexOffset;
        VertexElement.Offset = static_cast<WORD>( iCurrentVertexOffset );
        VertexElement.Usage = D3DDECLUSAGE_TANGENT;
        VertexElement.Type = static_cast<BYTE>( dwNormalType );
        m_VertexElements.push_back( VertexElement );
        iCurrentVertexOffset += GetElementSizeFromDeclType( VertexElement.Type );
    }
    if( m_VertexFormat.m_bBinormal )
    {
        iBinormalOffset = iCurrentVertexOffset;
        VertexElement.Offset = static_cast<WORD>( iCurrentVertexOffset );
        VertexElement.Usage = D3DDECLUSAGE_BINORMAL;
        VertexElement.Type = static_cast<BYTE>( dwNormalType );
        m_VertexElements.push_back( VertexElement );
        iCurrentVertexOffset += GetElementSizeFromDeclType( VertexElement.Type );
    }

    // save vertex size
    uVertexSize = iCurrentVertexOffset;
    if( uVertexSize == 0 )
        return;

    // create vertex buffer and allocate storage
    m_pVB = new ExportVB();
    m_pVB->SetVertexCount( VertexArray.size() );
    m_pVB->SetVertexSize( uVertexSize );
    m_pVB->Allocate();

    // copy raw vertex data into the packed vertex buffer
    for( size_t i = 0; i < VertexArray.size(); i++ )
    {
        auto pDestVertex = m_pVB->GetVertex( i );
        ExportMeshVertex* pSrcVertex = VertexArray[i];
        if( !pSrcVertex )
        {
            continue;
        }

        if( iPositionOffset != -1 )
        {
            auto pDest = reinterpret_cast<XMFLOAT3*>( pDestVertex + iPositionOffset );
            g_pScene->GetDCCTransformer()->TransformPosition( pDest, &pSrcVertex->Position );
        }
        if( iNormalOffset != -1 )
        {
            TransformAndWriteVector( pDestVertex + iNormalOffset, pSrcVertex->Normal, dwNormalType );
        }
        if( iSkinDataOffset != -1 )
        {
            BYTE* pDest = pDestVertex + iSkinDataOffset;
            BYTE* pBoneWeights = pDest;
            *pDest++ = static_cast<BYTE>( pSrcVertex->BoneWeights.x * 255.0f );
            *pDest++ = static_cast<BYTE>( pSrcVertex->BoneWeights.y * 255.0f );
            *pDest++ = static_cast<BYTE>( pSrcVertex->BoneWeights.z * 255.0f );
            *pDest++ = static_cast<BYTE>( pSrcVertex->BoneWeights.w * 255.0f );
            NormalizeBoneWeights( pBoneWeights );
            *pDest++ = pSrcVertex->BoneIndices.x;
            *pDest++ = pSrcVertex->BoneIndices.y;
            *pDest++ = pSrcVertex->BoneIndices.z;
            *pDest++ = pSrcVertex->BoneIndices.w;
        }
        if( iTangentOffset != -1 )
        {
            TransformAndWriteVector( pDestVertex + iTangentOffset, pSrcVertex->Tangent, dwNormalType );
        }
        if( iBinormalOffset != -1 )
        {
            TransformAndWriteVector( pDestVertex + iBinormalOffset, pSrcVertex->Binormal, dwNormalType );
        }
        if( iColorOffset != -1 )
        {
            UINT uColor = 0;
            uColor |= ( static_cast<BYTE>( pSrcVertex->Color.w * 255.0f ) ) << 24;
            uColor |= ( static_cast<BYTE>( pSrcVertex->Color.x * 255.0f ) ) << 16;
            uColor |= ( static_cast<BYTE>( pSrcVertex->Color.y * 255.0f ) ) << 8;
            uColor |= ( static_cast<BYTE>( pSrcVertex->Color.z * 255.0f ) );
            memcpy( pDestVertex + iColorOffset, &uColor, 4 );
        }
        if( iUVOffset != -1 )
        {
            if( bCompressVertexData )
            {
                auto pDest = reinterpret_cast<DWORD*>( pDestVertex + iUVOffset ); 
                for( size_t t = 0; t < m_VertexFormat.m_uUVSetCount; t++ )
                {
                    switch( m_VertexFormat.m_uUVSetSize )
                    {
                    case 1:
                        {
                            memcpy( pDest, &pSrcVertex->TexCoords[t], 4 );
                            pDest++;
                            break;
                        }
                    case 2:
                        {
                            auto pFloat16 = reinterpret_cast<HALF*>(pDest);
                            XMConvertFloatToHalfStream( pFloat16, sizeof(HALF), reinterpret_cast<const float*>( &pSrcVertex->TexCoords[t] ), sizeof(float), 2 );
                            pDest++;
                            break;
                        }
                    case 3:
                        {
                            pDest[1] = 0;
                            auto pFloat16 = reinterpret_cast<HALF*>(pDest);
                            XMConvertFloatToHalfStream( pFloat16, sizeof(HALF), reinterpret_cast<const float*>( &pSrcVertex->TexCoords[t] ), sizeof(float), 3 );
                            pDest += 2;
                            break;
                        }
                    case 4:
                        {
                            auto pFloat16 = reinterpret_cast<HALF*>(pDest);
                            XMConvertFloatToHalfStream( pFloat16, sizeof(HALF), reinterpret_cast<const float*>( &pSrcVertex->TexCoords[t] ), sizeof(float), 4 );
                            pDest += 2;
                            break;
                        }
                    default:
                        assert( false );
                        break;
                    }
                }
            }
            else
            {
                size_t uStride = m_VertexFormat.m_uUVSetSize * sizeof( float );
                BYTE* pDest = pDestVertex + iUVOffset;
                for( UINT t = 0; t < m_VertexFormat.m_uUVSetCount; t++ )
                {
                    memcpy( pDest, &pSrcVertex->TexCoords[t], uStride );
                    pDest += uStride;
                }
            }
        }
    }
}

void ExportMesh::ComputeBounds()
{
    if( !m_pVB )
        return;

    ComputeBoundingSphereFromPoints( &m_BoundingSphere, static_cast<UINT>( m_pVB->GetVertexCount() ), 
        reinterpret_cast<const XMFLOAT3*>( m_pVB->GetVertexData() ), m_pVB->GetVertexSize() );

    ComputeBoundingAxisAlignedBoxFromPoints( &m_BoundingAABB,
                                             static_cast<UINT>( m_pVB->GetVertexCount() ),
                                             reinterpret_cast<const XMFLOAT3*>( m_pVB->GetVertexData() ),
                                             m_pVB->GetVertexSize() );

    ComputeBoundingOrientedBoxFromPoints( &m_BoundingOBB,
                                          static_cast<UINT>( m_pVB->GetVertexCount() ),
                                          reinterpret_cast<const XMFLOAT3*>( m_pVB->GetVertexData() ),
                                          m_pVB->GetVertexSize() );

    float fVolumeSphere = XM_PI * ( 4.0f / 3.0f ) * 
                          m_BoundingSphere.Radius * 
                          m_BoundingSphere.Radius * 
                          m_BoundingSphere.Radius;

    float fVolumeAABB = m_BoundingAABB.Extents.x * 
                        m_BoundingAABB.Extents.y * 
                        m_BoundingAABB.Extents.z * 8.0f;

    float fVolumeOBB = m_BoundingOBB.Extents.x *
                       m_BoundingOBB.Extents.y *
                       m_BoundingOBB.Extents.z * 8.0f;

    if( fVolumeAABB <= fVolumeSphere && fVolumeAABB <= fVolumeOBB )
        m_SmallestBound = AxisAlignedBoxBound;
    else if( fVolumeOBB <= fVolumeAABB && fVolumeOBB <= fVolumeSphere )
        m_SmallestBound = OrientedBoxBound;
    else
        m_SmallestBound = SphereBound;
}

bool ExportModel::SetSubsetBinding( ExportString SubsetName, ExportMaterial* pMaterial, bool bSkipValidation )
{
    assert( m_pMesh != nullptr );
    if( !bSkipValidation )
    {
        bool bResult = false;
        for( UINT i = 0; i < m_pMesh->GetSubsetCount(); i++ )
        {
            ExportIBSubset* pSubset = m_pMesh->GetSubset( i );
            if( pSubset->GetName() == SubsetName )
                bResult = true;
        }
        if( !bResult )
            return false;
    }
    for( size_t i = 0; i < m_vBindings.size(); i++ )
    {
        ExportMaterialSubsetBinding* pBinding = m_vBindings[i];
        if( pBinding->SubsetName == SubsetName )
        {
            pBinding->pMaterial = pMaterial;
            return true;
        }
    }
    ExportMaterialSubsetBinding* pBinding = new ExportMaterialSubsetBinding();
    pBinding->SubsetName = SubsetName;
    pBinding->pMaterial = pMaterial;
    m_vBindings.push_back( pBinding );
    return true;
}

ExportModel::~ExportModel()
{
    for( UINT i = 0; i < m_vBindings.size(); i++ )
    {
        delete m_vBindings[i];
    }
    m_vBindings.clear();
    m_pMesh = nullptr;
}

};
