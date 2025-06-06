//-------------------------------------------------------------------------------------
// ExportProgress.h
//
// A DCC-agnostic system for reporting progress of the export operation.
// The export front-end implements a subclass of ExportProgress that manipulates the
// DCC user interface, and sets up the g_pProgress global pointer so the DCC-agnostic
// code can report progress updates.
//
// Advanced Technology Group (ATG)
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------
#pragma once

namespace ATG
{
    class ExportProgress
    {
    public:
        virtual ~ExportProgress() = default;

        virtual void Initialize(const CHAR* strTitle);
        virtual void Terminate();

        virtual void SetCaption(const CHAR* strCaption);
        virtual void StartNewTask(const CHAR* strCaption, float fTaskPercentOfWhole);
        virtual void SetProgress(float fTaskRelativeProgress);
    };

    extern ExportProgress* g_pProgress;
}

