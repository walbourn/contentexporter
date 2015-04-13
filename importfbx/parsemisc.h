//-------------------------------------------------------------------------------------
// ParseMisc.h
//
// Entry points for parsing less complex scene items in a FBX scene, including nodes,
// cameras, lights, etc.
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

VOID ParseNode( FbxNode* pNode, ExportFrame* pParentFrame, const D3DXMATRIX& matParentWorld );

VOID ParseCamera( FbxCamera* pFbxCamera, ExportFrame* pParentFrame );
VOID ParseLight( FbxLight* pFbxLight, ExportFrame* pParentFrame );

VOID FixupNode( ExportFrame* pFrame, const D3DXMATRIX& matParentWorld );

typedef stdext::hash_map<FbxNode*,FbxMatrix> PoseMap;
extern PoseMap g_BindPoseMap;
extern BOOL g_bBindPoseFixupRequired;

