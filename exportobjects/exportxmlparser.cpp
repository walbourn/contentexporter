//-------------------------------------------------------------------------------------
// ExportXmlParser.cpp
//  
// Simple callback non-validating XML parser implementation.
//
// Advanced Technology Group (ATG)
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------

#include "stdafx.h"
#include "ExportXmlParser.h"

using namespace ATG;

//-------------------------------------------------------------------------------------
// Name: XMLParser::XMLParser
//-------------------------------------------------------------------------------------
XMLParser::XMLParser() :
    m_pISAXCallback(nullptr),
    m_hFile(INVALID_HANDLE_VALUE),
    m_pInXMLBuffer(nullptr),
    m_uInXMLBufferCharsLeft(0),
    m_dwCharsTotal(0),
    m_dwCharsConsumed(0),
    m_pReadBuf{},
    m_pWriteBuf{},
    m_pReadPtr(m_pReadBuf),
    m_pWritePtr(m_pWriteBuf),
    m_bUnicode(false),
    m_bReverseBytes(false),
    m_bSkipNextAdvance(false),
    m_Ch(0)
{
}

//-------------------------------------------------------------------------------------
// Name: XMLParser::~XMLParser
//-------------------------------------------------------------------------------------
XMLParser::~XMLParser()
{
}


//-------------------------------------------------------------------------------------
// Name: XMLParser::FillBuffer
// Desc: Reads a block from the current open file
//-------------------------------------------------------------------------------------
void XMLParser::FillBuffer()
{
    DWORD NChars = 0;

    m_pReadPtr = m_pReadBuf;

    if (!m_hFile)
    {
        if (m_uInXMLBufferCharsLeft > XML_READ_BUFFER_SIZE)
            NChars = XML_READ_BUFFER_SIZE;
        else
            NChars = m_uInXMLBufferCharsLeft;

        CopyMemory(m_pReadBuf, m_pInXMLBuffer, NChars);
        m_uInXMLBufferCharsLeft -= NChars;
        m_pInXMLBuffer += NChars;
    }
    else
    {
        std::ignore = ReadFile(m_hFile, m_pReadBuf, XML_READ_BUFFER_SIZE, &NChars, nullptr);
    }

    m_dwCharsConsumed += NChars;
    const int64_t iProgress = (int64_t(m_dwCharsConsumed) * 1000) / int64_t(m_dwCharsTotal);
    m_pISAXCallback->SetParseProgress(static_cast<DWORD>(iProgress));

    m_pReadBuf[NChars] = '\0';
    m_pReadBuf[NChars + 1] = '\0';
}


//-------------------------------------------------------------------------------------
// Name: XMLParser::SkipNextAdvance
// Desc: Puts the last character read back on the input stream
//-------------------------------------------------------------------------------------
void XMLParser::SkipNextAdvance()
{
    m_bSkipNextAdvance = true;
}


//-------------------------------------------------------------------------------------
// Name: XMLParser::ConsumeSpace
// Desc: Skips spaces in the current stream 
//-------------------------------------------------------------------------------------
HRESULT XMLParser::ConsumeSpace()
{
    HRESULT hr;

    // Skip spaces
    if (FAILED(hr = AdvanceCharacter()))
        return hr;

    while ((m_Ch == ' ') || (m_Ch == '\t') ||
        (m_Ch == '\n') || (m_Ch == '\r'))
    {
        if (FAILED(hr = AdvanceCharacter()))
            return hr;
    }
    SkipNextAdvance();
    return S_OK;
}


