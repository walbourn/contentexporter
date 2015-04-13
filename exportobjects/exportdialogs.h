//-------------------------------------------------------------------------------------
// ExportDialogs.h
//
// Entry points for various GUI dialogs.
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

#include "ExportDialogUtils.h"

namespace ATG
{
    VOID InitializeExportDialogs( const CHAR* strTitle, HWND hParentWindow, HINSTANCE hInst );
    VOID TerminateExportDialogs();

    VOID ShowConsoleDialog();
    BOOL ShowSettingsDialog( BOOL bModal );

    class ExportDialogBase : public ThinDialog
    {
    public:
        virtual VOID Show();
        virtual VOID Hide();
    };
}