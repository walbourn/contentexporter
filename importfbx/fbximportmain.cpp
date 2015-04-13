//-------------------------------------------------------------------------------------
//  FBXImportMain.cpp
//
//  Microsoft XNA Developer Connection
//  Copyright © Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "FBXImportMain.h"
#include "ParseMisc.h"
#include "ParseAnimation.h"

using namespace ATG;

KFbxSdkManager* g_pSDKManager = NULL;
KFbxImporter* g_pImporter = NULL;
KFbxScene* g_pFBXScene = NULL;
KFbxPose* g_pBindPose = NULL;

extern ATG::ExportScene* g_pScene;

extern ExportPath g_CurrentOutputFileName;

VOID FBXTransformer::TransformMatrix( D3DXMATRIX* pDestMatrix, CONST D3DXMATRIX* pSrcMatrix ) CONST
{
	D3DXMATRIX SrcMatrix;
	if( pSrcMatrix == pDestMatrix )
	{
		memcpy( &SrcMatrix, pSrcMatrix, sizeof( D3DXMATRIX ) );
		pSrcMatrix = &SrcMatrix;
	}
	memcpy( pDestMatrix, pSrcMatrix, sizeof( D3DXMATRIX ) );

	// What we're doing here is premultiplying by a left hand -> right hand matrix,
	// and then postmultiplying by a right hand -> left hand matrix.
	// The end result of those multiplications is that the third row and the third
	// column are negated (so element _33 is left alone).  So instead of actually
	// carrying out the multiplication, we just negate the 6 matrix elements.

	pDestMatrix->_13 = -pSrcMatrix->_13;
	pDestMatrix->_23 = -pSrcMatrix->_23;
	pDestMatrix->_43 = -pSrcMatrix->_43;

	pDestMatrix->_31 = -pSrcMatrix->_31;
	pDestMatrix->_32 = -pSrcMatrix->_32;
	pDestMatrix->_34 = -pSrcMatrix->_34;

	// Apply the global unit scale to the translation components of the matrix.
	pDestMatrix->_41 *= m_fUnitScale;
	pDestMatrix->_42 *= m_fUnitScale;
	pDestMatrix->_43 *= m_fUnitScale;
}

VOID FBXTransformer::TransformPosition( D3DXVECTOR3* pDestPosition, CONST D3DXVECTOR3* pSrcPosition ) CONST
{
	D3DXVECTOR3 SrcVector;
	if( pSrcPosition == pDestPosition )
	{
		SrcVector = *pSrcPosition;
		pSrcPosition = &SrcVector;
	}
	pDestPosition->x = pSrcPosition->x * m_fUnitScale;
	pDestPosition->y = pSrcPosition->y * m_fUnitScale;
	pDestPosition->z = -pSrcPosition->z * m_fUnitScale;
}

VOID FBXTransformer::TransformDirection( D3DXVECTOR3* pDestDirection, CONST D3DXVECTOR3* pSrcDirection ) CONST
{
	D3DXVECTOR3 SrcVector;
	if( pSrcDirection == pDestDirection )
	{
		SrcVector = *pSrcDirection;
		pSrcDirection = &SrcVector;
	}
	pDestDirection->x = pSrcDirection->x;
	pDestDirection->y = pSrcDirection->y;
	pDestDirection->z = -pSrcDirection->z;
}

FLOAT FBXTransformer::TransformLength( FLOAT fInputLength ) CONST
{
	return fInputLength * m_fUnitScale;
}


HRESULT FBXImport::Initialize()
{
    if( g_pSDKManager == NULL )
    {
        g_pSDKManager = KFbxSdkManager::Create();
        if( g_pSDKManager == NULL )
            return E_FAIL;
    }

    if( g_pImporter == NULL )
    {
        g_pImporter = KFbxImporter::Create( g_pSDKManager, "" );
    }
    g_pFBXScene = KFbxScene::Create( g_pSDKManager, "" );

    return S_OK;
}

VOID FBXImport::ClearScene()
{
    g_pFBXScene->Clear();
}