//-------------------------------------------------------------------------------------
// Name: XMLParser::ConvertEscape
// Desc: Copies and converts an escape sequence into m_pWriteBuf 
//-------------------------------------------------------------------------------------
HRESULT XMLParser::ConvertEscape()
{
    HRESULT hr = S_OK;
    WCHAR wVal = 0;

    if (FAILED(hr = AdvanceCharacter()))
        return hr;

    // all escape sequences start with &, so ignore the first character    

    if (FAILED(hr = AdvanceCharacter()))
        return hr;

    if (m_Ch == '#')     // character as hex or decimal
    {
        if (FAILED(hr = AdvanceCharacter()))
            return hr;
        if (m_Ch == 'x')     // hex number
        {
            if (FAILED(hr = AdvanceCharacter()))
                return hr;

            while (m_Ch != ';')
            {
                wVal *= 16;

                if ((m_Ch >= '0') && (m_Ch <= '9'))
                {
                    wVal += m_Ch - '0';
                }
                else if ((m_Ch >= 'a') && (m_Ch <= 'f'))
                {
                    wVal += m_Ch - 'a' + 10;
                }
                else if ((m_Ch >= 'A') && (m_Ch <= 'F'))
                {
                    wVal += m_Ch - 'A' + 10;
                }
                else
                {
                    Error(E_INVALID_XML_SYNTAX, "Expected hex digit as part of &#x escape sequence");
                    return E_INVALID_XML_SYNTAX;
                }

                if (FAILED(hr = AdvanceCharacter()))
                    return hr;
            }
        }
        else                    // decimal number
        {
            while (m_Ch != ';')
            {
                wVal *= 10;

                if ((m_Ch >= '0') && (m_Ch <= '9'))
                {
                    wVal += m_Ch - '0';
                }
                else
                {
                    Error(E_INVALID_XML_SYNTAX, "Expected decimal digit as part of &# escape sequence");
                    return E_INVALID_XML_SYNTAX;
                }

                if (FAILED(hr = AdvanceCharacter()))
                    return hr;
            }
        }

        // copy character into the buffer
        m_Ch = wVal;

        return S_OK;
    }

    // must be an entity reference

    WCHAR* pEntityRefVal = m_pWritePtr;

    SkipNextAdvance();
    if (FAILED(hr = AdvanceName()))
        return hr;

    const UINT EntityRefLen = static_cast<UINT>(m_pWritePtr - pEntityRefVal);
    m_pWritePtr = pEntityRefVal;

    if (EntityRefLen == 0)
    {
        Error(E_INVALID_XML_SYNTAX, "Expecting entity name after &");
        return E_INVALID_XML_SYNTAX;
    }

    if (!wcsncmp(pEntityRefVal, L"lt", EntityRefLen))
        wVal = '<';
    else if (!wcsncmp(pEntityRefVal, L"gt", EntityRefLen))
        wVal = '>';
    else if (!wcsncmp(pEntityRefVal, L"amp", EntityRefLen))
        wVal = '&';
    else if (!wcsncmp(pEntityRefVal, L"apos", EntityRefLen))
        wVal = '\'';
    else if (!wcsncmp(pEntityRefVal, L"quot", EntityRefLen))
        wVal = '"';
    else
    {
        Error(E_INVALID_XML_SYNTAX, "Unrecognized entity name after & - (should be lt, gt, amp, apos, or quot)");
        return E_INVALID_XML_SYNTAX;   // return false if unrecognized token sequence
    }

    if (FAILED(hr = AdvanceCharacter()))
        return hr;

    if (m_Ch != ';')
    {
        Error(E_INVALID_XML_SYNTAX, "Expected terminating ; for entity reference");
        return E_INVALID_XML_SYNTAX;   // malformed reference - needs terminating ;
    }

    m_Ch = wVal;
    return S_OK;
}


//-------------------------------------------------------------------------------------
// Name: XMLParser::AdvanceAttrVal
// Desc: Copies an attribute value into m_pWrite buf, skipping surrounding quotes
//-------------------------------------------------------------------------------------
HRESULT XMLParser::AdvanceAttrVal()
{
    HRESULT hr;

    if (FAILED(hr = AdvanceCharacter()))
        return hr;

    if ((m_Ch != '"') && (m_Ch != '\''))
    {
        Error(E_INVALID_XML_SYNTAX, "Attribute values must be enclosed in quotes");
        return E_INVALID_XML_SYNTAX;
    }

    const WCHAR wQuoteChar = m_Ch;

    for (;; )
    {
        if (FAILED(hr = AdvanceCharacter()))
            return hr;
        else if (m_Ch == wQuoteChar)
            break;
        else if (m_Ch == '&')
        {
            SkipNextAdvance();
            if (FAILED(hr = ConvertEscape()))
                return hr;
        }
        else if (m_Ch == '<')
        {
            Error(E_INVALID_XML_SYNTAX, "Illegal character '<' in element tag");
            return E_INVALID_XML_SYNTAX;
        }

        // copy character into the buffer

        if (m_pWritePtr - m_pWriteBuf >= XML_WRITE_BUFFER_SIZE)
        {
            Error(E_INVALID_XML_SYNTAX, "Total element tag size may not be more than %d characters", XML_WRITE_BUFFER_SIZE);
            return E_INVALID_XML_SYNTAX;
        }

        *m_pWritePtr = m_Ch;
        m_pWritePtr++;
    }
    return S_OK;
}


