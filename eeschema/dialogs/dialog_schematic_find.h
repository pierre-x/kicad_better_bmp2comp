/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2010 Wayne Stambaugh <stambaughw@gmail.com>
 * Copyright (C) 2010-2021 KiCad Developers, see AUTHORS.txt for contributors.
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
 * @file
 *
 * Subclass of DIALOG_SCH_FIND_BASE, which is generated by wxFormBuilder.
 *
 * This dialog is used to define the search criteria used to search for items
 * in the current schematic.  What is searched depends on the schematic item
 * type.  Check the Matches() method for each item derived from SCH_ITEM() to
 * find out how matching is performed against that item.
 */

#ifndef __dialog_schematic_find__
#define __dialog_schematic_find__

#include "dialog_schematic_find_base.h"

#include <wx/fdrepdlg.h>          // Use the wxFindReplaceDialog events, data, and enums.

class SCH_EDIT_FRAME;
class SCH_EDITOR_CONTROL;


class DIALOG_SCH_FIND : public DIALOG_SCH_FIND_BASE
{
public:
    DIALOG_SCH_FIND( SCH_EDIT_FRAME* aParent, wxFindReplaceData* aData,
                     const wxPoint& aPosition = wxDefaultPosition,
                     const wxSize& aSize = wxDefaultSize, int aStyle = 0 );

    void SetFindEntries( const wxArrayString& aEntries, const wxString& aFindString );
    wxArrayString GetFindEntries() const;

    void SetReplaceEntries( const wxArrayString& aEntries );
    wxArrayString GetReplaceEntries() const { return m_comboReplace->GetStrings(); }

protected:
    // Handlers for DIALOG_SCH_FIND_BASE events.
    void OnClose( wxCloseEvent& aEvent ) override;
    void OnCancel( wxCommandEvent& aEvent ) override;
    void OnSearchForSelect( wxCommandEvent& aEvent ) override;
    void OnSearchForText( wxCommandEvent& aEvent ) override;
    void OnSearchForEnter( wxCommandEvent& event ) override;
    void OnReplaceWithSelect( wxCommandEvent& aEvent ) override;
    void OnReplaceWithText( wxCommandEvent& aEvent ) override;
    void OnReplaceWithEnter( wxCommandEvent& event ) override;
    void OnOptions( wxCommandEvent& event ) override;
    void OnUpdateReplaceUI( wxUpdateUIEvent& aEvent ) override;
    void OnUpdateReplaceAllUI( wxUpdateUIEvent& aEvent ) override;
    void OnIdle( wxIdleEvent& event ) override;
    void OnChar( wxKeyEvent& aEvent );
    void OnFind( wxCommandEvent& aEvent ) override;
    void OnReplace( wxCommandEvent& aEvent ) override;

    // Rebuild the search flags from dialog settings
    void updateFlags();

    SCH_EDIT_FRAME*     m_frame;
    SCH_EDITOR_CONTROL* m_editorControl;
    wxFindReplaceData*  m_findReplaceData;
    bool                m_findDirty;

    DECLARE_NO_COPY_CLASS( DIALOG_SCH_FIND )
};


#endif // __dialog_schematic_find__