VOID SetBindPose()
{
	assert( g_pFBXScene != NULL );

    g_pBindPose = NULL;
	INT iPoseCount = g_pFBXScene->GetPoseCount();
	INT iBindPoseIndex = -1;
    BOOL bMultipleBindPoses = FALSE;
	for( INT i = 0; i < iPoseCount; ++i )
	{
		KFbxPose* pPose = g_pFBXScene->GetPose( i );
        ExportLog::LogMsg( 4, "Found %spose: \"%s\"", pPose->IsBindPose() ? "bind " : "", pPose->GetName() );
		if( pPose->IsBindPose() && pPose->IsValidBindPose( g_pFBXScene->GetRootNode() ) )
		{
			if( iBindPoseIndex != -1 && !bMultipleBindPoses )
			{
                bMultipleBindPoses = TRUE;
				ExportLog::LogWarning( "More than one valid bind pose found." );
			}
			iBindPoseIndex = i;
		}
	}
	if( iBindPoseIndex == -1 )
	{
        if( g_pScene->Settings().bExportAnimations )
        {
            ExportLog::LogWarning( "No valid bind pose found; will export scene using the default pose." );
        }
	}
	else
	{
		KFbxPose* pBindPose = g_pFBXScene->GetPose( iBindPoseIndex );
		ExportLog::LogMsg( 2, "Using bind pose \"%s\".", pBindPose->GetName() );
		if( !pBindPose->IsValidBindPose( g_pFBXScene->GetRootNode() ) )
		{
			ExportLog::LogWarning( "Bind pose \"%s\" is not a valid bind pose.", pBindPose->GetName() );
		}
		// apply bind pose
		g_pBindPose = pBindPose;
	}
}

HRESULT FBXImport::ImportFile( const CHAR* strFileName )
{
    assert( g_pSDKManager != NULL );
    assert( g_pImporter != NULL );
    assert( g_pFBXScene != NULL );

    assert( g_pScene != NULL );

    CHAR strTemp[100];
    g_pScene->Information().ExporterName = g_strExporterName;
    INT iMajorVersion, iMinorVersion, iRevision;
    KFbxIO::GetCurrentVersion( iMajorVersion, iMinorVersion, iRevision );
    sprintf_s( strTemp, "FBX SDK %d.%d.%d", iMajorVersion, iMinorVersion, iRevision );
    g_pScene->Information().DCCNameAndVersion = strTemp;

    ExportLog::LogMsg( 2, "Compiled against %s", strTemp );
    ExportLog::LogMsg( 1, "Loading FBX file \"%s\"...", strFileName );

    INT iFileFormat = -1;

    if( !g_pSDKManager->GetIOPluginRegistry()->DetectFileFormat( strFileName, iFileFormat ) )
    {
        iFileFormat = g_pSDKManager->GetIOPluginRegistry()->FindReaderIDByDescription( "FBX binary (*.fbx)" );
    }
    g_pImporter->SetFileFormat( iFileFormat );

    BOOL bResult = g_pImporter->Initialize( strFileName );

    if( !bResult )
    {
        ExportLog::LogError( "Could not initialize FBX importer." );
        return E_FAIL;
    }

    bResult = g_pImporter->Import( g_pFBXScene );

    if( !bResult )
    {
        ExportLog::LogError( "Could not load FBX file \"%s\".", strFileName );
        return E_FAIL;
    }

    ExportLog::LogMsg( 1, "FBX file \"%s\" was successfully loaded.", strFileName );
    g_pImporter->GetFileVersion( iMajorVersion, iMinorVersion, iRevision );
    ExportLog::LogMsg( 2, "FBX file version: %d.%d.%d", iMajorVersion, iMinorVersion, iRevision );

    ExportLog::LogMsg( 2, "Parsing scene." );

	SetBindPose();

    assert( g_pFBXScene->GetRootNode() != NULL );
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
    ParseNode( g_pFBXScene->GetRootNode(), g_pScene, matIdentity );

    ExportFrame* pRootFrame = g_pScene->GetChildByIndex(0);
    assert( pRootFrame != NULL );

    KFbxGlobalCameraSettings* pCameraSettings = &g_pFBXScene->GetGlobalCameraSettings();
    //ParseCamera( &pCameraSettings->GetCameraProducerPerspective(), g_pScene );
	if( pCameraSettings->GetCameraProducerPerspective() != NULL )
	{
		ParseNode( pCameraSettings->GetCameraProducerPerspective()->GetNode(), pRootFrame, matIdentity );
	}

    if( g_pScene->Settings().bExportAnimations )
    {
        ParseAnimation( g_pFBXScene );
        if( g_pScene->Settings().bRenameAnimationsToFileName )
        {
            ExportPath AnimName = g_CurrentOutputFileName.GetFileNameWithoutExtension();

            DWORD dwAnimCount = g_pScene->GetAnimationCount();
            for( DWORD i = 0; i < dwAnimCount; ++i )
            {
                CHAR strCurrentAnimName[MAX_PATH];
                if( i > 0 )
                {
                    sprintf_s( strCurrentAnimName, "%s%d", (const CHAR*)AnimName, i );
                }
                else
                {
                    strcpy_s( strCurrentAnimName, (const CHAR*)AnimName );
                }
                ExportAnimation* pAnim = g_pScene->GetAnimation( i );
                ExportLog::LogMsg( 4, "Renaming animation \"%s\" to \"%s\".", pAnim->GetName().SafeString(), strCurrentAnimName );
                pAnim->SetName( strCurrentAnimName );
            }
        }
    }

    return S_OK;
}
