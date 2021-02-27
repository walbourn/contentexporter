//-------------------------------------------------------------------------------------
// ParseMisc.h
//
// Entry points for parsing less complex scene items in a FBX scene, including nodes,
// cameras, lights, etc.
//
// Advanced Technology Group (ATG)
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------
#pragma once

void ParseNode(FbxNode* pNode, ATG::ExportFrame* pParentFrame, DirectX::CXMMATRIX matParentWorld);

void ParseCamera(FbxCamera* pFbxCamera, ATG::ExportFrame* pParentFrame);
void ParseLight(FbxLight* pFbxLight, ATG::ExportFrame* pParentFrame);

void FixupNode(ATG::ExportFrame* pFrame, DirectX::CXMMATRIX matParentWorld);

typedef std::unordered_map<FbxNode*, FbxMatrix> PoseMap;
extern PoseMap g_BindPoseMap;
extern bool g_bBindPoseFixupRequired;

