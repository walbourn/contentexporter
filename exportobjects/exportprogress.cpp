//-------------------------------------------------------------------------------------
// ExportProgress.cpp
//  
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------

#include "stdafx.h"
#include "exportprogress.h"

namespace ATG
{
    ExportProgress g_NullProgressBar;
    ExportProgress* g_pProgress = &g_NullProgressBar;
}

using namespace ATG;

void ExportProgress::Initialize(const CHAR* strTitle)
{
}

void ExportProgress::Terminate()
{
}

void ExportProgress::StartNewTask(const CHAR* strCaption, float fTaskPercentOfWhole)
{
}

void ExportProgress::SetCaption(const CHAR* strCaption)
{
}

void ExportProgress::SetProgress(float fTaskRelativeProgress)
{
}
