//-------------------------------------------------------------------------------------
// ExportLog.h
//
// Classes and interfaces for a DCC-independent pluggable message logging system.
// The system supports warnings, errors, and different levels of message logging.
// Two log listeners are implemented here - a debug spew listener and a file listener.
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

namespace ATG
{

class ILogListener
{
public:
    virtual VOID LogMessage( const CHAR* strMessage ) = 0;
    virtual VOID LogWarning( const CHAR* strMessage ) { LogMessage( strMessage ); }
    virtual VOID LogError( const CHAR* strMessage ) { LogMessage( strMessage ); }
    virtual VOID LogCommand( DWORD dwCommand, VOID* pData ) { UNREFERENCED_PARAMETER( dwCommand ); UNREFERENCED_PARAMETER( pData ); }
};

class DebugSpewListener : public ILogListener
{
public:
    virtual VOID LogMessage( const CHAR* strMessage )
    {
        OutputDebugStringA( strMessage );
        OutputDebugStringA( "\n" );
    }
};

class FileListener : public ILogListener
{
public:
    FileListener();
    ~FileListener();
    VOID StartLogging( const CHAR* strFileName );
    VOID StopLogging();
    virtual VOID LogMessage( const CHAR* strMessage );
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

    static VOID EnableLogging( BOOL bEnable );
    static VOID SetLogLevel( UINT uLevel );
    static UINT GetLogLevel();
    static VOID AddListener( ILogListener* pListener );
    static VOID ClearListeners();

    static VOID GenerateLogReport( BOOL bEchoWarningsAndErrors = TRUE );
    static VOID ResetCounters();

    static VOID LogCommand( DWORD dwCommand, VOID* pData = NULL );
    static VOID LogError( const CHAR* strFormat, ... );
    static VOID LogWarning( const CHAR* strFormat, ... );
    static VOID LogMsg( UINT uImportance, const CHAR* strFormat, ... );
};

};
