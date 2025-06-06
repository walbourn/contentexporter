//-------------------------------------------------------------------------------------
// ExportAnimation.cpp
//
// Advanced Technology Group (ATG)
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------

#include "stdafx.h"
#include "exportanimation.h"

using namespace DirectX;

namespace ATG
{
    float g_fPositionExportTolerance = 0.99f;
    float g_fOrientationExportTolerance = 0.99f;
    float g_fScaleExportTolerance = 0.99f;
}

using namespace ATG;

namespace
{
    const XMVECTORF32 g_NewSlopeTolerance = { 1e-4f, 1e-4f, 1e-4f, 1e-4f };

    float ConvertToleranceValue(INT iValue)
    {
        const float fExponent = float(iValue) / 15.0f;
        const float fValue = 1.0f - powf(10.0f, -fExponent);
        return std::max(std::min(fValue, 1.0f), 0.9f);
    }
}

void ExportAnimation::SetAnimationExportQuality(INT iPos, INT iOrientation, INT iScale)
{
    g_fPositionExportTolerance = ConvertToleranceValue(iPos);
    g_fOrientationExportTolerance = ConvertToleranceValue(iOrientation);
    g_fScaleExportTolerance = ConvertToleranceValue(iScale);
}

void ExportAnimationTransformTrack::AddKey(float fTime, const XMFLOAT3& Position, const XMFLOAT4& Orientation, const XMFLOAT3& Scale)
{
    ExportAnimationPositionKey PositionKey = {};
    ExportAnimationOrientationKey OrientationKey = {};
    ExportAnimationScaleKey ScaleKey = {};
    PositionKey.fTime = fTime;
    OrientationKey.fTime = fTime;
    ScaleKey.fTime = fTime;
    PositionKey.Position = Position;
    OrientationKey.Orientation = Orientation;
    ScaleKey.Scale = Scale;

    const bool bForceNoConstantOptimization = !g_ExportCoreSettings.bOptimizeAnimations;

    if (bForceNoConstantOptimization || PositionChangedFromLastTwoKeys(PositionKey))
    {
        PositionKeys.push_back(PositionKey);
    }
    else
    {
        ExportAnimationPositionKey& LastKey = PositionKeys.back();
        LastKey.fTime = fTime;
    }

    if (bForceNoConstantOptimization || OrientationChangedFromLastTwoKeys(OrientationKey))
    {
        OrientationKeys.push_back(OrientationKey);
    }
    else
    {
        ExportAnimationOrientationKey& LastKey = OrientationKeys.back();
        LastKey.fTime = fTime;
    }

    if (bForceNoConstantOptimization || ScaleChangedFromLastTwoKeys(ScaleKey))
    {
        ScaleKeys.push_back(ScaleKey);
    }
    else
    {
        ExportAnimationScaleKey& LastKey = ScaleKeys.back();
        LastKey.fTime = fTime;
    }
}

void ExportAnimationTransformTrack::AddKey(float fTime, const ExportTransform& Transform)
{
    AddKey(fTime, Transform.Position(), Transform.Orientation(), Transform.Scale());
}

void ExportAnimationTransformTrack::AddKey(float fTime, const XMFLOAT4X4& matTransform)
{
    ExportTransform Transform;
    Transform.Initialize(matTransform);
    AddKey(fTime, Transform);
}

bool ExportAnimationTransformTrack::PositionChangedFromLastTwoKeys(const ExportAnimationPositionKey& pk)
{
    if (PositionKeys.size() < 2)
        return true;

    const XMVECTOR pk0 = XMLoadFloat3(&pk.Position);

    const size_t nkeys = PositionKeys.size();
    const XMVECTOR pk1 = XMLoadFloat3(&PositionKeys[nkeys - 1].Position);
    const XMVECTOR pk2 = XMLoadFloat3(&PositionKeys[nkeys - 2].Position);

    return (XMVector3NotEqual(pk1, pk0) || XMVector3NotEqual(pk2, pk0));
}

bool ExportAnimationTransformTrack::OrientationChangedFromLastTwoKeys(const ExportAnimationOrientationKey& pk)
{
    if (OrientationKeys.size() < 2)
        return true;

    const XMVECTOR pk0 = XMLoadFloat4(&pk.Orientation);

    const size_t nkeys = OrientationKeys.size();
    const XMVECTOR pk1 = XMLoadFloat4(&OrientationKeys[nkeys - 1].Orientation);
    const XMVECTOR pk2 = XMLoadFloat4(&OrientationKeys[nkeys - 2].Orientation);

    return (XMVector4NotEqual(pk1, pk0) || XMVector4NotEqual(pk2, pk0));
}

