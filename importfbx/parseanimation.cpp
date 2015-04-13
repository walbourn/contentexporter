//-------------------------------------------------------------------------------------
// ParseAnimation.cpp
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
#include "ParseAnimation.h"

extern ATG::ExportScene* g_pScene;

struct AnimationScanNode
{
    INT iParentIndex;
    FbxNode* pNode;
    ExportAnimationTrack* pTrack;
    DWORD dwFlags;
    D3DXMATRIX matGlobal;
};

typedef vector<AnimationScanNode> ScanList;

VOID ParseNode( FbxNode* pNode, ScanList& scanlist, DWORD dwFlags, INT iParentIndex, BOOL bIncludeNode )
{
    INT iCurrentIndex = iParentIndex;

    if( !bIncludeNode )
    {
        const CHAR* strNodeName = pNode->GetName();
        if( _stricmp( strNodeName, g_pScene->Settings().strAnimationRootNodeName ) == 0 )
        {
            bIncludeNode = TRUE;
        }
    }

    if( bIncludeNode )
    {
        iCurrentIndex = (INT)scanlist.size();

        // add node to anim list
        AnimationScanNode asn = { 0 };
        asn.iParentIndex = iParentIndex;
        asn.pNode = pNode;
        asn.dwFlags = dwFlags;
        scanlist.push_back( asn );
    }

    DWORD dwChildCount = pNode->GetChildCount();
    for( DWORD i = 0; i < dwChildCount; ++i )
    {
        ParseNode( pNode->GetChild( i ), scanlist, dwFlags, iCurrentIndex, bIncludeNode );
    }
}

static D3DXMATRIX ConvertMatrix( const FbxMatrix& matrix )
{
    D3DXMATRIX matResult;
    FLOAT* fData = (FLOAT*)&matResult;
    DOUBLE* pSrcData = (DOUBLE*)&matrix;
    for( DWORD i = 0; i < 16; ++i )
    {
        fData[i] = (FLOAT)pSrcData[i];
    }
    return matResult;
}


VOID AddKey( AnimationScanNode& asn, const AnimationScanNode* pParent, FbxAMatrix& matFBXGlobal, FLOAT fTime )
{
    D3DXMATRIX matGlobal = ConvertMatrix( matFBXGlobal );
    asn.matGlobal = matGlobal;
    D3DXMATRIX matLocal = matGlobal;
    if( pParent != NULL )
    {
        D3DXMATRIX matInvParentGlobal;
        D3DXMatrixInverse( &matInvParentGlobal, NULL, &pParent->matGlobal );

        D3DXMatrixMultiply( &matLocal, &matGlobal, &matInvParentGlobal );
    }

    D3DXMATRIX matLocalFinal;
    g_pScene->GetDCCTransformer()->TransformMatrix( &matLocalFinal, &matLocal );

    XMVECTOR vScale;
    XMVECTOR qRotation;
    XMVECTOR vTranslation;

    XMMatrixDecompose( &vScale, &qRotation, &vTranslation, (XMMATRIX)matLocalFinal );

    asn.pTrack->TransformTrack.AddKey( fTime, *(D3DXVECTOR3*)&vTranslation, *(D3DXQUATERNION*)&qRotation, *(D3DXVECTOR3*)&vScale );
}

VOID CaptureAnimation( ScanList& scanlist, ExportAnimation* pAnim, FbxScene* pFbxScene )
{
    const FLOAT fDeltaTime = pAnim->fSourceSamplingInterval;
    const FLOAT fStartTime = pAnim->fStartTime;
    const FLOAT fEndTime = pAnim->fEndTime;
    FLOAT fCurrentTime = fStartTime;

    DWORD dwNodeCount = (DWORD)scanlist.size();

    ExportLog::LogMsg( 2, "Capturing animation data from %d nodes, from time %0.3f to %0.3f, at an interval of %0.3f seconds.", dwNodeCount, fStartTime, fEndTime, fDeltaTime );

    while( fCurrentTime <= fEndTime )
    {
        FbxTime CurrentTime;
        CurrentTime.SetSecondDouble( fCurrentTime );
        for( DWORD i = 0; i < dwNodeCount; ++i )
        {
            AnimationScanNode& asn = scanlist[i];
            auto pAnimEvaluator = pFbxScene->GetEvaluator();
            auto matGlobal = pAnimEvaluator->GetNodeGlobalTransform( asn.pNode, CurrentTime );
            AnimationScanNode* pParent = NULL;
            if( asn.iParentIndex >= 0 )
                pParent = &scanlist[asn.iParentIndex];
            AddKey( asn, pParent, matGlobal, fCurrentTime - fStartTime );
        }
        fCurrentTime += fDeltaTime;
    }
}

