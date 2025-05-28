//-------------------------------------------------------------------------------------
// stdafx.h
//
// Precompiled header for the CMOFileWriter project.
//
// Advanced Technology Group (ATG)
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------
#pragma once

#pragma warning( disable : 4100 4481 )

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
#include <list>
#include <memory>
#include <string>
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

#include "..\ExportObjects\ExportXmlParser.h"
#include "..\ExportObjects\ExportPath.h"
#include "..\ExportObjects\ExportMaterial.h"
#include "..\ExportObjects\ExportObjects.h"
