//-------------------------------------------------------------------------------------
// CMOFileWriter.h
//
// Entry point for writing SDKMESH files.  This file writer takes data from the
// ExportScene stored in a global variable (g_pScene).
//
// Advanced Technology Group (ATG)
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------
#pragma once

namespace ATG
{

    class ExportManifest;

    bool WriteCMOFile(const CHAR* strFileName, ExportManifest* pManifest);

}
