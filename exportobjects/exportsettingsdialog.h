//-------------------------------------------------------------------------------------
// ExportSettingsDialog.h
//
// Implements a GUI for modifying the settings variables.
//
// Advanced Technology Group (ATG)
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------
#pragma once

#include "ExportDialogs.h"

namespace ATG
{
    class ExportSettingsDialog : public ExportDialogBase
    {
    public:
        enum DialogState
        {
            DS_HIDDEN_UNKNOWN,
            DS_HIDDEN_CANCELED,
            DS_HIDDEN_OK,
            DS_VISIBLE
        };
        ExportSettingsDialog();

        LRESULT OnCommand(WORD wNotifyCode, WORD idCtrl, HWND hwndCtrl) override;
        LRESULT OnInitDialog(HWND hwndFocusCtrl) override;
        LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
        LRESULT OnNotify(INT idCtrl, LPNMHDR pnmh) override;

        static UINT WINAPI ThreadEntry(void* pData);
        static LRESULT CALLBACK ScrollPaneWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        void Show() override;
        DialogState GetDialogState() const { return m_DialogState; }

    protected:
        void PopulateCategories(const ExportSettingsEntry* pEntry = nullptr, void* hParentItem = nullptr);
        void PopulateControls();
        void UpdateVScroll();
        void SynchronizeControlUI(HWND hwndControl);

    public:
        WNDPROC     m_pStaticWndProc;

    protected:
        HWND    m_hCategoryTree;
        HWND    m_hOKButton;
        HWND    m_hCancelButton;

        HWND    m_hScrollingPane;

        DWORD   m_dwControlsHeight;
        ExportSettingsEntry* m_pCurrentCategory;
        DialogState   m_DialogState;
        bool    m_bControlDataUpdate;
    };
}