//-------------------------------------------------------------------------------------
// Name: XMLParser::AdvanceName
// Desc: Copies a name into the m_pWriteBuf - returns true on success, false on failure
//       Ignores leading whitespace.  Currently does not support unicode names
//-------------------------------------------------------------------------------------
HRESULT XMLParser::AdvanceName()
{
    HRESULT hr;

    if (FAILED(hr = AdvanceCharacter()))
        return hr;

    if (((m_Ch < 'A') || (m_Ch > 'Z')) &&
        ((m_Ch < 'a') || (m_Ch > 'z')) &&
        (m_Ch != '_') && (m_Ch != ':'))
    {
        Error(E_INVALID_XML_SYNTAX, "Names must start with an alphabetic character or _ or :");
        return E_INVALID_XML_SYNTAX;
    }

    while (((m_Ch >= 'A') && (m_Ch <= 'Z')) ||
        ((m_Ch >= 'a') && (m_Ch <= 'z')) ||
        ((m_Ch >= '0') && (m_Ch <= '9')) ||
        (m_Ch == '_') || (m_Ch == ':') ||
        (m_Ch == '-') || (m_Ch == '.'))
    {

        if (m_pWritePtr - m_pWriteBuf >= XML_WRITE_BUFFER_SIZE)
        {
            Error(E_INVALID_XML_SYNTAX, "Total element tag size may not be more than %d characters", XML_WRITE_BUFFER_SIZE);
            return E_INVALID_XML_SYNTAX;
        }

        *m_pWritePtr = m_Ch;
        m_pWritePtr++;

        if (FAILED(hr = AdvanceCharacter()))
            return hr;
    }

    SkipNextAdvance();
    return S_OK;
}


//-------------------------------------------------------------------------------------
// Name: XMLParser::AdvanceCharacter
// Desc: Copies the character at *m_pReadPtr to m_Ch
//       handling difference in UTF16 / UTF8, and big/little endian
//       and getting another chunk of the file if needed
//       Returns S_OK if there are more characters, E_ABORT for no characters to read
//-------------------------------------------------------------------------------------
HRESULT XMLParser::AdvanceCharacter(bool bOkToFail)
{
    if (m_bSkipNextAdvance)
    {
        m_bSkipNextAdvance = false;
        return S_OK;
    }

    // If we hit EOF in the middle of a character,
    // it's ok-- we'll just have a corrupt last character 
    // (the buffer is padded with double NULLs )

    if (*m_pReadPtr == '\0')
    {
        // Read more from the file
        FillBuffer();

        // We are at EOF if it is still nullptr
        if (*m_pReadPtr == '\0')
        {
            if (!bOkToFail)
            {
                Error(E_INVALID_XML_SYNTAX, "Unexpected EOF while parsing XML file");
                return E_INVALID_XML_SYNTAX;
            }
            else
            {
                return E_FAIL;
            }
        }
    }

    if (!m_bUnicode)
    {
        m_Ch = *reinterpret_cast<CHAR*>(m_pReadPtr);
        m_pReadPtr++;
    }
    else
    {
        m_Ch = *reinterpret_cast<WCHAR*>(m_pReadPtr);

        if (m_bReverseBytes)
        {
            m_Ch = (m_Ch << 8) + (m_Ch >> 8);
        }

        m_pReadPtr += 2;
    }

    if (m_Ch == '\n')
    {
        m_pISAXCallback->m_LineNum++;
        m_pISAXCallback->m_LinePos = 0;
    }
    else if (m_Ch != '\r')
        m_pISAXCallback->m_LinePos++;

    return S_OK;
}


