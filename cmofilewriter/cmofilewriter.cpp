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

extern ATG::ExportScene* g_pScene;

using namespace DirectX;

namespace ATG
{
    bool WriteCMOFile(const CHAR* strFileName, ExportManifest* pManifest)
    {
        if (!g_pScene)
            return false;

        // TODO:
        return false;
    }
}
