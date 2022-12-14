/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2015 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
 * Copyright (C) 1992-2021 KiCad Developers, see AUTHORS.txt for contributors.
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
 * @brief Implementation of EDA_RECT base class for KiCad.
 */

#include <algorithm>
#include <deque>

#include <eda_rect.h>
#include <trigo.h>

void EDA_RECT::Normalize()
{
    if( m_size.y < 0 )
    {
        m_size.y = -m_size.y;
        m_pos.y -= m_size.y;
    }

    if( m_size.x < 0 )
    {
        m_size.x = -m_size.x;
        m_pos.x -= m_size.x;
    }
}


void EDA_RECT::Move( const VECTOR2I& aMoveVector )
{
    m_pos += aMoveVector;
}


bool EDA_RECT::Contains( const VECTOR2I& aPoint ) const
{
    VECTOR2I rel_pos = aPoint - m_pos;
    VECTOR2I size = m_size;

    if( size.x < 0 )
    {
        size.x = -size.x;
        rel_pos.x += size.x;
    }

    if( size.y < 0 )
    {
        size.y = -size.y;
        rel_pos.y += size.y;
    }

    return ( rel_pos.x >= 0 ) && ( rel_pos.y >= 0 ) && ( rel_pos.y <= size.y )
           && ( rel_pos.x <= size.x );
}


bool EDA_RECT::Contains( const EDA_RECT& aRect ) const
{
    return Contains( aRect.GetOrigin() ) && Contains( aRect.GetEnd() );
}


bool EDA_RECT::Intersects( const VECTOR2I& aPoint1, const VECTOR2I& aPoint2 ) const
{
    VECTOR2I point2, point4;

    if( Contains( aPoint1 ) || Contains( aPoint2 ) )
        return true;

    point2.x = GetEnd().x;
    point2.y = GetOrigin().y;
    point4.x = GetOrigin().x;
    point4.y = GetEnd().y;

    //Only need to test 3 sides since a straight line can't enter and exit on same side
    if( SegmentIntersectsSegment( aPoint1, aPoint2, GetOrigin(), point2 ) )
        return true;

    if( SegmentIntersectsSegment( aPoint1, aPoint2, point2, GetEnd() ) )
        return true;

    if( SegmentIntersectsSegment( aPoint1, aPoint2, GetEnd(), point4 ) )
        return true;

    return false;
}


bool EDA_RECT::Intersects( const VECTOR2I& aPoint1, const VECTOR2I& aPoint2,
                           VECTOR2I* aIntersection1, VECTOR2I* aIntersection2 ) const
{
    VECTOR2I point2, point4;

    point2.x = GetEnd().x;
    point2.y = GetOrigin().y;
    point4.x = GetOrigin().x;
    point4.y = GetEnd().y;

    bool intersects = false;

    VECTOR2I* aPointToFill = aIntersection1;

    if( SegmentIntersectsSegment( aPoint1, aPoint2, GetOrigin(), point2, aPointToFill ) )
        intersects = true;

    if( intersects )
        aPointToFill = aIntersection2;

    if( SegmentIntersectsSegment( aPoint1, aPoint2, point2, GetEnd(), aPointToFill ) )
        intersects = true;

    if( intersects )
        aPointToFill = aIntersection2;

    if( SegmentIntersectsSegment( aPoint1, aPoint2, GetEnd(), point4, aPointToFill ) )
        intersects = true;

    if( intersects )
        aPointToFill = aIntersection2;

    if( SegmentIntersectsSegment( aPoint1, aPoint2, point4, GetOrigin(), aPointToFill ) )
        intersects = true;

    return intersects;
}


