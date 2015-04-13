//-------------------------------------------------------------------------------------
//  ExportAnimation.cpp
//  
//  Microsoft XNA Developer Connection
//  Copyright © Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------

#include "stdafx.h"
#include "exportanimation.h"

namespace ATG
{
    FLOAT g_fPositionExportTolerance = 0.99f;
    FLOAT g_fOrientationExportTolerance = 0.99f;
    FLOAT g_fScaleExportTolerance = 0.99f;

    FLOAT ConvertToleranceValue( INT iValue )
    {
        FLOAT fExponent = (FLOAT)iValue / 15.0f;
        FLOAT fValue = 1.0f - powf( 10.0f, -fExponent );
        return max( min( fValue, 1.0f ), 0.9f );
    }

    VOID ExportAnimation::SetAnimationExportQuality( INT iPos, INT iOrientation, INT iScale )
    {
        g_fPositionExportTolerance = ConvertToleranceValue( iPos );
        g_fOrientationExportTolerance = ConvertToleranceValue( iOrientation );
        g_fScaleExportTolerance = ConvertToleranceValue( iScale );
    }

    VOID ExportAnimationTransformTrack::AddKey( FLOAT fTime, const D3DXVECTOR3& Position, const D3DXQUATERNION& Orientation, const D3DXVECTOR3& Scale )
    {
        ExportAnimationPositionKey PositionKey;
        ExportAnimationOrientationKey OrientationKey;
        ExportAnimationScaleKey ScaleKey;
        PositionKey.fTime = fTime;
        OrientationKey.fTime = fTime;
        ScaleKey.fTime = fTime;
        PositionKey.Position = Position;
        OrientationKey.Orientation = Orientation;
        ScaleKey.Scale = Scale;
        if( PositionChangedFromLastKey( PositionKey ) )
            PositionKeys.push_back( PositionKey );
        if( OrientationChangedFromLastKey( OrientationKey ) )
            OrientationKeys.push_back( OrientationKey );
        if( ScaleChangedFromLastKey( ScaleKey ) )
            ScaleKeys.push_back( ScaleKey );
    }

    VOID ExportAnimationTransformTrack::AddKey( FLOAT fTime, const ExportTransform& Transform )
    {
        AddKey( fTime, Transform.Position(), Transform.Orientation(), Transform.Scale() );
    }

    VOID ExportAnimationTransformTrack::AddKey( FLOAT fTime, const D3DXMATRIX& matTransform )
    {
        ExportTransform Transform;
        Transform.InitializeFromFloats( (FLOAT*)&matTransform );
        AddKey( fTime, Transform );
    }

    BOOL ExportAnimationTransformTrack::PositionChangedFromLastKey( const ExportAnimationPositionKey& pk )
    {
        if( PositionKeys.size() == 0 )
            return TRUE;
        const ExportAnimationPositionKey& pkc = PositionKeys.back();
        return pkc.Position != pk.Position;
    }

    BOOL ExportAnimationTransformTrack::OrientationChangedFromLastKey( const ExportAnimationOrientationKey& pk )
    {
        if( OrientationKeys.size() == 0 )
            return TRUE;
        const ExportAnimationOrientationKey& pkc = OrientationKeys.back();
        return pkc.Orientation != pk.Orientation;
    }

    BOOL ExportAnimationTransformTrack::ScaleChangedFromLastKey( const ExportAnimationScaleKey& pk )
    {
        if( ScaleKeys.size() == 0 )
            return TRUE;
        const ExportAnimationScaleKey& pkc = ScaleKeys.back();
        return pkc.Scale != pk.Scale;
    }

    VOID ExportAnimationTransformTrack::OptimizeKeys()
    {
        SortKeys();

        OptimizePositionKeys();
        OptimizeOrientationKeys();
        OptimizeScaleKeys();
    }

