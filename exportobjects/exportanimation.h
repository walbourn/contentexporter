//-------------------------------------------------------------------------------------
// ExportAnimation.h
//
// Data structures for keyframed animation curves, and code to manipulate and optimize
// the keyframes.
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

    struct ExportAnimationPositionKey
    {
        float               fTime;
        DirectX::XMFLOAT3   Position;
    };
    typedef std::vector<ExportAnimationPositionKey> PositionKeyList;

    struct ExportAnimationOrientationKey
    {
        float               fTime;
        DirectX::XMFLOAT4   Orientation;
    };
    typedef std::vector<ExportAnimationOrientationKey> OrientationKeyList;

    struct ExportAnimationScaleKey
    {
        float               fTime;
        DirectX::XMFLOAT3   Scale;
    };
    typedef std::vector<ExportAnimationScaleKey> ScaleKeyList;

    class ExportFrame;

    class ExportAnimationTransformTrack
    {
    public:
        ExportAnimationTransformTrack()
            : pSourceFrame(nullptr)
        {
        }
        void AddKey(float fTime, const ExportTransform& Transform);
        void AddKey(float fTime, const DirectX::XMFLOAT4X4& matTransform);
        void AddKey(float fTime, const DirectX::XMFLOAT3& Position, const DirectX::XMFLOAT4& Orientation, const DirectX::XMFLOAT3& Scale);
        void OptimizeKeys();
        void SortKeys();
        void EndianSwap();
        float* GetPositionData() const noexcept { return (float*)(PositionKeys.data()); }
        float* GetOrientationData() const noexcept { return (float*)(OrientationKeys.data()); }
        float* GetScaleData() const noexcept { return (float*)(ScaleKeys.data()); }
        size_t GetPositionDataSize() const noexcept { return PositionKeys.size() * 4 * sizeof(float); }
        size_t GetOrientationDataSize() const noexcept { return OrientationKeys.size() * 5 * sizeof(float); }
        size_t GetScaleDataSize() const noexcept { return ScaleKeys.size() * 4 * sizeof(float); }
        ExportAnimationPositionKey* GetPositionKeys() noexcept { return PositionKeys.data(); }
        size_t GetPositionKeyCount() const noexcept { return PositionKeys.size(); }
        ExportAnimationOrientationKey* GetOrientationKeys() noexcept { return OrientationKeys.data(); }
        size_t GetOrientationKeyCount() const noexcept { return OrientationKeys.size(); }
        ExportAnimationScaleKey* GetScaleKeys() noexcept { return ScaleKeys.data(); }
        size_t GetScaleKeyCount() const noexcept { return ScaleKeys.size(); }
        bool IsTrackEmpty() const;
    protected:
        void OptimizePositionKeys();
        void OptimizeOrientationKeys();
        void OptimizeScaleKeys();

        bool PositionChangedFromLastTwoKeys(const ExportAnimationPositionKey& pk);
        bool OrientationChangedFromLastTwoKeys(const ExportAnimationOrientationKey& ok);
        bool ScaleChangedFromLastTwoKeys(const ExportAnimationScaleKey& sk);
    public:
        PositionKeyList         PositionKeys;
        OrientationKeyList      OrientationKeys;
        ScaleKeyList            ScaleKeys;
        ExportFrame* pSourceFrame;
    };

    class ExportAnimationTrack :
        public ExportBase
    {
    public:
        ExportAnimationTransformTrack       TransformTrack;
    };

    class ExportAnimation :
        public ExportBase
    {
    public:
        ExportAnimation();
        ~ExportAnimation();
        void AddTrack(ExportAnimationTrack* pTrack) { m_vTracks.push_back(pTrack); }
        size_t GetTrackCount() const noexcept { return m_vTracks.size(); }
        ExportAnimationTrack* GetTrack(size_t dwIndex) { return m_vTracks[dwIndex]; }
        float GetDuration() const noexcept { return fEndTime - fStartTime; }
        void Optimize();
        void EndianSwap();
        static void SetAnimationExportQuality(INT iPos, INT iOrientation, INT iScale);
    public:
        float                               fStartTime;
        float                               fEndTime;
        float                               fSourceFrameInterval;
        float                               fSourceSamplingInterval;
    protected:
        std::vector< ExportAnimationTrack* >     m_vTracks;
    };

};
