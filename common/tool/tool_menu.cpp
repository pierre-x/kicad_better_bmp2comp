/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017-2019 KiCad Developers, see CHANGELOG.txt for contributors.
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

#include <draw_frame.h>
#include <tool/tool_menu.h>
#include <tool/tool_interactive.h>
#include <tool/action_menu.h>
#include <tool/actions.h>
#include <tool/zoom_menu.h>
#include <tool/grid_menu.h>
#include <tool/selection.h>


TOOL_MENU::TOOL_MENU( TOOL_INTERACTIVE& aTool ) :
    m_menu( true, &aTool ),
    m_tool( aTool )
{
}


TOOL_MENU::~TOOL_MENU()
{
}


CONDITIONAL_MENU& TOOL_MENU::GetMenu()
{
    return m_menu;
}


void TOOL_MENU::AddSubMenu( std::shared_ptr<ACTION_MENU> aSubMenu )
{
    // store a copy of the menu (keeps a reference)
    m_subMenus.push_back( std::move( aSubMenu ) );
}


void TOOL_MENU::ShowContextMenu( SELECTION& aSelection )
{
    m_menu.Evaluate( aSelection );
    m_menu.UpdateAll();
    m_menu.ClearDirty();
    m_tool.SetContextMenu( &m_menu, CMENU_NOW );
}


void TOOL_MENU::ShowContextMenu()
{
    m_menu.SetDirty();
    m_tool.SetContextMenu( &m_menu, CMENU_NOW );
}


void TOOL_MENU::CloseContextMenu( OPT_TOOL_EVENT& evt )
{
}


// This makes the factory functions a bit less verbose
using S_C = SELECTION_CONDITIONS;

void TOOL_MENU::AddStandardSubMenus( EDA_DRAW_FRAME* aFrame )
{
    m_menu.AddItem( ACTIONS::zoomCenter, S_C::ShowAlways, 1000 );
    m_menu.AddItem( ACTIONS::zoomIn, S_C::ShowAlways, 1000  );
    m_menu.AddItem( ACTIONS::zoomOut, S_C::ShowAlways, 1000 );
    m_menu.AddItem( ACTIONS::zoomFitScreen, S_C::ShowAlways, 1000 );

    m_menu.AddSeparator(SELECTION_CONDITIONS::ShowAlways, 1000 );

    if( aFrame )
    {
        m_menu.AddMenu( createOwnSubMenu<ZOOM_MENU>( aFrame ).get(), S_C::ShowAlways, 1000 );
        m_menu.AddMenu( createOwnSubMenu<GRID_MENU>( aFrame ).get(), S_C::ShowAlways, 1000 );
    }
}
