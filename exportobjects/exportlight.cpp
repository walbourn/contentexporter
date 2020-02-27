//-------------------------------------------------------------------------------------
// ExportLight.cpp
//  
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------

#include "stdafx.h"
#include "ExportLight.h"

using namespace ATG;

ExportLight::ExportLight(ExportString strName)
    : ExportBase(strName),
    Type(LT_AMBIENT),
    LocalPosition(0, 0, 0),
    Color(1, 1, 1, 1),
    Direction(0, 0, -1),
    fRange(0),
    Falloff(LF_NONE),
    SpotFalloff(LF_NONE),
    fInnerAngle(0),
    fOuterAngle(0)
{
}