    VOID ExportAnimationTransformTrack::OptimizePositionKeys()
    {
        if( PositionKeys.size() < 2 )
            return;
        D3DXVECTOR3 CurrentDelta = PositionKeys[1].Position - PositionKeys[0].Position;
        D3DXVec3Normalize( &CurrentDelta, &CurrentDelta );
        PositionKeys.push_back( PositionKeys[0] );

        PositionKeyList NewKeyList;
        NewKeyList.push_back( PositionKeys[0] );
        for( DWORD i = 2; i < PositionKeys.size(); i++ )
        {
            D3DXVECTOR3 Delta = PositionKeys[i].Position - PositionKeys[i - 1].Position;
            FLOAT fDotSubtraction = D3DXVec3Dot( &Delta, &Delta );
            D3DXVec3Normalize( &Delta, &Delta );
            FLOAT fDot = D3DXVec3Dot( &CurrentDelta, &Delta );
            if( CurrentDelta == Delta )
                fDot = 1.0f;
            if( fDotSubtraction < 1e-5f )
                fDot = 1.0f;
            if( fDot < g_fPositionExportTolerance )
            {
                CurrentDelta = Delta;
                NewKeyList.push_back( PositionKeys[i - 1] );
            }
        }
        PositionKeys = NewKeyList;
    }

    D3DXVECTOR4 CheapNormalize4( const D3DXVECTOR4& Vec4 )
    {
        if( Vec4.x == 0 && Vec4.y == 0 && Vec4.z == 0 && Vec4.w == 0 )
            return Vec4;
        D3DXVECTOR4 Temp;
        D3DXVec4Normalize( &Temp, &Vec4 );
        return Temp;
    }

    VOID ExportAnimationTransformTrack::OptimizeOrientationKeys()
    {
        if( OrientationKeys.size() < 2 )
            return;
        D3DXVECTOR4 OrientationB = (D3DXVECTOR4)OrientationKeys[1].Orientation;
        D3DXVECTOR4 OrientationA = (D3DXVECTOR4)OrientationKeys[0].Orientation;
        D3DXVECTOR4 CurrentDelta = OrientationB - OrientationA;
        CurrentDelta = CheapNormalize4( CurrentDelta );
        OrientationKeys.push_back( OrientationKeys[0] );

        OrientationKeyList NewKeyList;
        NewKeyList.push_back( OrientationKeys[0] );
        for( DWORD i = 2; i < OrientationKeys.size(); i++ )
        {
            OrientationB = (D3DXVECTOR4)OrientationKeys[i].Orientation;
            OrientationA = (D3DXVECTOR4)OrientationKeys[i - 1].Orientation;
            D3DXVECTOR4 Delta = OrientationB - OrientationA;
            FLOAT fDotSubtraction = D3DXVec4Dot( &Delta, &Delta );
            Delta = CheapNormalize4( Delta );
            FLOAT fDot = D3DXVec4Dot( &CurrentDelta, &Delta );
            if( CurrentDelta == Delta )
                fDot = 1.0f;
            if( fDotSubtraction < 1e-9f )
                fDot = 1.0f;
            if( fDot < g_fOrientationExportTolerance )
            {
                CurrentDelta = Delta;
                NewKeyList.push_back( OrientationKeys[i - 1] );
            }
        }
        OrientationKeys = NewKeyList;
    }

    VOID ExportAnimationTransformTrack::OptimizeScaleKeys()
    {
        if( ScaleKeys.size() < 2 )
            return;
        D3DXVECTOR3 CurrentDelta = ScaleKeys[1].Scale - ScaleKeys[0].Scale;
        D3DXVec3Normalize( &CurrentDelta, &CurrentDelta );
        ScaleKeys.push_back( ScaleKeys[0] );

        ScaleKeyList NewKeyList;
        NewKeyList.push_back( ScaleKeys[0] );
        for( DWORD i = 2; i < ScaleKeys.size(); i++ )
        {
            D3DXVECTOR3 Delta = ScaleKeys[i].Scale - ScaleKeys[i - 1].Scale;
            FLOAT fDotSubtraction = D3DXVec3Dot( &Delta, &Delta );
            D3DXVec3Normalize( &Delta, &Delta );
            FLOAT fDot = D3DXVec3Dot( &CurrentDelta, &Delta );
            if( CurrentDelta == Delta )
                fDot = 1.0f;
            if( fDotSubtraction < 1e-5f )
                fDot = 1.0f;
            if( fDot < g_fScaleExportTolerance )
            {
                CurrentDelta = Delta;
                NewKeyList.push_back( ScaleKeys[i - 1] );
            }
        }
        ScaleKeys = NewKeyList;
    }

