//-------------------------------------------------------------------------------------
// stdafx.h
//
// Precompiled header for the ImportFBX project.
//
// Advanced Technology Group (ATG)
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------
#pragma once

#pragma warning( disable : 4100 4296 4481 4505 4512 4996 )

#pragma warning (disable : 26400 26401 26409 26426 26429 26432 26440 26446 26447 26451 26455 26462 26472 26475 26476 26481 26482 26485 26486 26487 26489 26490 26492 26493 26812 26814)

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOMCX
#define NOSERVICE
#define NOHELP

#include <Windows.h>
#include <WindowsX.h>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cwchar>
#include <exception>
#include <iterator>
#include <list>
#include <memory>
#include <new>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <commctrl.h>
#include <richedit.h>
#include <process.h>
#include <shellapi.h>
#include <dxgiformat.h>

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>

#include "..\ExporterGlobals.h"
#include "..\ExportObjects\ExportXmlParser.h"
#include "..\ExportObjects\ExportPath.h"
#include "..\ExportObjects\ExportMaterial.h"
#include "..\ExportObjects\ExportObjects.h"

#pragma warning(push)
#pragma warning( disable : 4616 6011 )
#include <fbxsdk.h>
#pragma warning(pop)

#include "..\SDKMeshFileWriter\SDKMeshFileWriter.h"
#include "..\XATGFileWriter\XATGFileWriter.h"

#define CONTENT_EXPORTER_TITLE CONTENT_EXPORTER_GLOBAL_TITLE " for FBX"
extern CHAR g_strExporterName[100];

#ifndef UNUSED
#define UNUSED(x) (x)
#endif
