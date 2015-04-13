//-------------------------------------------------------------------------------------
// collision.h
//  
// A subset of the ATG Collision header
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



//-----------------------------------------------------------------------------
// Bounding volumes structures.
//
// The bounding volume structures are setup for near minimum size because there
// are likely to be many of them, and memory bandwidth and space will be at a
// premium relative to CPU cycles on Xbox 360.
//-----------------------------------------------------------------------------

struct Sphere
{
    XMFLOAT3 Center;            // Center of the sphere.
    FLOAT Radius;               // Radius of the sphere.
};

struct AxisAlignedBox
{
    XMFLOAT3 Center;            // Center of the box.
    XMFLOAT3 Extents;           // Distance from the center to each side.
};

struct OrientedBox
{
    XMFLOAT3 Center;            // Center of the box.
    XMFLOAT3 Extents;           // Distance from the center to each side.
    XMFLOAT4 Orientation;       // Unit quaternion representing rotation (box -> world).
};


//-----------------------------------------------------------------------------
// Bounding volume construction.
//-----------------------------------------------------------------------------
VOID ComputeBoundingSphereFromPoints( Sphere* pOut, UINT Count, const XMFLOAT3* pPoints, UINT Stride );
VOID ComputeBoundingAxisAlignedBoxFromPoints( AxisAlignedBox* pOut, UINT Count, const XMFLOAT3* pPoints, UINT Stride );
VOID ComputeBoundingOrientedBoxFromPoints( OrientedBox* pOut, UINT Count, const XMFLOAT3* pPoints, UINT Stride );

}; // namespace
