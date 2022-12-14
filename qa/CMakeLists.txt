# This program source code file is part of KiCad, a free EDA CAD application.
#
# Copyright (C) 2014-2019 KiCad Developers, see AUTHORS.txt for contributors.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, you may find one here:
# http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
# or you may search the http://www.gnu.org website for the version 2 license,
# or you may write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

include(KiCadQABuildUtils)

if( KICAD_TEST_XML_OUTPUT )
    # To do this, you will need xmlrunner
    set( PY_TEST_ARGS --xml=${CMAKE_CURRENT_BINARY_DIR}/python.xunit-results )
endif()

# Test that runs the QA tests through scripting
add_test( NAME qa_python
    COMMAND ${PYTHON_EXECUTABLE} test.py ${PY_TEST_ARGS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)


set_property( TEST qa_python
    PROPERTY ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}/pcbnew${PYTHON_QA_PATH}"
)

# Shared QA helper libraries
add_subdirectory( qa_utils )
add_subdirectory( pcbnew_utils )

# Unit tests
add_subdirectory( unittests )

# Utility/debugging/profiling programs
add_subdirectory( tools )

if( KICAD_SIGNAL_INTEGRITY )
    add_subdirectory( ibis )
endif()