bool EDA_RECT::Intersects( const EDA_RECT& aRect ) const
{
    if( !m_init )
        return false;

    // this logic taken from wxWidgets' geometry.cpp file:
    bool     rc;
    EDA_RECT me( *this );
    EDA_RECT rect( aRect );
    me.Normalize();   // ensure size is >= 0
    rect.Normalize(); // ensure size is >= 0

    // calculate the left common area coordinate:
    int left = std::max( me.m_pos.x, rect.m_pos.x );

    // calculate the right common area coordinate:
    int right = std::min( me.m_pos.x + me.m_size.x, rect.m_pos.x + rect.m_size.x );

    // calculate the upper common area coordinate:
    int top = std::max( me.m_pos.y, rect.m_pos.y );

    // calculate the lower common area coordinate:
    int bottom = std::min( me.m_pos.y + me.m_size.y, rect.m_pos.y + rect.m_size.y );

    // if a common area exists, it must have a positive (null accepted) size
    if( left <= right && top <= bottom )
        rc = true;
    else
        rc = false;

    return rc;
}


bool EDA_RECT::Intersects( const EDA_RECT& aRect, const EDA_ANGLE& aRotation ) const
{
    if( !m_init )
        return false;

    EDA_ANGLE rotation = aRotation;
    rotation.Normalize();

    /*
     * Most rectangles will be axis aligned.  It is quicker to check for this case and pass
     * the rect to the simpler intersection test.
     */

    // Prevent floating point comparison errors
    static const EDA_ANGLE ROT_EPSILON( 0.000000001, DEGREES_T );

    static const EDA_ANGLE ROT_PARALLEL[]      = { ANGLE_0, ANGLE_180, ANGLE_360 };
    static const EDA_ANGLE ROT_PERPENDICULAR[] = { ANGLE_0, ANGLE_90,  ANGLE_270 };

    // Test for non-rotated rectangle
    for( EDA_ANGLE ii : ROT_PARALLEL )
    {
        if( std::abs( rotation - ii ) < ROT_EPSILON )
            return Intersects( aRect );
    }

    // Test for rectangle rotated by multiple of 90 degrees
    for( EDA_ANGLE jj : ROT_PERPENDICULAR )
    {
        if( std::abs( rotation - jj ) < ROT_EPSILON )
        {
            EDA_RECT rotRect;

            // Rotate the supplied rect by 90 degrees
            rotRect.SetOrigin( aRect.Centre() );
            rotRect.Inflate( aRect.GetHeight(), aRect.GetWidth() );
            return Intersects( rotRect );
        }
    }

    /* There is some non-orthogonal rotation.
     * There are three cases to test:
     * A) One point of this rect is inside the rotated rect
     * B) One point of the rotated rect is inside this rect
     * C) One of the sides of the rotated rect intersect this
     */

    VECTOR2I corners[4];

    /* Test A : Any corners exist in rotated rect? */
    corners[0] = m_pos;
    corners[1] = m_pos + VECTOR2I( m_size.x, 0 );
    corners[2] = m_pos + VECTOR2I( m_size.x, m_size.y );
    corners[3] = m_pos + VECTOR2I( 0, m_size.y );

    VECTOR2I rCentre = aRect.Centre();

    for( int i = 0; i < 4; i++ )
    {
        VECTOR2I delta = corners[i] - rCentre;
        RotatePoint( delta, -rotation );
        delta += rCentre;

        if( aRect.Contains( delta ) )
            return true;
    }

    /* Test B : Any corners of rotated rect exist in this one? */
    int w = aRect.GetWidth() / 2;
    int h = aRect.GetHeight() / 2;

    // Construct corners around center of shape
    corners[0] = VECTOR2I( -w, -h );
    corners[1] = VECTOR2I( w, -h );
    corners[2] = VECTOR2I( w, h );
    corners[3] = VECTOR2I( -w, h );

    // Rotate and test each corner
    for( int j = 0; j < 4; j++ )
    {
        RotatePoint( corners[j], rotation );
        corners[j] += rCentre;

        if( Contains( corners[j] ) )
            return true;
    }

    /* Test C : Any sides of rotated rect intersect this */
    if( Intersects( corners[0], corners[1] ) || Intersects( corners[1], corners[2] )
            || Intersects( corners[2], corners[3] ) || Intersects( corners[3], corners[0] ) )
    {
        return true;
    }

    return false;
}


