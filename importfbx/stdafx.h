//-------------------------------------------------------------------------------------
// stdafx.h
//
// Precompiled header for the ImportFBX project.
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

#pragma warning( disable: 4100 4296 4505 4512 4996 )

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <WindowsX.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <list>
#include <vector>
#include <hash_map>
#include <algorithm>
#include <commctrl.h>
#include <richedit.h>
#include <process.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <shellapi.h>

#include <xnamath.h>

#include "..\ExporterGlobals.h"
#include "..\ExportObjects\collision.h"
#include "..\ExportObjects\ExportXmlParser.h"
#include "..\ExportObjects\ExportPath.h"
#include "..\ExportObjects\ExportMaterial.h"
#include "..\ExportObjects\ExportObjects.h"

#include <fbxsdk.h>

#include "..\XATGFileWriter\XATGFileWriter.h"
#include "..\SDKMeshFileWriter\SDKMeshFileWriter.h"

#define CONTENT_EXPORTER_TITLE CONTENT_EXPORTER_GLOBAL_TITLE " for FBX"
extern CHAR g_strExporterName[100];

#ifndef UNUSED
#define UNUSED(x) (x)
#endif
