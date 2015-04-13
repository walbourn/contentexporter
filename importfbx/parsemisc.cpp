//-------------------------------------------------------------------------------------
// ParseMisc.cpp
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

#include "StdAfx.h"
#include "ParseMisc.h"
#include "ParseMesh.h"

extern ATG::ExportScene* g_pScene;

static D3DXMATRIX ConvertMatrix( const FbxMatrix& matFbx )
{
    D3DXMATRIX matConverted;
    FLOAT* pFloats = (FLOAT*)&matConverted;
    const DOUBLE* pDoubles = (const DOUBLE*)matFbx.mData;
    for( DWORD i = 0; i < 16; ++i )
    {
        pFloats[i] = (FLOAT)pDoubles[i];
    }
    return matConverted;
}

inline BOOL IsEqual( FLOAT A, FLOAT B )
{
    return fabs( A - B ) <= 1e-5f;
}

D3DXMATRIX ParseTransform( FbxNode* pNode, ExportFrame* pFrame, const D3DXMATRIX& matParentWorld, const BOOL bWarnings = TRUE )
{
    D3DXMATRIX matWorld;
    D3DXMATRIX matLocal;
    BOOL bProcessDefaultTransform = TRUE;

    if( !g_BindPoseMap.empty() )
    {
        PoseMap::iterator iter = g_BindPoseMap.find( pNode );
        if( iter != g_BindPoseMap.end() )
        {
            FbxMatrix PoseMatrix = iter->second;
            matWorld = ConvertMatrix( PoseMatrix );
            D3DXMATRIX matInvParentWorld;
            D3DXMatrixInverse( &matInvParentWorld, NULL, &matParentWorld );
            D3DXMatrixMultiply( &matLocal, &matWorld, &matInvParentWorld );
            bProcessDefaultTransform = FALSE;
        }
    }

    if( bProcessDefaultTransform )
    {
        FbxVector4 Translation;
        if ( pNode->LclTranslation.IsValid() )
            Translation = pNode->LclTranslation.Get();

        FbxVector4 Rotation;
        if ( pNode->LclRotation.IsValid() )
            Rotation = pNode->LclRotation.Get();

        FbxVector4 Scale;
        if ( pNode->LclScaling.IsValid() )
            Scale = pNode->LclScaling.Get();

        FbxMatrix matTransform( Translation, Rotation, Scale );
        matLocal = ConvertMatrix( matTransform );
        D3DXMatrixMultiply( &matWorld, &matParentWorld, &matLocal );
    }

    pFrame->Transform().InitializeFromFloats( (FLOAT*)&matLocal );

    const D3DXVECTOR3& Scale = pFrame->Transform().Scale();
    if( bWarnings && 
        ( !IsEqual( Scale.x, Scale.y ) ||
          !IsEqual( Scale.y, Scale.z ) ||
          !IsEqual( Scale.x, Scale.z ) ) )
    {
        ExportLog::LogWarning( "Non-uniform scale found on node \"%s\".", pFrame->GetName().SafeString() );
    }

    const ExportTransform& Transform = pFrame->Transform();
    ExportLog::LogMsg( 5, "Node transform for \"%s\": Translation <%0.3f %0.3f %0.3f> Rotation <%0.3f %0.3f %0.3f %0.3f> Scale <%0.3f %0.3f %0.3f>",
        pFrame->GetName().SafeString(),
        Transform.Position().x,
        Transform.Position().y,
        Transform.Position().z,
        Transform.Orientation().x,
        Transform.Orientation().y,
        Transform.Orientation().z,
        Transform.Orientation().w,
        Transform.Scale().x,
        Transform.Scale().y,
        Transform.Scale().z );

    return matWorld;
}


VOID ParseNode( FbxNode* pNode, ExportFrame* pParentFrame, const D3DXMATRIX& matParentWorld )
{
    ExportLog::LogMsg( 2, "Parsing node \"%s\".", pNode->GetName() );

    auto pFrame = new ExportFrame( pNode->GetName() );
    pFrame->SetDCCObject( pNode );
    D3DXMATRIX matWorld = ParseTransform( pNode, pFrame, matParentWorld );
    pParentFrame->AddChild( pFrame );

    if( pNode->GetSubdiv() != NULL )
    {
        ParseSubDiv( pNode, pNode->GetSubdiv(), pFrame );
    }
    else if( pNode->GetMesh() != NULL )
    {
        ParseMesh( pNode, pNode->GetMesh(), pFrame, FALSE );
    }
    ParseCamera( pNode->GetCamera(), pFrame );
    ParseLight( pNode->GetLight(), pFrame );

    DWORD dwChildCount = pNode->GetChildCount();
    for( DWORD i = 0; i < dwChildCount; ++i )
    {
        ParseNode( pNode->GetChild( i ), pFrame, matWorld );
    }
}

