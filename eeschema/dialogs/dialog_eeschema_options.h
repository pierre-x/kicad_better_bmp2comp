/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2009 Wayne Stambaugh <stambaughw@verizon.net>
 * Copyright (C) 1992-2011 KiCad Developers, see AUTHORS.txt for contributors.
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
 * @file dialog_eeschema_options.h
 *
 * Subclass of DIALOG_EESCHEMA_OPTIONS_BASE, which is generated by wxFormBuilder.
 */

#ifndef __dialog_eeschema_options__
#define __dialog_eeschema_options__

#include <dialog_eeschema_options_base.h>

class DIALOG_EESCHEMA_OPTIONS : public DIALOG_EESCHEMA_OPTIONS_BASE
{
public:
    DIALOG_EESCHEMA_OPTIONS( wxWindow* parent );

    void SetUnits( const wxArrayString& units, int select = 0 );
    int GetUnitsSelection( void ) { return m_choiceUnits->GetSelection(); }

    void SetGridSelection( int select ) { m_choiceGridSize->SetSelection( select ); }
    int GetGridSelection( void ) { return m_choiceGridSize->GetSelection(); }
    void SetGridSizes( const GRIDS& grid_sizes, int grid_id );

    void SetBusWidth( int aWidth )
    {
        m_spinBusWidth->SetValue( aWidth );
    }

    int GetBusWidth( void )
    {
        return m_spinBusWidth->GetValue();
    }

    void SetLineWidth( int aWidth ) { m_spinLineWidth->SetValue( aWidth ); }
    int GetLineWidth( void ) { return m_spinLineWidth->GetValue(); }

    void SetTextSize( int text_size ) { m_spinTextSize->SetValue( text_size ); }
    int GetTextSize( void ) { return m_spinTextSize->GetValue(); }

    void SetRepeatHorizontal( int displacement )
    {
        m_spinRepeatHorizontal->SetValue( displacement );
    }
    int GetRepeatHorizontal( void ) { return m_spinRepeatHorizontal->GetValue(); }
    void SetRepeatVertical( int displacement ) { m_spinRepeatVertical->SetValue( displacement ); }

    int GetRepeatVertical( void ) { return m_spinRepeatVertical->GetValue(); }
    void SetRepeatLabel( int increment ) { m_spinRepeatLabel->SetValue( increment ); }
    int GetRepeatLabel( void ) { return m_spinRepeatLabel->GetValue(); }

    void SetAutoSaveInterval( int aInterval ) { m_spinAutoSaveInterval->SetValue( aInterval ); }
    int GetAutoSaveInterval() const { return m_spinAutoSaveInterval->GetValue(); }

    void SetRefIdSeparator( wxChar aSep, wxChar aFirstId);
    void GetRefIdSeparator( int& aSep, int& aFirstId);

    void SetShowGrid( bool show ) { m_checkShowGrid->SetValue( show ); }
    bool GetShowGrid( void ) { return m_checkShowGrid->GetValue(); }

    void SetShowHiddenPins( bool show ) { m_checkShowHiddenPins->SetValue( show ); }
    bool GetShowHiddenPins( void ) { return m_checkShowHiddenPins->GetValue(); }

    void SetEnableZoomNoCenter( bool enable )
    {
        m_checkEnableZoomNoCenter->SetValue( enable );
    }

    bool GetEnableZoomNoCenter( void )
    {
        return m_checkEnableZoomNoCenter->GetValue();
    }
    void SetEnableMiddleButtonPan( bool enable )
    {
        m_checkEnableMiddleButtonPan->SetValue( enable );
        m_checkMiddleButtonPanLimited->Enable( enable );
    }

    bool GetEnableMiddleButtonPan( void )
    {
        return m_checkEnableMiddleButtonPan->GetValue();
    }
    void SetMiddleButtonPanLimited( bool enable )
    {
        m_checkMiddleButtonPanLimited->SetValue( enable );
    }
    bool GetMiddleButtonPanLimited( void )
    {
        return m_checkMiddleButtonPanLimited->GetValue();
    }

    void SetEnableAutoPan( bool enable ) { m_checkAutoPan->SetValue( enable ); }
    bool GetEnableAutoPan( void ) { return m_checkAutoPan->GetValue(); }

    void SetEnableHVBusOrientation( bool enable ) { m_checkHVOrientation->SetValue( enable ); }
    bool GetEnableHVBusOrientation( void ) { return m_checkHVOrientation->GetValue(); }

    void SetShowPageLimits( bool show ) { m_checkPageLimits->SetValue( show ); }
    bool GetShowPageLimits( void ) { return m_checkPageLimits->GetValue(); }

    /** Set the field \a aNdx textctrl to \a aName */
    void SetFieldName( int aNdx, wxString aName );

    /** Get the field \a aNdx name from the fields textctrl */
    wxString GetFieldName( int aNdx );

private:
    void OnMiddleBtnPanEnbl( wxCommandEvent& event )
    {
        m_checkMiddleButtonPanLimited->Enable( GetEnableMiddleButtonPan() );
    }
};

#endif // __dialog_eeschema_options__
