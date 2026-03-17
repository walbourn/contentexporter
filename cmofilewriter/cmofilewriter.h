//-------------------------------------------------------------------------------------
// CMOFileWriter.h
//
// Entry point for writing Compiled Mesh Object (CMO) files.  This file writer takes
// data from the ExportScene stored in a global variable (g_pScene).
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

    bool WriteCMOMeshFile(const CHAR* strFileName, ExportManifest* pManifest);
}
