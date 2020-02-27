//-------------------------------------------------------------------------------------
// ExportLight.h
//
// Classes to represent various types of lights found in a scene.
//  
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------
#pragma once

#include "ExportBase.h"

namespace ATG
{
    class ExportLight : public ExportBase
    {
    public:
        ExportLight(ExportString strName);
        enum LightType
        {
            LT_AMBIENT = 0,
            LT_DIRECTIONAL,
            LT_POINT,
            LT_SPOT,
        };
        enum LightFalloff
        {
            LF_NONE = 0,
            LF_LINEAR,
            LF_SQUARED
        };

        LightType           Type;
        DirectX::XMFLOAT3   LocalPosition;
        DirectX::XMFLOAT4   Color;
        DirectX::XMFLOAT3   Direction;
        float               fRange;
        LightFalloff        Falloff;
        LightFalloff        SpotFalloff;
        float               fInnerAngle;
        float               fOuterAngle;
    };
}
