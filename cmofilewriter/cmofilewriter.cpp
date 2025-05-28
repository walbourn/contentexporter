//-------------------------------------------------------------------------------------
// CMOFileWriter.cpp
//
// Advanced Technology Group (ATG)
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------
#include "stdafx.h"
#include "CMOFileWriter.h"
#include "CMO.h"

extern ATG::ExportScene* g_pScene;

using namespace DirectX;
using namespace ATG;

bool ATG::WriteCMOMeshFile(const CHAR* strFileName, ExportManifest* pManifest)
{
    if (!g_pScene)
        return false;

    ExportLog::LogError("CMO not yet implemented");

    // TODO
    return false;
}