bool ExportAnimationTransformTrack::ScaleChangedFromLastTwoKeys(const ExportAnimationScaleKey& pk)
{
    if (ScaleKeys.size() < 2)
        return true;

    const XMVECTOR pk0 = XMLoadFloat3(&pk.Scale);

    const size_t nkeys = ScaleKeys.size();

    const XMVECTOR pk1 = XMLoadFloat3(&ScaleKeys[nkeys - 1].Scale);
    const XMVECTOR pk2 = XMLoadFloat3(&ScaleKeys[nkeys - 2].Scale);

    return (XMVector3NotEqual(pk1, pk0) || XMVector3NotEqual(pk2, pk0));
}

void ExportAnimationTransformTrack::OptimizeKeys()
{
    SortKeys();

    OptimizePositionKeys();
    OptimizeOrientationKeys();
    OptimizeScaleKeys();
}

XMVECTOR ComputeSlope(const ExportAnimationPositionKey& KeyA, const ExportAnimationPositionKey& KeyB)
{
    const XMVECTOR pk1 = XMLoadFloat3(&KeyA.Position);
    const XMVECTOR pk2 = XMLoadFloat3(&KeyB.Position);
    const XMVECTOR vDelta = pk2 - pk1;
    const float fTimeDelta = KeyB.fTime - KeyA.fTime;
    assert(fTimeDelta > 0.0f);
    return vDelta / fTimeDelta;
}

XMVECTOR ComputeSlope(const ExportAnimationOrientationKey& KeyA, const ExportAnimationOrientationKey& KeyB)
{
    const XMVECTOR pk1 = XMLoadFloat4(&KeyA.Orientation);
    const XMVECTOR pk2 = XMLoadFloat4(&KeyB.Orientation);
    const XMVECTOR vDelta = pk2 - pk1;
    const float fTimeDelta = KeyB.fTime - KeyA.fTime;
    assert(fTimeDelta > 0.0f);
    return vDelta / fTimeDelta;
}

XMVECTOR ComputeSlope(const ExportAnimationScaleKey& KeyA, const ExportAnimationScaleKey& KeyB)
{
    const XMVECTOR pk1 = XMLoadFloat3(&KeyA.Scale);
    const XMVECTOR pk2 = XMLoadFloat3(&KeyB.Scale);
    const XMVECTOR vDelta = pk2 - pk1;
    const float fTimeDelta = KeyB.fTime - KeyA.fTime;
    assert(fTimeDelta > 0.0f);
    return vDelta / fTimeDelta;
}

bool NewSlopeEncountered3(FXMVECTOR vSlopeRef, FXMVECTOR vSlopeNew, const float fThreshold)
{
    XMVECTOR len = XMVector3LengthSq(vSlopeRef);

    XMVECTOR vRefNormalized = {};
    if (XMVector3Less(len, g_NewSlopeTolerance))
    {
        vRefNormalized = g_XMIdentityR0; // [1 0 0]
    }
    else
    {
        vRefNormalized = XMVector3Normalize(vSlopeRef);
    }

    len = XMVector3LengthSq(vSlopeNew);

    XMVECTOR vNewNormalized = {};
    if (XMVector3Less(len, g_NewSlopeTolerance))
    {
        vNewNormalized = g_XMIdentityR0; // [1 0 0]
    }
    else
    {
        vNewNormalized = XMVector3Normalize(vSlopeNew);
    }

    const float fDot = XMVectorGetX(XMVector3Dot(vRefNormalized, vNewNormalized));
    return(fDot <= fThreshold);
}

bool NewSlopeEncountered4(FXMVECTOR vSlopeRef, FXMVECTOR vSlopeNew, const float fThreshold)
{
    XMVECTOR len = XMVector4LengthSq(vSlopeRef);

    XMVECTOR vRefNormalized = {};
    if (XMVector4Less(len, g_NewSlopeTolerance))
    {
        vRefNormalized = g_XMIdentityR0; // [1 0 0 0]
    }
    else
    {
        vRefNormalized = XMVector4Normalize(vSlopeRef);
    }

    len = XMVector4LengthSq(vSlopeNew);

    XMVECTOR vNewNormalized = {};
    if (XMVector4Less(len, g_NewSlopeTolerance))
    {
        vNewNormalized = g_XMIdentityR0; // [1 0 0 0]
    }
    else
    {
        vNewNormalized = XMVector4Normalize(vSlopeNew);
    }

    const float fDot = XMVectorGetX(XMVector4Dot(vRefNormalized, vNewNormalized));
    return(fDot <= fThreshold);
}

