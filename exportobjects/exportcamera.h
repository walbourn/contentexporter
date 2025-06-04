//-------------------------------------------------------------------------------------
// ExportCamera.h
//
// A data structure representing a camera.
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
    class ExportCamera : public ExportBase
    {
    public:
        ExportCamera(ExportString strName);

        DirectX::XMFLOAT3   LocalPosition;
        DirectX::XMFLOAT3   Direction;
        DirectX::XMFLOAT3   Up;
        float               fFocalLength;
        float               fFieldOfView;
        float               fNearClip;
        float               fFarClip;
    };
}
