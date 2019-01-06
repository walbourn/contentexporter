//-------------------------------------------------------------------------------------
// SDKMeshFileWriter.h
//
// Entry point for writing SDKMESH files.  This file writer takes data from the
// ExportScene stored in a global variable (g_pScene).
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------
#pragma once

namespace ATG
{

class ExportManifest;

bool WriteSDKMeshFile( const CHAR* strFileName, ExportManifest* pManifest, bool version2 );

}