VOID ParseAnimStack( FbxScene* pFbxScene, FbxString* strAnimStackName )
{
    // TODO - Ignore "Default"? FBXSDK_TAKENODE_DEFAULT_NAME

    auto curAnimStack = pFbxScene->FindMember<FbxAnimStack>( strAnimStackName->Buffer() );
    if ( !curAnimStack )
        return;

    pFbxScene->GetEvaluator()->SetContext( curAnimStack );

    auto pTakeInfo = pFbxScene->GetTakeInfo( *strAnimStackName  );

    ExportLog::LogMsg( 2, "Parsing animation \"%s\"", strAnimStackName->Buffer() );

    auto pAnim = new ExportAnimation();
    pAnim->SetName( strAnimStackName->Buffer() );
    pAnim->SetDCCObject( pTakeInfo );
    g_pScene->AddAnimation( pAnim );

    FbxTime FrameTime;
    FrameTime.SetTime( 0, 0, 0, 1, 0, pFbxScene->GetGlobalSettings().GetTimeMode() );

    FLOAT fFrameTime = (FLOAT)FrameTime.GetSecondDouble();
    FLOAT fSampleTime = fFrameTime / (FLOAT)g_pScene->Settings().iAnimSampleCountPerFrame;
    assert( fSampleTime > 0 );

    float fStartTime, fEndTime;
    if( pTakeInfo != NULL )
    {
        fStartTime = (FLOAT)pTakeInfo->mLocalTimeSpan.GetStart().GetSecondDouble();
        fEndTime = (FLOAT)pTakeInfo->mLocalTimeSpan.GetStop().GetSecondDouble();
    }
    else
    {
        FbxTimeSpan tlTimeSpan;
        pFbxScene->GetGlobalSettings().GetTimelineDefaultTimeSpan( tlTimeSpan );

        fStartTime = (float)tlTimeSpan.GetStart().GetSecondDouble();
        fEndTime = (float)tlTimeSpan.GetStop().GetSecondDouble();

        ExportLog::LogWarning( "Animation take \"%s\" has no takeinfo; using defaults.", pAnim->GetName().SafeString() );
    }
    pAnim->fStartTime = fStartTime;
    pAnim->fEndTime = fEndTime;
    pAnim->fSourceFrameInterval = fFrameTime;
    pAnim->fSourceSamplingInterval = fSampleTime;

    BOOL bIncludeAllNodes = TRUE;
    if( strlen( g_pScene->Settings().strAnimationRootNodeName ) > 0 )
    {
        bIncludeAllNodes = FALSE;
    }

    ScanList scanlist;
    ParseNode( pFbxScene->GetRootNode(), scanlist, 0, -1, bIncludeAllNodes );

    DWORD dwTrackCount = (DWORD)scanlist.size();
    for( DWORD i = 0; i < dwTrackCount; ++i )
    {
        const CHAR* strTrackName = scanlist[i].pNode->GetName();
        ExportLog::LogMsg( 4, "Track: %s", strTrackName );
        auto pTrack = new ExportAnimationTrack();
        pTrack->SetName( strTrackName );
        pTrack->TransformTrack.pSourceFrame = g_pScene->FindFrameByDCCObject( scanlist[i].pNode );
        pAnim->AddTrack( pTrack );
        scanlist[i].pTrack = pTrack;
    }

    CaptureAnimation( scanlist, pAnim, pFbxScene );

    pAnim->Optimize();
}

VOID ParseAnimation( FbxScene* pFbxScene )
{
    assert( pFbxScene != NULL );

    // set animation quality settings
    ExportAnimation::SetAnimationExportQuality( g_pScene->Settings().iAnimPositionExportQuality, g_pScene->Settings().iAnimOrientationExportQuality, 50 );

    FbxArray<FbxString*> AnimStackNameArray;
    pFbxScene->FillAnimStackNameArray( AnimStackNameArray );

    DWORD dwAnimStackCount = (DWORD)AnimStackNameArray.GetCount();
    for( DWORD i = 0; i < dwAnimStackCount; ++i )
    {
        ParseAnimStack( pFbxScene, AnimStackNameArray.GetAt(i) );
    }
}