void ExportAnimationTransformTrack::OptimizePositionKeys()
{
    if (PositionKeys.size() < 2)
        return;

    XMVECTOR vCurrentSlope = XMVectorZero();
    PositionKeyList NewKeyList;
    NewKeyList.push_back(PositionKeys[0]);

    for (size_t i = 1; i < PositionKeys.size(); ++i)
    {
        const XMVECTOR vSlope = ComputeSlope(PositionKeys[i - 1], PositionKeys[i]);
        if (NewSlopeEncountered3(vCurrentSlope, vSlope, g_fPositionExportTolerance))
        {
            if (i > 1)
            {
                NewKeyList.push_back(PositionKeys[i - 1]);
            }
            vCurrentSlope = vSlope;
        }
    }

    const XMVECTOR vFinalSlope = ComputeSlope(NewKeyList.back(), PositionKeys.back());
    const XMVECTOR len = XMVector3LengthSq(vFinalSlope);

    if (XMVector3Greater(len, g_NewSlopeTolerance))
    {
        NewKeyList.push_back(PositionKeys.back());
    }

    PositionKeys = NewKeyList;
}

void ExportAnimationTransformTrack::OptimizeOrientationKeys()
{
    if (OrientationKeys.size() < 2)
        return;

    XMVECTOR vCurrentSlope = XMVectorZero();
    OrientationKeyList NewKeyList;
    NewKeyList.push_back(OrientationKeys[0]);

    for (size_t i = 1; i < OrientationKeys.size(); ++i)
    {
        const XMVECTOR vSlope = ComputeSlope(OrientationKeys[i - 1], OrientationKeys[i]);
        if (NewSlopeEncountered4(vCurrentSlope, vSlope, g_fOrientationExportTolerance))
        {
            if (i > 1)
            {
                NewKeyList.push_back(OrientationKeys[i - 1]);
            }
            vCurrentSlope = vSlope;
        }
    }

    const XMVECTOR vFinalSlope = ComputeSlope(NewKeyList.back(), OrientationKeys.back());
    const XMVECTOR len = XMVector4LengthSq(vFinalSlope);

    if (XMVector4Greater(len, g_NewSlopeTolerance))
    {
        NewKeyList.push_back(OrientationKeys.back());
    }

    OrientationKeys = NewKeyList;
}

void ExportAnimationTransformTrack::OptimizeScaleKeys()
{
    if (ScaleKeys.size() < 2)
        return;

    XMVECTOR vCurrentSlope = XMVectorZero();
    ScaleKeyList NewKeyList;
    NewKeyList.push_back(ScaleKeys[0]);

    for (size_t i = 1; i < ScaleKeys.size(); ++i)
    {
        const XMVECTOR vSlope = ComputeSlope(ScaleKeys[i - 1], ScaleKeys[i]);
        if (NewSlopeEncountered3(vCurrentSlope, vSlope, g_fScaleExportTolerance))
        {
            if (i > 1)
            {
                NewKeyList.push_back(ScaleKeys[i - 1]);
            }
            vCurrentSlope = vSlope;
        }
    }

    const XMVECTOR vFinalSlope = ComputeSlope(NewKeyList.back(), ScaleKeys.back());
    const XMVECTOR len = XMVector3LengthSq(vFinalSlope);

    if (XMVector3Greater(len, g_NewSlopeTolerance))
    {
        NewKeyList.push_back(ScaleKeys.back());
    }

    ScaleKeys = NewKeyList;
}

bool PositionLessEqual(const ExportAnimationPositionKey& A, const ExportAnimationPositionKey& B)
{
    return A.fTime <= B.fTime;
}

bool OrientationLessEqual(const ExportAnimationOrientationKey& A, const ExportAnimationOrientationKey& B)
{
    return A.fTime <= B.fTime;
}

bool ScaleLessEqual(const ExportAnimationScaleKey& A, const ExportAnimationScaleKey& B)
{
    return A.fTime <= B.fTime;
}

