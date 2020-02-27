//-------------------------------------------------------------------------------------
// ExportConsoleDialog.h
//
// Implements a Win32 console dialog UI for exporter output spew.
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------
#pragma once

#include "ExportDialogs.h"

namespace ATG
{
    class ExportConsoleDialog : public ILogListener, public ExportProgress, public ExportDialogBase
    {
    public:
        ExportConsoleDialog();

        LRESULT OnCommand(WORD wNotifyCode, WORD idCtrl, HWND hwndCtrl) override;
        LRESULT OnInitDialog(HWND hwndFocusCtrl) override;
        LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

        void LogMessage(const CHAR* strMessage) override;
        void LogWarning(const CHAR* strMessage) override;
        void LogError(const CHAR* strMessage) override;
        void LogCommand(DWORD dwCommand, void* pData) override;

        void Initialize(const CHAR* strTitle) override;
        void Terminate() override;
        void SetCaption(const CHAR* strCaption) override;
        void StartNewTask(const CHAR* strCaption, float fTaskPercentOfWhole) override;
        void SetProgress(float fTaskRelativeProgress) override;

        void ConsolePrint(COLORREF rgb, const CHAR* strText);
        void ConsoleNewline();
        void ConsoleClear();

        static UINT WINAPI ThreadEntry(void* pData);

    protected:
        void RecordWarning(const CHAR* strMessage);
        void RecordError(const CHAR* strMessage);
        void PrintWarningsAndErrors();
        void ClearWarningsAndErrors();

    protected:
        HWND    m_hRichTextBox;
        HWND    m_hProgressText;
        HWND    m_hProgressBar;
        HWND    m_hOKButton;
        float   m_fCurrentTaskMin;
        float   m_fCurrentTaskSize;
    };
}