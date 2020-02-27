//-------------------------------------------------------------------------------------
// ExportDialogs.h
//
// Entry points for various GUI dialogs.
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------
#pragma once

#include "ExportDialogUtils.h"

namespace ATG
{
    void InitializeExportDialogs(const CHAR* strTitle, HWND hParentWindow, HINSTANCE hInst);
    void TerminateExportDialogs();

    void ShowConsoleDialog();
    bool ShowSettingsDialog(bool bModal);

    class ExportDialogBase : public ThinDialog
    {
    public:
        virtual void Show();
        virtual void Hide();
    };
}