const VECTOR2I EDA_RECT::ClosestPointTo( const VECTOR2I& aPoint ) const
{
    EDA_RECT me( *this );

    me.Normalize(); // ensure size is >= 0

    // Determine closest point to the circle centre within this rect
    int nx = std::max( me.GetLeft(), std::min( aPoint.x, me.GetRight() ) );
    int ny = std::max( me.GetTop(), std::min( aPoint.y, me.GetBottom() ) );

    return VECTOR2I( nx, ny );
}


const VECTOR2I EDA_RECT::FarthestPointTo( const VECTOR2I& aPoint ) const
{
    EDA_RECT me( *this );

    me.Normalize(); // ensure size is >= 0

    int fx = std::max( std::abs( aPoint.x - me.GetLeft() ), std::abs( aPoint.x - me.GetRight() ) );
    int fy = std::max( std::abs( aPoint.y - me.GetTop() ), std::abs( aPoint.y - me.GetBottom() ) );

    return VECTOR2I( fx, fy );
}


bool EDA_RECT::IntersectsCircle( const VECTOR2I& aCenter, const int aRadius ) const
{
    if( !m_init )
        return false;

    VECTOR2I closest = ClosestPointTo( aCenter );

    double dx = static_cast<double>( aCenter.x ) - closest.x;
    double dy = static_cast<double>( aCenter.y ) - closest.y;

    double r = static_cast<double>( aRadius );

    return ( dx * dx + dy * dy ) <= ( r * r );
}


bool EDA_RECT::IntersectsCircleEdge( const VECTOR2I& aCenter, const int aRadius,
                                     const int aWidth ) const
{
    if( !m_init )
        return false;

    EDA_RECT me( *this );
    me.Normalize(); // ensure size is >= 0

    // Test if the circle intersects at all
    if( !IntersectsCircle( aCenter, aRadius + aWidth / 2 ) )
    {
        return false;
    }

    VECTOR2I farpt = FarthestPointTo( aCenter );
    // Farthest point must be further than the inside of the line
    double fx = (double) farpt.x;
    double fy = (double) farpt.y;

    double r = (double) aRadius - (double) aWidth / 2;

    return ( fx * fx + fy * fy ) > ( r * r );
}


EDA_RECT& EDA_RECT::Inflate( int aDelta )
{
    Inflate( aDelta, aDelta );
    return *this;
}


EDA_RECT& EDA_RECT::Inflate( wxCoord dx, wxCoord dy )
{
    if( m_size.x >= 0 )
    {
        if( m_size.x < -2 * dx )
        {
            // Don't allow deflate to eat more width than we have,
            m_pos.x += m_size.x / 2;
            m_size.x = 0;
        }
        else
        {
            // The inflate is valid.
            m_pos.x -= dx;
            m_size.x += 2 * dx;
        }
    }
    else // size.x < 0:
    {
        if( m_size.x > 2 * dx )
        {
            // Don't allow deflate to eat more width than we have,
            m_pos.x -= m_size.x / 2;
            m_size.x = 0;
        }
        else
        {
            // The inflate is valid.
            m_pos.x += dx;
            m_size.x -= 2 * dx; // m_Size.x <0: inflate when dx > 0
        }
    }

    if( m_size.y >= 0 )
    {
        if( m_size.y < -2 * dy )
        {
            // Don't allow deflate to eat more height than we have,
            m_pos.y += m_size.y / 2;
            m_size.y = 0;
        }
        else
        {
            // The inflate is valid.
            m_pos.y -= dy;
            m_size.y += 2 * dy;
        }
    }
    else // size.y < 0:
    {
        if( m_size.y > 2 * dy )
        {
            // Don't allow deflate to eat more height than we have,
            m_pos.y -= m_size.y / 2;
            m_size.y = 0;
        }
        else
        {
            // The inflate is valid.
            m_pos.y += dy;
            m_size.y -= 2 * dy; // m_Size.y <0: inflate when dy > 0
        }
    }

    return *this;
}


