//-------------------------------------------------------------------------------------
// ParseMesh.h
//
// Entry points for FBX mesh parsing.
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

VOID ParseMesh( FbxNode* pNode, FbxMesh* pFbxMesh, ExportFrame* pParentFrame, BOOL bSubDProcess = FALSE, const CHAR* strSuffix = NULL );
VOID ParseSubDiv( FbxNode* pNode, FbxSubDiv* pFbxSubD, ExportFrame* pParentFrame );
