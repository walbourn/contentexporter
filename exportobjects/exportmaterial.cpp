//-------------------------------------------------------------------------------------
// ExportMaterial.cpp
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
#include "ExportMaterial.h"

namespace ATG
{

ExportMaterial::ExportMaterial()
: ExportBase( nullptr ),
  m_pMaterialDefinition( nullptr ),
  m_bTransparent( false )
{
}

ExportMaterial::ExportMaterial( ExportString name )
: ExportBase( name ),
  m_pMaterialDefinition( nullptr ),
  m_bTransparent( false )
{
}

ExportMaterial::~ExportMaterial()
{
}

ExportMaterialParameter* ExportMaterial::FindParameter( const ExportString strName )
{
    MaterialParameterList::iterator iter = m_Parameters.begin();
    MaterialParameterList::iterator end = m_Parameters.end();
    while( iter != end )
    {
        ExportMaterialParameter& param = *iter;
        if( param.Name == strName )
            return &param;
        ++iter;
    }
    return nullptr;
}

LPDIRECT3DDEVICE9 g_pd3dDevice = nullptr;
DWORD g_dwRefCount = 0;

LPDIRECT3DDEVICE9 ExportMaterial::GetDirect3DDevice()
{
    if( g_dwRefCount > 0 )
    {
        assert( g_pd3dDevice != nullptr );
        ++g_dwRefCount;
        return g_pd3dDevice;
    }

    ExportLog::LogMsg( 5, "Initializing D3D device..." );

    assert( g_pd3dDevice == nullptr );

    IDirect3D9* pD3D = Direct3DCreate9( D3D_SDK_VERSION );
    if( !pD3D )
        return nullptr;

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

    ++g_dwRefCount;
    return g_pd3dDevice;
}


void ExportMaterial::ReleaseDirect3DDevice()
{
    if( !g_pd3dDevice )
    {
        return;
    }

    assert( g_dwRefCount > 0 );
    --g_dwRefCount;

    if( g_dwRefCount == 0 )
    {
        assert( g_pd3dDevice != nullptr );
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
        ExportLog::LogMsg( 5, "D3D device released." );
    }
}

ExportString ExportMaterial::GetDefaultDiffuseMapTextureName()
{
    return ExportString( g_ExportCoreSettings.strDefaultDiffuseMapTextureName );
}

ExportString ExportMaterial::GetDefaultNormalMapTextureName()
{
    return ExportString( g_ExportCoreSettings.strDefaultNormalMapTextureName );
}

};