void EDA_RECT::Merge( const EDA_RECT& aRect )
{
    if( !m_init )
    {
        if( aRect.IsValid() )
        {
            m_pos  = aRect.GetPosition();
            m_size = aRect.GetSize();
            m_init = true;
        }
        return;
    }

    Normalize(); // ensure width and height >= 0
    EDA_RECT rect = aRect;
    rect.Normalize(); // ensure width and height >= 0
    VECTOR2I end = GetEnd();
    VECTOR2I rect_end = rect.GetEnd();

    // Change origin and size in order to contain the given rect
    m_pos.x = std::min( m_pos.x, rect.m_pos.x );
    m_pos.y = std::min( m_pos.y, rect.m_pos.y );
    end.x   = std::max( end.x, rect_end.x );
    end.y   = std::max( end.y, rect_end.y );
    SetEnd( end );
}


void EDA_RECT::Merge( const VECTOR2I& aPoint )
{
    if( !m_init )
    {
        m_pos  = aPoint;
        m_size = VECTOR2I( 0, 0 );
        m_init = true;
        return;
    }

    Normalize(); // ensure width and height >= 0

    VECTOR2I end = GetEnd();

    // Change origin and size in order to contain the given rect
    m_pos.x = std::min( m_pos.x, aPoint.x );
    m_pos.y = std::min( m_pos.y, aPoint.y );
    end.x   = std::max( end.x, aPoint.x );
    end.y   = std::max( end.y, aPoint.y );
    SetEnd( end );
}


double EDA_RECT::GetArea() const
{
    return (double) GetWidth() * (double) GetHeight();
}


EDA_RECT EDA_RECT::Common( const EDA_RECT& aRect ) const
{
    EDA_RECT r;

    if( Intersects( aRect ) )
    {
        VECTOR2I originA( std::min( GetOrigin().x, GetEnd().x ),
                          std::min( GetOrigin().y, GetEnd().y ) );
        VECTOR2I originB( std::min( aRect.GetOrigin().x, aRect.GetEnd().x ),
                          std::min( aRect.GetOrigin().y, aRect.GetEnd().y ) );
        VECTOR2I endA( std::max( GetOrigin().x, GetEnd().x ),
                       std::max( GetOrigin().y, GetEnd().y ) );
        VECTOR2I endB( std::max( aRect.GetOrigin().x, aRect.GetEnd().x ),
                       std::max( aRect.GetOrigin().y, aRect.GetEnd().y ) );

        r.SetOrigin(
                VECTOR2I( std::max( originA.x, originB.x ), std::max( originA.y, originB.y ) ) );
        r.SetEnd( VECTOR2I( std::min( endA.x, endB.x ), std::min( endA.y, endB.y ) ) );
    }

    return r;
}


const EDA_RECT EDA_RECT::GetBoundingBoxRotated( const VECTOR2I& aRotCenter,
                                                const EDA_ANGLE& aAngle ) const
{
    VECTOR2I corners[4];

    // Build the corners list
    corners[0]   = GetOrigin();
    corners[2]   = GetEnd();
    corners[1].x = corners[0].x;
    corners[1].y = corners[2].y;
    corners[3].x = corners[2].x;
    corners[3].y = corners[0].y;

    // Rotate all corners, to find the bounding box
    for( int ii = 0; ii < 4; ii++ )
        RotatePoint( corners[ii], aRotCenter, aAngle );

    // Find the corners bounding box
    VECTOR2I start = corners[0];
    VECTOR2I end = corners[0];

    for( int ii = 1; ii < 4; ii++ )
    {
        start.x = std::min( start.x, corners[ii].x );
        start.y = std::min( start.y, corners[ii].y );
        end.x   = std::max( end.x, corners[ii].x );
        end.y   = std::max( end.y, corners[ii].y );
    }

    EDA_RECT bbox;
    bbox.SetOrigin( start );
    bbox.SetEnd( end );

    return bbox;
}