//-------------------------------------------------------------------------------------
// Name: XMLParser::AdvanceElement
// Desc: Builds <element> data, calls callback 
//-------------------------------------------------------------------------------------
HRESULT XMLParser::AdvanceElement()
{
    HRESULT hr = S_OK;

    // write ptr at the beginning of the buffer
    m_pWritePtr = m_pWriteBuf;

    if (FAILED(hr = AdvanceCharacter()))
        return hr;

    // if first character wasn't '<', we wouldn't be here

    if (FAILED(hr = AdvanceCharacter()))
        return hr;

    if (m_Ch == '!')
    {
        if (FAILED(hr = AdvanceCharacter()))
            return hr;
        if (m_Ch == '-')
        {
            if (FAILED(hr = AdvanceCharacter()))
                return hr;
            if (m_Ch != '-')
            {
                Error(E_INVALID_XML_SYNTAX, "Expecting '-' after '<!-'");
                return E_INVALID_XML_SYNTAX;
            }
            if (FAILED(hr = AdvanceComment()))
                return hr;
            return S_OK;
        }

        if (m_Ch != '[')
        {
            Error(E_INVALID_XML_SYNTAX, "Expecting '<![CDATA['");
            return E_INVALID_XML_SYNTAX;
        }
        if (FAILED(hr = AdvanceCharacter()))
            return hr;
        if (m_Ch != 'C')
        {
            Error(E_INVALID_XML_SYNTAX, "Expecting '<![CDATA['");
            return E_INVALID_XML_SYNTAX;
        }
        if (FAILED(hr = AdvanceCharacter()))
            return hr;
        if (m_Ch != 'D')
        {
            Error(E_INVALID_XML_SYNTAX, "Expecting '<![CDATA['");
            return E_INVALID_XML_SYNTAX;
        }
        if (FAILED(hr = AdvanceCharacter()))
            return hr;
        if (m_Ch != 'A')
        {
            Error(E_INVALID_XML_SYNTAX, "Expecting '<![CDATA['");
            return E_INVALID_XML_SYNTAX;
        }
        if (FAILED(hr = AdvanceCharacter()))
            return hr;
        if (m_Ch != 'T')
        {
            Error(E_INVALID_XML_SYNTAX, "Expecting '<![CDATA['");
            return E_INVALID_XML_SYNTAX;
        }
        if (FAILED(hr = AdvanceCharacter()))
            return hr;
        if (m_Ch != 'A')
        {
            Error(E_INVALID_XML_SYNTAX, "Expecting '<![CDATA['");
            return E_INVALID_XML_SYNTAX;
        }
        if (FAILED(hr = AdvanceCharacter()))
            return hr;
        if (m_Ch != '[')
        {
            Error(E_INVALID_XML_SYNTAX, "Expecting '<![CDATA['");
            return E_INVALID_XML_SYNTAX;
        }
        if (FAILED(hr = AdvanceCDATA()))
            return hr;
    }
    else if (m_Ch == '/')
    {
        WCHAR* pEntityRefVal = m_pWritePtr;

        if (FAILED(hr = AdvanceName()))
            return hr;

        if (FAILED(m_pISAXCallback->ElementEnd(pEntityRefVal,
            static_cast<UINT>(m_pWritePtr - pEntityRefVal))))
            return E_ABORT;

        if (FAILED(hr = ConsumeSpace()))
            return hr;

        if (FAILED(hr = AdvanceCharacter()))
            return hr;

        if (m_Ch != '>')
        {
            Error(E_INVALID_XML_SYNTAX, "Expecting '>' after name for closing entity reference");
            return E_INVALID_XML_SYNTAX;
        }
    }
    else if (m_Ch == '?')
    {
        // just skip any xml header tag since not really important after identifying character set
        for (;; )
        {
            if (FAILED(hr = AdvanceCharacter()))
                return hr;

            if (m_Ch == '>')
                return S_OK;
        }
    }
    else
    {
        XMLAttribute   Attributes[XML_MAX_ATTRIBUTES_PER_ELEMENT] = {};
        UINT           NumAttrs = 0;

        WCHAR* pEntityRefVal = m_pWritePtr;

        SkipNextAdvance();

        // Entity tag
        if (FAILED(hr = AdvanceName()))
            return hr;

        const UINT EntityRefLen = static_cast<UINT>(m_pWritePtr - pEntityRefVal);

        if (FAILED(hr = ConsumeSpace()))
            return hr;

        if (FAILED(hr = AdvanceCharacter()))
            return hr;

        // read attributes
        while ((m_Ch != '>') && (m_Ch != '/'))
        {
            SkipNextAdvance();

            if (NumAttrs >= XML_MAX_ATTRIBUTES_PER_ELEMENT)
            {
                Error(E_INVALID_XML_SYNTAX, "Elements may not have more than %d attributes", XML_MAX_ATTRIBUTES_PER_ELEMENT);
                return E_INVALID_XML_SYNTAX;
            }

            Attributes[NumAttrs].strName = m_pWritePtr;

            // Attribute name
            if (FAILED(hr = AdvanceName()))
                return hr;

            Attributes[NumAttrs].NameLen = static_cast<UINT>(m_pWritePtr - Attributes[NumAttrs].strName);

            if (FAILED(hr = ConsumeSpace()))
                return hr;

            if (FAILED(hr = AdvanceCharacter()))
                return hr;

            if (m_Ch != '=')
            {
                Error(E_INVALID_XML_SYNTAX, "Expecting '=' character after attribute name");
                return E_INVALID_XML_SYNTAX;
            }

            if (FAILED(hr = ConsumeSpace()))
                return hr;

            Attributes[NumAttrs].strValue = m_pWritePtr;

            if (FAILED(hr = AdvanceAttrVal()))
                return hr;

            Attributes[NumAttrs].ValueLen = static_cast<UINT>(m_pWritePtr -
                Attributes[NumAttrs].strValue);

            ++NumAttrs;

            if (FAILED(hr = ConsumeSpace()))
                return hr;

            if (FAILED(hr = AdvanceCharacter()))
                return hr;
        }

        if (m_Ch == '/')
        {
            if (FAILED(hr = AdvanceCharacter()))
                return hr;
            if (m_Ch != '>')
            {
                Error(E_INVALID_XML_SYNTAX, "Expecting '>' after '/' in element tag");
                return E_INVALID_XML_SYNTAX;
            }

            if (FAILED(m_pISAXCallback->ElementBegin(pEntityRefVal, EntityRefLen,
                Attributes, NumAttrs)))
                return E_ABORT;

            if (FAILED(m_pISAXCallback->ElementEnd(pEntityRefVal, EntityRefLen)))
                return E_ABORT;
        }
        else
        {
            if (FAILED(m_pISAXCallback->ElementBegin(pEntityRefVal, EntityRefLen,
                Attributes, NumAttrs)))
                return E_ABORT;
        }
    }

    return S_OK;
}


