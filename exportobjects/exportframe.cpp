//-------------------------------------------------------------------------------------
// ExportFrame.cpp
//  
// Advanced Technology Group (ATG)
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------

#include "stdafx.h"
#include "ExportFrame.h"

using namespace ATG;

ExportFrame::ExportFrame()
    : ExportBase(nullptr)
{
    m_Transform.SetIdentity();
}

ExportFrame::ExportFrame(ExportString name)
    : ExportBase(name)
{
    m_Transform.SetIdentity();
}

ExportFrame::~ExportFrame()
{
    for (size_t i = 0; i < m_vModels.size(); i++)
    {
        delete m_vModels[i];
    }
    m_vModels.clear();
    for (size_t i = 0; i < m_vLights.size(); i++)
    {
        delete m_vLights[i];
    }
    m_vLights.clear();
    for (size_t i = 0; i < m_vCameras.size(); i++)
    {
        delete m_vCameras[i];
    }
    m_vCameras.clear();
    for (size_t i = 0; i < m_vChildren.size(); i++)
    {
        delete m_vChildren[i];
    }
    m_vChildren.clear();
}

ExportFrame* ExportFrame::FindFrameByDCCObject(void* pObject)
{
    if (!pObject)
        return nullptr;
    if (pObject == GetDCCObject())
        return this;

    for (size_t i = 0; i < m_vChildren.size(); i++)
    {
        ExportFrame* pFrame = m_vChildren[i]->FindFrameByDCCObject(pObject);
        if (pFrame)
            return pFrame;
    }
    return nullptr;
}

void ExportFrame::NormalizeTransform()
{
    m_Transform.Normalize();
}
