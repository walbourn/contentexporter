//-------------------------------------------------------------------------------------
// ParseMesh.h
//
// Entry points for FBX mesh parsing.
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------
#pragma once

void ParseMesh( FbxNode* pNode, FbxMesh* pFbxMesh, ATG::ExportFrame* pParentFrame, bool bSubDProcess = false, const CHAR* strSuffix = nullptr );
void ParseSubDiv( FbxNode* pNode, const FbxSubDiv* pFbxSubD, ATG::ExportFrame* pParentFrame );
