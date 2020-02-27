//-------------------------------------------------------------------------------------
// ExportMaterial.cpp
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------

#include "stdafx.h"
#include "ExportMaterial.h"

using namespace ATG;

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

ExportString ExportMaterial::GetDefaultDiffuseMapTextureName()
{
    return ExportString( g_ExportCoreSettings.strDefaultDiffuseMapTextureName );
}

ExportString ExportMaterial::GetDefaultNormalMapTextureName()
{
    return ExportString( g_ExportCoreSettings.strDefaultNormalMapTextureName );
}

ExportString ExportMaterial::GetDefaultSpecularMapTextureName()
{
    return ExportString( g_ExportCoreSettings.strDefaultSpecMapTextureName );
}

