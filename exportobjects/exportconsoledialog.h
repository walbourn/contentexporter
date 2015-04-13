//-------------------------------------------------------------------------------------
// ExportConsoleDialog.h
//
// Implements a Win32 console dialog UI for exporter output spew.
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

#include "ExportDialogs.h"

namespace ATG
{
    class ExportConsoleDialog : public ILogListener, public ExportProgress, public ExportDialogBase
    {
    public:
        ExportConsoleDialog();

        virtual LRESULT OnCommand( WORD wNotifyCode, WORD idCtrl, HWND hwndCtrl ); 
        virtual LRESULT OnInitDialog( HWND hwndFocusCtrl );
        virtual LRESULT OnMessage( UINT uMsg, WPARAM wParam, LPARAM lParam );

        virtual VOID LogMessage( const CHAR* strMessage );
        virtual VOID LogWarning( const CHAR* strMessage );
        virtual VOID LogError( const CHAR* strMessage );
        virtual VOID LogCommand( DWORD dwCommand, VOID* pData );

        virtual void Initialize( const CHAR* strTitle );
        virtual void Terminate();
        virtual void SetCaption( const CHAR* strCaption );
        virtual void StartNewTask( const CHAR* strCaption, FLOAT fTaskPercentOfWhole );
        virtual void SetProgress( FLOAT fTaskRelativeProgress );

        VOID ConsolePrint( COLORREF rgb, const CHAR* strText );
        VOID ConsoleNewline();
        VOID ConsoleClear();

        static UINT WINAPI ThreadEntry( VOID* pData );

    protected:
        VOID RecordWarning( const CHAR* strMessage );
        VOID RecordError( const CHAR* strMessage );
        VOID PrintWarningsAndErrors();
        VOID ClearWarningsAndErrors();

    protected:
        HWND    m_hRichTextBox;
        HWND    m_hProgressText;
        HWND    m_hProgressBar;
        HWND    m_hOKButton;
        FLOAT   m_fCurrentTaskMin;
        FLOAT   m_fCurrentTaskSize;
    };
}