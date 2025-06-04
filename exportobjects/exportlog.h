//-------------------------------------------------------------------------------------
// ExportLog.h
//
// Classes and interfaces for a DCC-independent pluggable message logging system.
// The system supports warnings, errors, and different levels of message logging.
// Two log listeners are implemented here - a debug spew listener and a file listener.
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

    class ILogListener
    {
    public:
        virtual ~ILogListener() = default;

        virtual void LogMessage(const CHAR* strMessage) = 0;
        virtual void LogWarning(const CHAR* strMessage) { LogMessage(strMessage); }
        virtual void LogError(const CHAR* strMessage) { LogMessage(strMessage); }
        virtual void LogCommand(DWORD dwCommand, void* pData) { UNREFERENCED_PARAMETER(dwCommand); UNREFERENCED_PARAMETER(pData); }
    };

    class DebugSpewListener : public ILogListener
    {
    public:
        void LogMessage(const CHAR* strMessage) override
        {
            OutputDebugStringA(strMessage);
            OutputDebugStringA("\n");
        }
    };

    class FileListener : public ILogListener
    {
    public:
        FileListener();
        ~FileListener();
        void StartLogging(const CHAR* strFileName);
        void StopLogging();

        void LogMessage(const CHAR* strMessage) override;
    protected:
        HANDLE      m_hFileHandle;
    };

    class ExportLog
    {
    public:
        enum LogCommands
        {
            ELC_CLEAR = 0,
            ELC_STARTEXPORT,
            ELC_ENDEXPORT
        };

        static void EnableLogging(bool bEnable);
        static void SetLogLevel(UINT uLevel);
        static UINT GetLogLevel();
        static void AddListener(ILogListener* pListener);
        static void ClearListeners();

        static bool GenerateLogReport(bool bEchoWarningsAndErrors = true);
        static void ResetCounters();

        static void LogCommand(DWORD dwCommand, void* pData = nullptr);
        static void LogError(_In_z_ _Printf_format_string_ const CHAR* strFormat, ...);
        static void LogWarning(_In_z_ _Printf_format_string_ const CHAR* strFormat, ...);
        static void LogMsg(UINT uImportance, _In_z_ _Printf_format_string_ const CHAR* strFormat, ...);
    };

};
