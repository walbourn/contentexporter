//-------------------------------------------------------------------------------------
// ExportMaterialDatabase.cpp
//
// Advanced Technology Group (ATG)
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------

#include "stdafx.h"
#include "exportmaterialdatabase.h"

namespace
{
    CHAR g_strMaterialDBFileName[MAX_PATH] = {};

    const CHAR* ConvertString(const WCHAR* strData, DWORD dwLength = 0)
    {
        static CHAR strText[256] = {};
        if (!strData)
        {
            *strText = 0;
        }
        else
        {
            const int length = (dwLength == 0) ? -1 : static_cast<int>(dwLength);
            int result = WideCharToMultiByte(CP_ACP, 0, strData, length, strText, 255, nullptr, nullptr);
            if (result <= 0)
            {
                *strText = 0;
            }
            else if (result < 256)
            {
                strText[result] = 0;
            }
        }
        return strText;
    }

    bool ConvertBool(const WCHAR* strBool, bool bDefaultValue)
    {
        if (!strBool)
            return bDefaultValue;
        const WCHAR FirstChar = strBool[0];
        return (FirstChar == L'T' || FirstChar == L't' || FirstChar == L'1');
    }

    void CopyString(const WCHAR* strData, size_t dwLength, WCHAR* strDest, size_t dwDestLength)
    {
        wcsncpy_s(strDest, dwDestLength, strData, dwLength);
        strDest[dwLength] = L'\0';
    }
}

#define MATCH_ELEMENT_NAME(x) ( _wcsicmp( m_strCurrentElementName, x ) == 0 )

namespace ATG
{
    ExportMaterialDefinitionVector  g_Materials;
}

using namespace ATG;

HRESULT MaterialDatabaseReader::ElementBegin(const WCHAR* strName, UINT NameLen, const XMLAttribute* pAttributes, UINT NumAttributes)
{
    ProcessElementBeginContent();

    CopyString(strName, NameLen, m_strCurrentElementName, ARRAYSIZE(m_strCurrentElementName));
    m_bCurrentElementEndTag = false;

    ParseAttributes(pAttributes, NumAttributes);

    return S_OK;
}

HRESULT MaterialDatabaseReader::ElementContent(const WCHAR* strData, UINT DataLen, bool More)
{
    return S_OK;
}

HRESULT MaterialDatabaseReader::ElementEnd(const WCHAR* strName, UINT NameLen)
{
    ProcessElementBeginContent();

    CopyString(strName, NameLen, m_strCurrentElementName, ARRAYSIZE(m_strCurrentElementName));
    m_bCurrentElementEndTag = true;

    ProcessElementEnd();

    m_strCurrentElementName[0] = L'\0';

    return S_OK;
}

void MaterialDatabaseReader::ParseAttributes(const XMLAttribute* pAttributes, size_t dwAttributeCount)
{
    m_CurrentElementAttributes.clear();
    for (size_t i = 0; i < dwAttributeCount; ++i)
    {
        ElementAttribute ea;
        CopyString(pAttributes[i].strName, pAttributes[i].NameLen, ea.strName, ARRAYSIZE(ea.strName));
        CopyString(pAttributes[i].strValue, pAttributes[i].ValueLen, ea.strValue, ARRAYSIZE(ea.strValue));
        m_CurrentElementAttributes.push_back(ea);
    }
}

const WCHAR* MaterialDatabaseReader::FindAttribute(const WCHAR* strName)
{
    const size_t dwCount = m_CurrentElementAttributes.size();
    for (size_t i = 0; i < dwCount; ++i)
    {
        if (_wcsicmp(strName, m_CurrentElementAttributes[i].strName) == 0)
            return m_CurrentElementAttributes[i].strValue;
    }
    return nullptr;
}

void MaterialDatabaseReader::Error(HRESULT hError, const CHAR* strMessage)
{

}

ExportMaterialParameterType ConvertType(const WCHAR* strType)
{
    if (!strType)
        return MPT_STRING;

    if (_wcsicmp(strType, L"bool") == 0)
        return MPT_BOOL;
    else if (_wcsicmp(strType, L"texture2d") == 0)
        return MPT_TEXTURE2D;
    else if (_wcsicmp(strType, L"float4") == 0)
        return MPT_FLOAT4;
    else if (_wcsicmp(strType, L"float2") == 0)
        return MPT_FLOAT2;
    else if (_wcsicmp(strType, L"float3") == 0)
        return MPT_FLOAT3;
    else if (_wcsicmp(strType, L"float") == 0)
        return MPT_FLOAT;
    else if (_wcsicmp(strType, L"integer") == 0)
        return MPT_INTEGER;
    else if (_wcsicmp(strType, L"texturecube") == 0)
        return MPT_TEXTURECUBE;
    else if (_wcsicmp(strType, L"texturevolume") == 0)
        return MPT_TEXTUREVOLUME;
    else if (_wcsicmp(strType, L"texture") == 0)
        return MPT_TEXTURE2D;

    return MPT_STRING;
}

