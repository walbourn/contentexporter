//-------------------------------------------------------------------------------------
//  ParseMisc.h
//
//  Entry points for parsing less complex scene items in a FBX scene, including nodes,
//  cameras, lights, etc.
//
//  Microsoft XNA Developer Connection
//  Copyright © Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------

#pragma once

using namespace ATG;

VOID ParseNode( KFbxNode* pNode, ExportFrame* pParentFrame, const D3DXMATRIX& matParentWorld );

VOID ParseCamera( KFbxCamera* pFbxCamera, ExportFrame* pParentFrame );
VOID ParseLight( KFbxLight* pFbxLight, ExportFrame* pParentFrame );