//-------------------------------------------------------------------------------------
// Name: XMLParser::AdvanceCDATA
// Desc: Read a CDATA section
//-------------------------------------------------------------------------------------
HRESULT XMLParser::AdvanceCDATA()
{
    HRESULT hr = S_OK;
    WORD wStage = 0;

    if (FAILED(m_pISAXCallback->CDATABegin()))
        return E_ABORT;

    for (;; )
    {
        if (FAILED(hr = AdvanceCharacter()))
            return hr;

        *m_pWritePtr = m_Ch;
        m_pWritePtr++;

        if ((m_Ch == ']') && (wStage == 0))
            wStage = 1;
        else if ((m_Ch == ']') && (wStage == 1))
            wStage = 2;
        else if ((m_Ch == '>') && (wStage == 2))
        {
            m_pWritePtr -= 3;
            break;
        }
        else
            wStage = 0;

        if (m_pWritePtr - m_pWriteBuf >= XML_WRITE_BUFFER_SIZE)
        {
            if (FAILED(m_pISAXCallback->CDATAData(m_pWriteBuf, static_cast<UINT>(m_pWritePtr - m_pWriteBuf), true)))
                return E_ABORT;
            m_pWritePtr = m_pWriteBuf;
        }
    }

    if (FAILED(m_pISAXCallback->CDATAData(m_pWriteBuf, static_cast<UINT>(m_pWritePtr - m_pWriteBuf), false)))
        return E_ABORT;

    m_pWritePtr = m_pWriteBuf;

    if (FAILED(m_pISAXCallback->CDATAEnd()))
        return E_ABORT;

    return S_OK;
}