void MaterialDatabaseReader::ProcessElementBeginContent()
{
    if (MATCH_ELEMENT_NAME(L"Material"))
    {
        m_pCurrentMaterial = nullptr;

        const WCHAR* strName = FindAttribute(L"Name");
        if (!strName)
            return;

        m_pCurrentMaterial = new ExportMaterialDefinition();
        m_pCurrentMaterial->strName = ConvertString(strName);

        const WCHAR* strDesc = FindAttribute(L"Description");
        if (strDesc)
            m_pCurrentMaterial->strDescription = ConvertString(strDesc);

        g_Materials.push_back(m_pCurrentMaterial);
        return;
    }
    else if (MATCH_ELEMENT_NAME(L"Parameter"))
    {
        if (!m_pCurrentMaterial)
            return;

        if (m_pCurrentParam)
            return;

        const WCHAR* strName = FindAttribute(L"Name");
        if (!strName)
            return;

        m_pCurrentParam = new ExportMaterialParameterDefinition();
        m_pCurrentParam->strName = ConvertString(strName);

        m_pCurrentMaterial->Parameters.push_back(m_pCurrentParam);

        const WCHAR* strDisplayName = FindAttribute(L"DisplayName");
        if (strDisplayName)
            m_pCurrentParam->strDisplayName = ConvertString(strDisplayName);
        else
            m_pCurrentParam->strDisplayName = m_pCurrentParam->strName;

        const WCHAR* strDesc = FindAttribute(L"Description");
        m_pCurrentParam->strDescription = ConvertString(strDesc);

        const WCHAR* strDisplayHint = FindAttribute(L"DisplayHint");
        if (!strDisplayHint || wcslen(strDisplayHint) < 1)
            m_pCurrentParam->strDisplayHint = " ";
        else
            m_pCurrentParam->strDisplayHint = ConvertString(strDisplayHint);

        const WCHAR* strLoaderHint = FindAttribute(L"LoadHint");
        m_pCurrentParam->strLoaderHint = ConvertString(strLoaderHint);

        const WCHAR* strType = FindAttribute(L"Type");
        m_pCurrentParam->ParamType = ConvertType(strType);

        const WCHAR* strVisible = FindAttribute(L"ToolVisible");
        m_pCurrentParam->bVisibleInTool = ConvertBool(strVisible, false);

        const WCHAR* strExport = FindAttribute(L"Export");
        m_pCurrentParam->bExportToContentFile = ConvertBool(strExport, true);

        const WCHAR* strDetectAlpha = FindAttribute(L"DetectAlpha");
        m_pCurrentParam->bDetectAlpha = ConvertBool(strDetectAlpha, false);

        const WCHAR* strDefaultValue = FindAttribute(L"DefaultValue");
        m_pCurrentParam->strDefaultValue = ConvertString(strDefaultValue);
        return;
    }
}

void MaterialDatabaseReader::ProcessElementEnd()
{
    if (MATCH_ELEMENT_NAME(L"Material"))
    {
        assert(m_pCurrentMaterial != nullptr);
        assert(m_pCurrentParam == nullptr);
        m_pCurrentMaterial = nullptr;
    }
    else if (MATCH_ELEMENT_NAME(L"Parameter"))
    {
        assert(m_pCurrentMaterial != nullptr);
        assert(m_pCurrentParam != nullptr);
        m_pCurrentParam = nullptr;
    }
}

ExportMaterialDefinition::~ExportMaterialDefinition()
{
    const size_t dwCount = Parameters.size();
    for (size_t i = 0; i < dwCount; ++i)
        delete Parameters[i];
    Parameters.clear();
}

void ExportMaterialDatabase::Clear()
{
    for (size_t i = 0; i < g_Materials.size(); ++i)
    {
        delete g_Materials[i];
    }
    g_Materials.clear();
}

bool ExportMaterialDatabase::Initialize(const CHAR* strFileName)
{
    MaterialDatabaseReader mdr;
    XMLParser xp;
    xp.RegisterSAXCallbackInterface(&mdr);
    const HRESULT hr = xp.ParseXMLFile(strFileName);
    if (hr == S_OK)
    {
        strcpy_s(g_strMaterialDBFileName, strFileName);
    }
    return (hr == S_OK);
}

const CHAR* ExportMaterialDatabase::GetDatabaseFileName()
{
    return g_strMaterialDBFileName;
}

size_t ExportMaterialDatabase::GetMaterialCount()
{
    return g_Materials.size();
}

const ExportMaterialDefinition* ExportMaterialDatabase::GetMaterial(size_t dwIndex)
{
    assert(dwIndex < GetMaterialCount());
    return g_Materials[dwIndex];
}

const ExportMaterialDefinition* ExportMaterialDatabase::FindMaterial(ExportString strName)
{
    const size_t dwCount = GetMaterialCount();
    for (size_t i = 0; i < dwCount; ++i)
    {
        if (g_Materials[i]->strName == strName)
            return g_Materials[i];
    }
    return nullptr;
}
