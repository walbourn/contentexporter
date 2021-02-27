//-------------------------------------------------------------------------------------
// XATGFileWriter.h
//  
// Writes files in the XATG file format, from data stored in the export objects hierarchy.
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

    bool WriteXATGFile(const CHAR* strFileName, ExportManifest* pManifest);

    void BundleTextures();

    struct XATGExportSettings
    {
        bool    bBundleTextures;
        bool    bUseExistingBundle;
        bool    bBinaryBlobExport;
    };

    void XATGInitializeSettings();

}
