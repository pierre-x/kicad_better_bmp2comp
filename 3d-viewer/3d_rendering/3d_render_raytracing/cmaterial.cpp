/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2015-2020 Mario Luzeiro <mrluzeiro@ua.pt>
 * Copyright (C) 2015-2020 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

/**
 * @file  cmaterial.cpp
 * @brief
 */

#include "cmaterial.h"
#include <3d_math.h>
#include <wx/debug.h>

int CMATERIAL::m_default_nrsamples_refractions = 4;
int CMATERIAL::m_default_nrsamples_reflections = 3;
int CMATERIAL::m_default_refractions_recursive_levels = 2;
int CMATERIAL::m_default_reflections_recursive_levels = 3;

// This may be a good value if based on nr of lights
// that contribute to the illumination of that point
#define AMBIENT_FACTOR  (1.0f / 6.0f)
#define SPECULAR_FACTOR 1.0f


CMATERIAL::CMATERIAL()
{
    m_ambientColor  = SFVEC3F( 0.2f, 0.2f, 0.2f );
    m_emissiveColor = SFVEC3F( 0.0f, 0.0f, 0.0f );
    m_specularColor = SFVEC3F( 1.0f, 1.0f, 1.0f );
    m_shinness      = 50.2f;
    m_transparency  = 0.0f; // completely opaque
    m_cast_shadows  = true;
    m_reflection    = 0.0f;
    m_absorbance    = 1.0f;
    m_refraction_nr_samples = m_default_nrsamples_refractions;
    m_reflections_nr_samples = m_default_nrsamples_reflections;
    m_refractions_recursive_levels = m_default_refractions_recursive_levels;
    m_reflections_recursive_levels = m_default_reflections_recursive_levels;

    m_normal_perturbator = NULL;
}


CMATERIAL::CMATERIAL( const SFVEC3F &aAmbient,
                      const SFVEC3F &aEmissive,
                      const SFVEC3F &aSpecular,
                      float aShinness,
                      float aTransparency,
                      float aReflection )
{
    wxASSERT( aReflection >= 0.0f );
    wxASSERT( aReflection <= 1.0f );

    wxASSERT( aTransparency >= 0.0f );
    wxASSERT( aTransparency <= 1.0f );

    wxASSERT( aShinness >= 0.0f );
    wxASSERT( aShinness <= 180.0f );

    m_ambientColor  = aAmbient * SFVEC3F(AMBIENT_FACTOR);

    m_emissiveColor = aEmissive;
    m_specularColor = aSpecular;
    m_shinness      = aShinness;
    m_transparency  = glm::clamp( aTransparency, 0.0f, 1.0f );
    m_absorbance    = 1.0f;
    m_reflection    = aReflection;
    m_cast_shadows  = true;
    m_refraction_nr_samples = m_default_nrsamples_refractions;
    m_reflections_nr_samples = m_default_nrsamples_reflections;
    m_refractions_recursive_levels = m_default_refractions_recursive_levels;
    m_reflections_recursive_levels = m_default_reflections_recursive_levels;

    m_normal_perturbator = NULL;
}


void CMATERIAL::PerturbeNormal( SFVEC3F &aNormal,
                                const RAY &aRay,
                                const HITINFO &aHitInfo ) const
{
    if( m_normal_perturbator )
    {
        aNormal = aNormal + m_normal_perturbator->Generate( aRay, aHitInfo );
        aNormal = glm::normalize( aNormal );
    }
}


// https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_shading_model
SFVEC3F CBLINN_PHONG_MATERIAL::Shade( const RAY &aRay,
                                      const HITINFO &aHitInfo,
                                      float NdotL,
                                      const SFVEC3F &aDiffuseObjColor,
                                      const SFVEC3F &aDirToLight,
                                      const SFVEC3F &aLightColor,
                                      float aShadowAttenuationFactor ) const
{
    wxASSERT( NdotL >= FLT_EPSILON );

    // This is a hack to get some kind of fake ambient illumination
    // There is no logic behind this, just pure artistic experimentation
    //const float ambientFactor = glm::max( ( (1.0f - NdotL) /** (1.0f - NdotL)*/ ) *
    //                                      ( AMBIENT_FACTOR + AMBIENT_FACTOR ),
    //                                      AMBIENT_FACTOR );

    if( aShadowAttenuationFactor > FLT_EPSILON )
    {
        // Calculate the diffuse light factoring in light color,
        // power and the attenuation
        const SFVEC3F diffuse = NdotL * aLightColor;

        // Calculate the half vector between the light vector and the view vector.
        const SFVEC3F H = glm::normalize( aDirToLight - aRay.m_Dir );

        //Intensity of the specular light
        const float NdotH = glm::dot( H, aHitInfo.m_HitNormal );
        const float intensitySpecular = glm::pow( glm::max( NdotH, 0.0f ),
                                                  m_shinness );

        return  m_ambientColor +
                aShadowAttenuationFactor * ( diffuse * aDiffuseObjColor +
                                             SPECULAR_FACTOR *
                                             aLightColor *
                                             intensitySpecular *
                                             m_specularColor );
    }

    return m_ambientColor;
}