void ExportAnimationTransformTrack::SortKeys()
{
    std::sort(PositionKeys.begin(), PositionKeys.end(), PositionLessEqual);
    std::sort(OrientationKeys.begin(), OrientationKeys.end(), OrientationLessEqual);
    std::sort(ScaleKeys.begin(), ScaleKeys.end(), ScaleLessEqual);
}

void ExportAnimationTransformTrack::EndianSwap()
{
    const size_t dwPositionFloatCount = GetPositionDataSize() / sizeof(float);
    const size_t dwOrientationFloatCount = GetOrientationDataSize() / sizeof(float);
    const size_t dwScaleFloatCount = GetScaleDataSize() / sizeof(float);

    auto pData = reinterpret_cast<DWORD*>(GetPositionData());
    for (size_t i = 0; i < dwPositionFloatCount; ++i)
    {
        *pData = _byteswap_ulong(*pData);
        ++pData;
    }
    pData = reinterpret_cast<DWORD*>(GetOrientationData());
    for (size_t i = 0; i < dwOrientationFloatCount; ++i)
    {
        *pData = _byteswap_ulong(*pData);
        ++pData;
    }
    pData = reinterpret_cast<DWORD*>(GetScaleData());
    for (size_t i = 0; i < dwScaleFloatCount; ++i)
    {
        *pData = _byteswap_ulong(*pData);
        ++pData;
    }
}

bool ExportAnimationTransformTrack::IsTrackEmpty() const
{
    // If any key array has more than 1 key, the track is not empty
    if (PositionKeys.size() > 1 ||
        OrientationKeys.size() > 1 ||
        ScaleKeys.size() > 1)
    {
        return false;
    }

    bool bResult = true;

    if (pSourceFrame)
    {
        // Check for an orientation that is different from the scene position
        if (OrientationKeys.size() == 1)
        {
            const XMVECTOR scene = XMLoadFloat4(&pSourceFrame->Transform().Orientation());
            const XMVECTOR key = XMLoadFloat4(&OrientationKeys[0].Orientation);

            if (XMVector4NotEqual(key, scene))
            {
                bResult = false;
            }
        }
        // Check for a position that is different from the scene position
        if (PositionKeys.size() == 1)
        {
            const XMVECTOR scene = XMLoadFloat3(&pSourceFrame->Transform().Position());
            const XMVECTOR key = XMLoadFloat3(&PositionKeys[0].Position);

            if (XMVector3NotEqual(key, scene))
            {
                bResult = false;
            }
        }
        // Check for a scale that is different from the scene scale
        if (ScaleKeys.size() == 1)
        {
            const XMVECTOR scene = XMLoadFloat3(&pSourceFrame->Transform().Scale());
            const XMVECTOR key = XMLoadFloat3(&ScaleKeys[0].Scale);

            if (XMVector3NotEqual(key, scene))
            {
                bResult = false;
            }
        }
    }
    else
    {
        bResult = false;
    }

    return bResult;
}

ExportAnimation::ExportAnimation() :
    fStartTime(0),
    fEndTime(0),
    fSourceFrameInterval(0),
    fSourceSamplingInterval(0)
{
}

ExportAnimation::~ExportAnimation()
{
}

void ExportAnimation::Optimize()
{
    if (!g_ExportCoreSettings.bOptimizeAnimations)
    {
        return;
    }

    ExportLog::LogMsg(4, "Optimizing animation with %zu tracks.", m_vTracks.size());
    std::vector< ExportAnimationTrack* > NewTrackList;
    for (size_t i = 0; i < m_vTracks.size(); i++)
    {
        ExportAnimationTrack* pTrack = m_vTracks[i];
        if (!pTrack)
            continue;

        pTrack->TransformTrack.OptimizeKeys();
        if (pTrack->TransformTrack.IsTrackEmpty())
        {
            delete pTrack;
        }
        else
        {
            NewTrackList.push_back(pTrack);
        }
    }
    ExportLog::LogMsg(4, "Animation has %zu tracks after optimization.", NewTrackList.size());
    m_vTracks = NewTrackList;
}

void ExportAnimation::EndianSwap()
{
    for (size_t i = 0; i < m_vTracks.size(); i++)
    {
        ExportAnimationTrack* pTrack = m_vTracks[i];
        pTrack->TransformTrack.EndianSwap();
    }
}

