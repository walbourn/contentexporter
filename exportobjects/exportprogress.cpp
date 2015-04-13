//-------------------------------------------------------------------------------------
// ExportProgress.cpp
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

#include "stdafx.h"
#include "exportprogress.h"

namespace ATG
{
    VOID ExportProgress::Initialize( CONST CHAR* strTitle )
    {
    }

    VOID ExportProgress::Terminate()
    {
    }

    VOID ExportProgress::StartNewTask( CONST CHAR* strCaption, FLOAT fTaskPercentOfWhole )
    {
    }

    VOID ExportProgress::SetCaption( CONST CHAR* strCaption )
    {
    }

    VOID ExportProgress::SetProgress( FLOAT fTaskRelativeProgress )
    {
    }

    ExportProgress g_NullProgressBar;
    ExportProgress* g_pProgress = &g_NullProgressBar;
}