CPROCEDURALGENERATOR::CPROCEDURALGENERATOR()
{
}


static PerlinNoise s_perlinNoise = PerlinNoise( 0 );


CBOARDNORMAL::CBOARDNORMAL( float aScale ) : CPROCEDURALGENERATOR()
{
    m_scale = (2.0f * glm::pi<float>()) / aScale;
}


SFVEC3F CBOARDNORMAL::Generate( const RAY &aRay, const HITINFO &aHitInfo ) const
{
    const SFVEC3F hitPos = aHitInfo.m_HitPoint * m_scale;

    // http://www.fooplot.com/#W3sidHlwZSI6MCwiZXEiOiJzaW4oc2luKHNpbih4KSoxLjkpKjEuNSkiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC45NjIxMDU3MDgwNzg1MjYyIiwiNy45NzE0MjYyNjc2MDE0MyIsIi0yLjUxNzYyMDM1MTQ4MjQ0OSIsIjIuOTc5OTM3Nzg3Mzk3NTMwMyJdLCJzaXplIjpbNjQ2LDM5Nl19XQ--

    // Implement a texture as the "measling crazing blistering" method of FR4
    const float x = glm::sin( glm::sin( hitPos.x ) * 1.5f ) * 0.06f;
    const float y = glm::sin( glm::sin( hitPos.y ) * 1.5f ) * 0.03f;
    const float z = -(x + y) + glm::sin( hitPos.z ) * 0.06f;

    const float noise1 = s_perlinNoise.noise( hitPos.x * 1.0f, hitPos.y * 0.7f ) - 0.5f;
    const float noise2 = s_perlinNoise.noise( hitPos.x * 0.7f, hitPos.y * 1.0f ) - 0.5f;
    const float noise3 = s_perlinNoise.noise( hitPos.x * 0.3f, hitPos.z * 1.0f ) - 0.5f;

    return ( SFVEC3F( noise1, noise2, -( noise3 ) ) * 0.3f + SFVEC3F( x, y, z ) );
}


CCOPPERNORMAL::CCOPPERNORMAL( float aScale, const CPROCEDURALGENERATOR *aBoardNormalGenerator )
{
    m_board_normal_generator = aBoardNormalGenerator;
    m_scale = 1.0f / aScale;
}


SFVEC3F CCOPPERNORMAL::Generate( const RAY &aRay, const HITINFO &aHitInfo ) const
{
    if( m_board_normal_generator )
    {
        const SFVEC3F boardNormal = m_board_normal_generator->Generate( aRay, aHitInfo );

        SFVEC3F hitPos = aHitInfo.m_HitPoint * m_scale;

        const float noise =
                ( s_perlinNoise.noise( hitPos.x + boardNormal.y + aRay.m_Origin.x * 0.2f,
                          hitPos.y + boardNormal.x )
                        - 0.5f )
                * 2.0f;

        float scratchPattern =
                ( s_perlinNoise.noise( noise + hitPos.x / 100.0f, hitPos.y * 100.0f ) - 0.5f );

        const float x = scratchPattern * 0.14f;
        const float y = (noise + noise * scratchPattern) * 0.14f;

        return SFVEC3F( x, y, - ( x + y ) ) + boardNormal * 0.25f;
    }
    else
    {
        return SFVEC3F( 0.0f );
    }
}


CSOLDERMASKNORMAL::CSOLDERMASKNORMAL( const CPROCEDURALGENERATOR *aCopperNormalGenerator )
{
    m_copper_normal_generator = aCopperNormalGenerator;
}


SFVEC3F CSOLDERMASKNORMAL::Generate( const RAY &aRay, const HITINFO &aHitInfo ) const
{
    if( m_copper_normal_generator )
    {
        const SFVEC3F copperNormal = m_copper_normal_generator->Generate( aRay, aHitInfo );

        return copperNormal * 0.05f;
    }
    else
    {
        return SFVEC3F(0.0f);
    }
}


