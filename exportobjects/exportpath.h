//-------------------------------------------------------------------------------------
// ExportPath.h
//
// The ExportPath class manages a path string, representing a relative or absolute
// path.  Common operations such as replacing the filename, replacing the extension,
// and appending paths are supported.
//
// Advanced Technology Group (ATG)
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------
#pragma once

class ExportPath
{
protected:
    CHAR m_strPath[MAX_PATH];
    CHAR* m_strExtension;  // m_strExtension points to the period before the extension.
    CHAR* m_strFileName;   // m_strFileName points to the first character in the filename, following the slash.

public:
    ExportPath();
    ExportPath(const CHAR* strPath);
    ExportPath(const ExportPath& OtherPath);

    void SetPathAndFileName(const CHAR* strPath);
    void SetPathOnly(const CHAR* strPath);

    ExportPath& operator=(const ExportPath& OtherPath);
    operator const CHAR* () const noexcept { return m_strPath; }

    static ExportPath GetTempPath();
    static ExportPath GetCurrentPath();

    ExportPath GetDirectory() const;
    ExportPath GetFileName() const;
    ExportPath GetFileNameWithoutExtension() const;

    bool HasExtension() const noexcept { return m_strExtension != nullptr; }
    bool HasFileName() const;
    const CHAR* GetExtension() const;
    bool IsAbsolutePath() const;
    bool IsEmpty() const noexcept { return m_strPath[0] == '\0'; }

    void ChangeExtension(const CHAR* strExtension);
    void ChangeFileName(const CHAR* strFileName);
    void ChangeFileName(const ExportPath& OtherPath);
    void ChangeFileNameWithExtension(const CHAR* strFileName);
    void ChangeFileNameWithExtension(const ExportPath& OtherPath);
    void Append(const ExportPath& OtherPath);
    void AppendToFileName(const CHAR* strText);
    void TrimOffFileName();

protected:
    void Initialize(const CHAR* strPath);
};
