//-------------------------------------------------------------------------------------
// stdafx.h
//
// Precompiled header for the SDKMeshFileWriter project.
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------
#pragma once

#pragma warning( disable : 4100 4481 )

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOMCX
#define NOSERVICE
#define NOHELP

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <WindowsX.h>
#include <list>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <commctrl.h>
#include <richedit.h>
#include <process.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <shellapi.h>
#include <dxgiformat.h>

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>

#include "..\ExportObjects\ExportXmlParser.h"
#include "..\ExportObjects\ExportPath.h"
#include "..\ExportObjects\ExportMaterial.h"
#include "..\ExportObjects\ExportObjects.h"