//-------------------------------------------------------------------------------------
// Name: XMLParser::AdvanceComment
// Desk: Skips over a comment
//-------------------------------------------------------------------------------------
HRESULT XMLParser::AdvanceComment()
{
    HRESULT hr = S_OK;
    WORD wStage;

    wStage = 0;
    for (;; )
    {
        if (FAILED(hr = AdvanceCharacter()))
            return hr;

        if ((m_Ch == '-') && (wStage == 0))
            wStage = 1;
        else if ((m_Ch == '-') && (wStage == 1))
            wStage = 2;
        else if ((m_Ch == '>') && (wStage == 2))
            break;
        else
            wStage = 0;
    }

    return S_OK;
}


//-------------------------------------------------------------------------------------
// Name: XMLParser::RegisterSAXCallbackInterface
// Desc: Registers callback interface 
//-------------------------------------------------------------------------------------
void XMLParser::RegisterSAXCallbackInterface(ISAXCallback* pISAXCallback)
{
    m_pISAXCallback = pISAXCallback;
}


//-------------------------------------------------------------------------------------
// Name: XMLParser::GetSAXCallbackInterface
// Desc: Returns current callback interface 
//-------------------------------------------------------------------------------------
ISAXCallback* XMLParser::GetSAXCallbackInterface()
{
    return m_pISAXCallback;
}


//-------------------------------------------------------------------------------------
// Name: XMLParser::MainParseLoop
// Desc: Main Loop to Parse Data - source agnostic
//-------------------------------------------------------------------------------------
HRESULT XMLParser::MainParseLoop()
{
    bool bWhiteSpaceOnly = true;
    HRESULT hr = S_OK;

    if (FAILED(m_pISAXCallback->StartDocument()))
        return E_ABORT;

    m_pWritePtr = m_pWriteBuf;

    FillBuffer();

    if (*reinterpret_cast<WCHAR*>(m_pReadBuf) == 0xFEFF)
    {
        m_bUnicode = true;
        m_bReverseBytes = false;
        m_pReadPtr += 2;
    }
    else if (*reinterpret_cast<WCHAR*>(m_pReadBuf) == 0xFFFE)
    {
        m_bUnicode = true;
        m_bReverseBytes = true;
        m_pReadPtr += 2;
    }
    else if (*reinterpret_cast<WCHAR*>(m_pReadBuf) == 0x003C)
    {
        m_bUnicode = true;
        m_bReverseBytes = false;
    }
    else if (*reinterpret_cast<WCHAR*>(m_pReadBuf) == 0x3C00)
    {
        m_bUnicode = true;
        m_bReverseBytes = true;
    }
    else if (m_pReadBuf[0] == 0x3C)
    {
        m_bUnicode = false;
        m_bReverseBytes = false;
    }
    else
    {
        Error(E_INVALID_XML_SYNTAX, "Unrecognized encoding (parser does not support UTF-8 language encodings)");
        return E_INVALID_XML_SYNTAX;
    }

    for (;; )
    {
        if (FAILED(AdvanceCharacter(true)))
        {
            if ((static_cast<UINT>(m_pWritePtr - m_pWriteBuf) != 0) && (!bWhiteSpaceOnly))
            {
                if (FAILED(m_pISAXCallback->ElementContent(m_pWriteBuf, static_cast<UINT>(m_pWritePtr - m_pWriteBuf), false)))
                    return E_ABORT;

                bWhiteSpaceOnly = true;
            }

            if (FAILED(m_pISAXCallback->EndDocument()))
                return E_ABORT;

            return S_OK;
        }

        if (m_Ch == '<')
        {
            if ((static_cast<UINT>(m_pWritePtr - m_pWriteBuf) != 0) && (!bWhiteSpaceOnly))
            {
                if (FAILED(m_pISAXCallback->ElementContent(m_pWriteBuf, static_cast<UINT>(m_pWritePtr - m_pWriteBuf), false)))
                    return E_ABORT;

                bWhiteSpaceOnly = true;
            }

            SkipNextAdvance();

            m_pWritePtr = m_pWriteBuf;

            if (FAILED(hr = AdvanceElement()))
                return hr;

            m_pWritePtr = m_pWriteBuf;
        }
        else
        {
            if (m_Ch == '&')
            {
                SkipNextAdvance();
                if (FAILED(hr = ConvertEscape()))
                    return hr;
            }

            if (bWhiteSpaceOnly && (m_Ch != ' ') && (m_Ch != '\n') && (m_Ch != '\r') &&
                (m_Ch != '\t'))
            {
                bWhiteSpaceOnly = false;
            }

            *m_pWritePtr = m_Ch;
            m_pWritePtr++;

            if (m_pWritePtr - m_pWriteBuf >= XML_WRITE_BUFFER_SIZE)
            {
                if (!bWhiteSpaceOnly)
                {
                    if (FAILED(m_pISAXCallback->ElementContent(m_pWriteBuf,
                        (UINT)(m_pWritePtr - m_pWriteBuf),
                        true)))
                    {
                        return E_ABORT;
                    }
                }

                m_pWritePtr = m_pWriteBuf;
                bWhiteSpaceOnly = true;
            }
        }
    }
}