    BOOL PositionLessEqual( ExportAnimationPositionKey& A, ExportAnimationPositionKey& B )
    {
        return A.fTime <= B.fTime;
    }

    BOOL OrientationLessEqual( ExportAnimationOrientationKey& A, ExportAnimationOrientationKey& B )
    {
        return A.fTime <= B.fTime;
    }

    BOOL ScaleLessEqual( ExportAnimationScaleKey& A, ExportAnimationScaleKey& B )
    {
        return A.fTime <= B.fTime;
    }

    VOID ExportAnimationTransformTrack::SortKeys()
    {
        std::sort( PositionKeys.begin(), PositionKeys.end(), PositionLessEqual );
        std::sort( OrientationKeys.begin(), OrientationKeys.end(), OrientationLessEqual );
        std::sort( ScaleKeys.begin(), ScaleKeys.end(), ScaleLessEqual );
    }

    VOID ExportAnimationTransformTrack::EndianSwap()
    {
        DWORD dwPositionFloatCount = GetPositionDataSize() / sizeof( FLOAT );
        DWORD dwOrientationFloatCount = GetOrientationDataSize() / sizeof( FLOAT );
        DWORD dwScaleFloatCount = GetScaleDataSize() / sizeof( FLOAT );

        DWORD* pData = (DWORD*)GetPositionData();
        for( DWORD i = 0; i < dwPositionFloatCount; ++i )
        {
            *pData = _byteswap_ulong( *pData );
            ++pData;
        }
        pData = (DWORD*)GetOrientationData();
        for( DWORD i = 0; i < dwOrientationFloatCount; ++i )
        {
            *pData = _byteswap_ulong( *pData );
            ++pData;
        }
        pData = (DWORD*)GetScaleData();
        for( DWORD i = 0; i < dwScaleFloatCount; ++i )
        {
            *pData = _byteswap_ulong( *pData );
            ++pData;
        }
    }

    BOOL ExportAnimationTransformTrack::IsTrackEmpty()
    {
        // If any key array has more than 1 key, the track is not empty
        if( PositionKeys.size() > 1 ||
            OrientationKeys.size() > 1 ||
            ScaleKeys.size() > 1 )
        {
            return FALSE;
        }

        BOOL bResult = TRUE;

        if( pSourceFrame != NULL )
        {
            // Check for an orientation that is different from the scene position
            if( OrientationKeys.size() == 1 )
            {
                if( OrientationKeys[0].Orientation != pSourceFrame->Transform().Orientation() )
                {
                    bResult = FALSE;
                }
            }
            // Check for a position that is different from the scene position
            if( PositionKeys.size() == 1 )
            {
                if( PositionKeys[0].Position != pSourceFrame->Transform().Position() )
                {
                    bResult = FALSE;
                }
            }
            // Check for a scale that is different from the scene scale
            if( ScaleKeys.size() == 1 )
            {
                if( ScaleKeys[0].Scale != pSourceFrame->Transform().Scale() )
                {
                    bResult = FALSE;
                }
            }
        }

        return bResult;
    }

ExportAnimation::ExportAnimation(void)
{
}

ExportAnimation::~ExportAnimation(void)
{
}

VOID ExportAnimation::Optimize()
{
    ExportLog::LogMsg( 4, "Optimizing animation with %d tracks.", m_vTracks.size() );
    vector< ExportAnimationTrack* > NewTrackList;
    for( DWORD i = 0; i < m_vTracks.size(); i++ )
    {
        ExportAnimationTrack* pTrack = m_vTracks[i];
        pTrack->TransformTrack.OptimizeKeys();
        if( pTrack->TransformTrack.IsTrackEmpty() )
        {
            delete pTrack;
        }
        else
        {
            NewTrackList.push_back( pTrack );
        }
    }
    ExportLog::LogMsg( 4, "Animation has %d tracks after optimization.", NewTrackList.size() );
    m_vTracks = NewTrackList;
}

VOID ExportAnimation::EndianSwap()
{
    for( DWORD i = 0; i < m_vTracks.size(); i++ )
    {
        ExportAnimationTrack* pTrack = m_vTracks[i];
        pTrack->TransformTrack.EndianSwap();
    }
}

};