VOID FixupNode( ExportFrame* pFrame, const D3DXMATRIX& matParentWorld )
{
    auto pNode = reinterpret_cast<FbxNode*>( pFrame->GetDCCObject() );

    D3DXMATRIX matWorld;
    if( pNode != NULL )
    {
        ExportLog::LogMsg( 4, "Fixing up frame \"%s\".", pFrame->GetName().SafeString() );
        matWorld = ParseTransform( pNode, pFrame, matParentWorld, FALSE );
    }
    else
    {
        matWorld = matParentWorld;
    }

    DWORD dwChildCount = pFrame->GetChildCount();
    for( DWORD i = 0; i < dwChildCount; ++i )
    {
        FixupNode( pFrame->GetChildByIndex( i ), matWorld );
    }
}

VOID ParseCamera( FbxCamera* pFbxCamera, ExportFrame* pParentFrame )
{
    if( pFbxCamera == NULL || !g_pScene->Settings().bExportCameras )
        return;

    ExportLog::LogMsg( 2, "Parsing camera \"%s\".", pFbxCamera->GetName() );

    ExportCamera* pCamera = new ExportCamera( pFbxCamera->GetName() );
    pCamera->SetDCCObject( pFbxCamera );

    pCamera->fNearClip = (FLOAT)pFbxCamera->NearPlane.Get();
    pCamera->fFarClip = (FLOAT)pFbxCamera->FarPlane.Get();
    pCamera->fFieldOfView = (FLOAT)pFbxCamera->FieldOfView.Get();
    pCamera->fFocalLength = (FLOAT)pFbxCamera->FocalLength.Get();

    pParentFrame->AddCamera( pCamera );
}

VOID ParseLight( FbxLight* pFbxLight, ExportFrame* pParentFrame )
{
    if( pFbxLight == NULL || !g_pScene->Settings().bExportLights )
        return;

    switch( pFbxLight->LightType.Get() )
    {
    case FbxLight::ePoint:
    case FbxLight::eSpot:
    case FbxLight::eDirectional:
        break;

    case FbxLight::eArea:
    case FbxLight::eVolume:
        ExportLog::LogWarning( "Ignores area and volume lights" );
        return;

    default:
        ExportLog::LogWarning( "Could not determine light type, ignored." );
        return;
    }

    ExportLog::LogMsg( 2, "Parsing light \"%s\".", pFbxLight->GetName() );

    ExportLight* pLight = new ExportLight( pFbxLight->GetName() );
    pLight->SetDCCObject( pFbxLight );
    pParentFrame->AddLight( pLight );

    auto colorRGB = pFbxLight->Color.Get();
    FLOAT fIntensity = (FLOAT)pFbxLight->Intensity.Get();
    fIntensity *= 0.01f;

    D3DXCOLOR Color( (FLOAT)colorRGB[0], (FLOAT)colorRGB[1], (FLOAT)colorRGB[2], 1.0f );
    Color *= fIntensity;
    pLight->Color = Color;

    switch( pFbxLight->DecayType.Get() )
    {
    case FbxLight::eNone:
        pLight->Falloff = ExportLight::LF_NONE;
        pLight->fRange = 20.0f;
        break;

    case FbxLight::eLinear:
        pLight->Falloff = ExportLight::LF_LINEAR;
        pLight->fRange = 4.0f * fIntensity;
        break;

    case FbxLight::eQuadratic:
    case FbxLight::eCubic:
        pLight->Falloff = ExportLight::LF_SQUARED;
        pLight->fRange = 2.0f * sqrtf( fIntensity );
        break;

    default:
        ExportLog::LogWarning( "Could not determine light decay type, using None" );
        pLight->Falloff = ExportLight::LF_NONE;
        pLight->fRange = 20.0f;
        break;
    }

    pLight->fRange *= g_pScene->Settings().fLightRangeScale;

    ExportLog::LogMsg( 4, "Light color (multiplied by intensity): <%0.2f %0.2f %0.2f> intensity: %0.2f falloff: %0.2f", Color.r, Color.g, Color.b, fIntensity, pLight->fRange );

    switch( pFbxLight->LightType.Get() )
    {
    case FbxLight::ePoint:
        pLight->Type = ExportLight::LT_POINT;
        break;

    case FbxLight::eSpot:
        pLight->Type = ExportLight::LT_SPOT;
        pLight->fOuterAngle = (FLOAT)pFbxLight->OuterAngle.Get();
        pLight->fInnerAngle = (FLOAT)pFbxLight->InnerAngle.Get();
        pLight->SpotFalloff = pLight->Falloff;
        break;

    case FbxLight::eDirectional:
        pLight->Type = ExportLight::LT_DIRECTIONAL;
        break;
    }
}