//-------------------------------------------------------------------------------------
// Name: XMLParser::ParseXMLFile
// Desc: Builds element data
//-------------------------------------------------------------------------------------
HRESULT XMLParser::ParseXMLFile(const CHAR* strFilename)
{
    HRESULT hr = S_OK;

    if (!m_pISAXCallback)
        return E_NOINTERFACE;

    m_pISAXCallback->m_LineNum = 1;
    m_pISAXCallback->m_LinePos = 0;
    m_pISAXCallback->m_strFilename = strFilename;  // save this off only while we parse the file

    m_bSkipNextAdvance = false;
    m_pReadPtr = m_pReadBuf;

    m_pReadBuf[0] = '\0';
    m_pReadBuf[1] = '\0';

    m_pInXMLBuffer = nullptr;
    m_uInXMLBufferCharsLeft = 0;
    m_hFile = CreateFile(strFilename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);

    if (m_hFile == INVALID_HANDLE_VALUE)
    {
        Error(E_COULD_NOT_OPEN_FILE, "Error opening file");
        hr = E_COULD_NOT_OPEN_FILE;

    }
    else
    {
        LARGE_INTEGER iFileSize;
        GetFileSizeEx(m_hFile, &iFileSize);
        m_dwCharsTotal = static_cast<DWORD>(iFileSize.QuadPart);
        m_dwCharsConsumed = 0;
        hr = MainParseLoop();
    }

    // Close the file
    if (m_hFile != INVALID_HANDLE_VALUE)
        CloseHandle(m_hFile);
    m_hFile = INVALID_HANDLE_VALUE;

    // we no longer own strFilename, so un-set it
    m_pISAXCallback->m_strFilename = nullptr;

    return hr;
}

//-------------------------------------------------------------------------------------
// Name: XMLParser::ParseXMLFile
// Desc: Builds element data
//-------------------------------------------------------------------------------------
HRESULT XMLParser::ParseXMLBuffer(const CHAR* strBuffer, UINT uBufferSize)
{
    if (!m_pISAXCallback)
        return E_NOINTERFACE;

    m_pISAXCallback->m_LineNum = 1;
    m_pISAXCallback->m_LinePos = 0;
    m_pISAXCallback->m_strFilename = "";  // save this off only while we parse the file

    m_bSkipNextAdvance = false;
    m_pReadPtr = m_pReadBuf;

    m_pReadBuf[0] = '\0';
    m_pReadBuf[1] = '\0';

    m_hFile = nullptr;
    m_pInXMLBuffer = strBuffer;
    m_uInXMLBufferCharsLeft = uBufferSize;
    m_dwCharsTotal = m_uInXMLBufferCharsLeft;
    m_dwCharsConsumed = 0;

    const HRESULT hr = MainParseLoop();

    // we no longer own strFilename, so un-set it
    m_pISAXCallback->m_strFilename = nullptr;

    return hr;
}

//-------------------------------------------------------------------------------------
// XMLParser::Error()      
//      Logs an error through the callback interface
//-------------------------------------------------------------------------------------

void XMLParser::Error(HRESULT hErr, const CHAR* strFormat, ...)
{
    constexpr INT MAX_OUTPUT_STR = 160;
    CHAR strBuffer[MAX_OUTPUT_STR];
    va_list pArglist;
    va_start(pArglist, strFormat);

    vsprintf_s(strBuffer, strFormat, pArglist);

    m_pISAXCallback->Error(hErr, strBuffer);
    va_end(pArglist);
}