SFVEC3F CPLATEDCOPPERNORMAL::Generate( const RAY &aRay, const HITINFO &aHitInfo ) const
{
    SFVEC3F hitPos = aHitInfo.m_HitPoint * m_scale;

    const float noise1 = ( s_perlinNoise.noise( hitPos.x, hitPos.y ) - 0.5f );
    const float noise2 = ( s_perlinNoise.noise( hitPos.y, hitPos.x ) - 0.5f );

    return SFVEC3F( noise1, noise2, -( noise1 + noise2 ) ) * 0.02f;
}


CPLASTICNORMAL::CPLASTICNORMAL( float aScale )
{
    m_scale = 1.0f / aScale;
}


SFVEC3F CPLASTICNORMAL::Generate( const RAY &aRay, const HITINFO &aHitInfo ) const
{
        const SFVEC3F hitPos = aHitInfo.m_HitPoint * m_scale;

        const float noise1 = s_perlinNoise.noise( hitPos.x * 1.0f,
                                                  hitPos.y * 1.1f,
                                                  hitPos.z * 1.2f ) - 0.5f;

        const float noise2 = s_perlinNoise.noise( hitPos.x * 1.3f,
                                                  hitPos.y * 1.0f,
                                                  hitPos.z * 1.5f ) - 0.5f;

        const float noise3 = s_perlinNoise.noise( hitPos.x * 1.0f,
                                                  hitPos.y * 1.0f,
                                                  hitPos.z * 1.8f ) - 0.5f;

        const float distanceReduction = 1.0f / ( aHitInfo.m_tHit + 0.5f );

        return SFVEC3F( noise1, noise2, noise3 ) * SFVEC3F( distanceReduction );
}


CPLASTICSHINENORMAL::CPLASTICSHINENORMAL( float aScale )
{
    m_scale = 1.0f / aScale;
}


SFVEC3F CPLASTICSHINENORMAL::Generate( const RAY &aRay, const HITINFO &aHitInfo ) const
{
    const SFVEC3F hitPos = aHitInfo.m_HitPoint * m_scale;

    const float noise1 = s_perlinNoise.noise( hitPos.x * 0.01f,
                                              hitPos.y * 0.01f,
                                              hitPos.z * 0.01f ) - 0.5f;

    const float noise2 = s_perlinNoise.noise( hitPos.x * 1.0f,
                                              hitPos.y * 1.0f,
                                              hitPos.z * 1.6f ) - 0.5f;

    float noise3 = s_perlinNoise.noise( hitPos.x * 1.5f,
                                        hitPos.y * 1.5f,
                                        hitPos.z * 1.0f ) - 0.5f;
    noise3 = noise3 * noise3 * noise3;

    return SFVEC3F( noise1, noise2, noise3 ) * SFVEC3F( 0.1f, 0.2f, 1.0f );
}


CMETALBRUSHEDNORMAL::CMETALBRUSHEDNORMAL( float aScale )
{
    m_scale = 1.0f / aScale;
}


SFVEC3F CMETALBRUSHEDNORMAL::Generate( const RAY &aRay, const HITINFO &aHitInfo ) const
{
    const SFVEC3F hitPos = aHitInfo.m_HitPoint * m_scale;

    const float noise1 = s_perlinNoise.noise( hitPos.x * 1.0f,
                                              hitPos.y * 1.1f,
                                              hitPos.z * 1.2f ) - 0.5f;

    const float noise2 = s_perlinNoise.noise( hitPos.x * 1.3f,
                                              hitPos.y * 1.4f,
                                              hitPos.z * 1.5f ) - 0.5f;

    const float noise3 = s_perlinNoise.noise( hitPos.x * 0.1f,
                                              hitPos.y * 0.1f,
                                              hitPos.z * 1.0f ) - 0.5f;

    return SFVEC3F( noise1 * 0.15f + noise3 * 0.35f,
                    noise2 * 0.25f,
                    noise1 * noise2 * noise3 );
}


CSILKSCREENNORMAL::CSILKSCREENNORMAL( float aScale )
{
    m_scale = 1.0f / aScale;
}


SFVEC3F CSILKSCREENNORMAL::Generate( const RAY &aRay, const HITINFO &aHitInfo ) const
{
    const SFVEC3F hitPos = aHitInfo.m_HitPoint * m_scale;

    const float noise1 = s_perlinNoise.noise( hitPos.x * 2.7f,
                                              hitPos.y * 2.6f,
                                              hitPos.z );

    const float noise2 = s_perlinNoise.noise( hitPos.x * 1.1f,
                                              hitPos.y * 1.2f,
                                              hitPos.z );

    SFVEC3F t =
            glm::abs( ( 1.8f / ( SFVEC3F( noise1, noise2, hitPos.z ) + 0.4f ) ) - 1.5f ) - 0.25f;
    t = t * t * t * 0.1f;

    return t;
}
