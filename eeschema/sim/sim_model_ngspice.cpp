/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2022 Mikolaj Wielgus
 * Copyright (C) 2022 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * https://www.gnu.org/licenses/gpl-3.0.html
 * or you may search the http://www.gnu.org website for the version 3 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <sim/sim_model_ngspice.h>
#include <locale_io.h>

using TYPE = SIM_MODEL::TYPE;


SIM_MODEL_NGSPICE::SIM_MODEL_NGSPICE( TYPE aType )
    : SIM_MODEL( aType )
{
    const MODEL_INFO& modelInfo = ModelInfo( getModelType() );

    for( const SIM_MODEL::PARAM::INFO& paramInfo : modelInfo.instanceParams )
    {
        // For now, only the geometry parameters.
        if( paramInfo.category == SIM_MODEL::PARAM::CATEGORY::PRINCIPAL
            || paramInfo.category == SIM_MODEL::PARAM::CATEGORY::GEOMETRY )
        {
            AddParam( paramInfo, getIsOtherVariant() );
        }
    }

    for( const SIM_MODEL::PARAM::INFO& paramInfo : modelInfo.modelParams )
        AddParam( paramInfo, getIsOtherVariant() );
}


std::vector<wxString> SIM_MODEL_NGSPICE::GenerateSpiceCurrentNames( const wxString& aRefName ) const
{
    LOCALE_IO toggle;

    switch( TypeInfo( GetType() ).deviceType )
    {
        case DEVICE_TYPE_::NPN:
        case DEVICE_TYPE_::PNP:
            return { wxString::Format( "I(%s:c)", aRefName ),
                     wxString::Format( "I(%s:b)", aRefName ),
                     wxString::Format( "I(%s:e)", aRefName ) };

        case DEVICE_TYPE_::NJFET:
        case DEVICE_TYPE_::PJFET:
        case DEVICE_TYPE_::NMES:
        case DEVICE_TYPE_::PMES:
        case DEVICE_TYPE_::NMOS:
        case DEVICE_TYPE_::PMOS:
            return { wxString::Format( "I(%s:d)", aRefName ),
                     wxString::Format( "I(%s:g)", aRefName ),
                     wxString::Format( "I(%s:s)", aRefName ) };

        case DEVICE_TYPE_::R:
        case DEVICE_TYPE_::C:
        case DEVICE_TYPE_::L:
        case DEVICE_TYPE_::D:
            return SIM_MODEL::GenerateSpiceCurrentNames( aRefName );

        default:
            wxFAIL_MSG( "Unhandled model device type" );
            return {};
    }
}


bool SIM_MODEL_NGSPICE::SetParamFromSpiceCode( const wxString& aParamName, const wxString& aParamValue,
                                               SIM_VALUE_GRAMMAR::NOTATION aNotation )
{
    // "level" and "version" are not really parameters - they're part of the type - so silently
    // ignore them.
    if( aParamName == "level" || aParamName == "version" )
        return true;


    // First we try to use the name as is. Note that you can't set instance parameters from this
    // function, it's generally for ".model" cards, not for instantiations.

    std::vector<std::reference_wrapper<const PARAM>> params = GetParams();

    auto paramIt = std::find_if( params.begin(), params.end(),
                                 [aParamName]( const PARAM& param )
                                 {
                                      return !param.info.isSpiceInstanceParam
                                          && param.info.category != PARAM::CATEGORY::SUPERFLUOUS
                                          && ( param.info.name == aParamName.Lower()
                                               || param.info.name == aParamName.Lower() + "_" );
                                 } );

    if( paramIt != params.end() )
        return SetParamValue( paramIt - params.begin(), aParamValue, aNotation );


    // One Spice param can have multiple names, we need to take this into account.

    std::vector<PARAM::INFO> ngspiceParams = ModelInfo( getModelType() ).modelParams;

    auto ngspiceParamIt = std::find_if( ngspiceParams.begin(), ngspiceParams.end(),
                                        [aParamName]( const PARAM& param )
                                        {
                                            return param.info.name == aParamName.Lower();
                                        } );

    if( ngspiceParamIt == ngspiceParams.end() )
        return false;


    // We obtain the id of the Ngspice param that is to be set.
    unsigned id = ngspiceParamIt->id;

    // Find an actual parameter with the same id.
    paramIt = std::find_if( params.begin(), params.end(),
                            [id]( const PARAM& param )
                            {
                                return param.info.id == id;
                            } );
    
    if( paramIt == params.end() )
        return false;

    return SetParamValue( paramIt - params.begin(), aParamValue, aNotation );
}


std::vector<wxString> SIM_MODEL_NGSPICE::getPinNames() const
{
    return ModelInfo( getModelType() ).pinNames;
}


SIM_MODEL_NGSPICE::MODEL_TYPE SIM_MODEL_NGSPICE::getModelType() const
{
    switch( GetType() )
    {
    case TYPE::NONE:                 return MODEL_TYPE::NONE;
    case TYPE::SW_V:                 return MODEL_TYPE::SWITCH;
    case TYPE::SW_I:                 return MODEL_TYPE::CSWITCH;
    case TYPE::D:                    return MODEL_TYPE::DIODE;

    case TYPE::NPN_GUMMELPOON:
    case TYPE::PNP_GUMMELPOON:       return MODEL_TYPE::BJT;
    case TYPE::NPN_VBIC:
    case TYPE::PNP_VBIC:             return MODEL_TYPE::VBIC;
    case TYPE::NPN_HICUM2:
    case TYPE::PNP_HICUM2:           return MODEL_TYPE::HICUM2;

    case TYPE::NJFET_SHICHMANHODGES:
    case TYPE::PJFET_SHICHMANHODGES: return MODEL_TYPE::JFET;
    case TYPE::NJFET_PARKERSKELLERN:
    case TYPE::PJFET_PARKERSKELLERN: return MODEL_TYPE::JFET2;

    case TYPE::NMES_STATZ:
    case TYPE::PMES_STATZ:           return MODEL_TYPE::MES;
    case TYPE::NMES_YTTERDAL:
    case TYPE::PMES_YTTERDAL:        return MODEL_TYPE::MESA;
    case TYPE::NMES_HFET1:
    case TYPE::PMES_HFET1:           return MODEL_TYPE::HFET1;
    case TYPE::NMES_HFET2:
    case TYPE::PMES_HFET2:           return MODEL_TYPE::HFET2;

    case TYPE::NMOS_MOS1:
    case TYPE::PMOS_MOS1:            return MODEL_TYPE::MOS1;
    case TYPE::NMOS_MOS2:
    case TYPE::PMOS_MOS2:            return MODEL_TYPE::MOS2;
    case TYPE::NMOS_MOS3:
    case TYPE::PMOS_MOS3:            return MODEL_TYPE::MOS3;
    case TYPE::NMOS_BSIM1:
    case TYPE::PMOS_BSIM1:           return MODEL_TYPE::BSIM1;
    case TYPE::NMOS_BSIM2:
    case TYPE::PMOS_BSIM2:           return MODEL_TYPE::BSIM2;
    case TYPE::NMOS_MOS6:
    case TYPE::PMOS_MOS6:            return MODEL_TYPE::MOS6;
    case TYPE::NMOS_BSIM3:
    case TYPE::PMOS_BSIM3:           return MODEL_TYPE::BSIM3;
    case TYPE::NMOS_MOS9:
    case TYPE::PMOS_MOS9:            return MODEL_TYPE::MOS9;
    case TYPE::NMOS_B4SOI:
    case TYPE::PMOS_B4SOI:           return MODEL_TYPE::B4SOI;
    case TYPE::NMOS_BSIM4:
    case TYPE::PMOS_BSIM4:           return MODEL_TYPE::BSIM4;
    case TYPE::NMOS_B3SOIFD:
    case TYPE::PMOS_B3SOIFD:         return MODEL_TYPE::B3SOIFD;
    case TYPE::NMOS_B3SOIDD:
    case TYPE::PMOS_B3SOIDD:         return MODEL_TYPE::B3SOIDD;
    case TYPE::NMOS_B3SOIPD:
    case TYPE::PMOS_B3SOIPD:         return MODEL_TYPE::B3SOIPD;
    case TYPE::NMOS_HISIM2:
    case TYPE::PMOS_HISIM2:          return MODEL_TYPE::HISIM2;
    case TYPE::NMOS_HISIMHV1:
    case TYPE::PMOS_HISIMHV1:        return MODEL_TYPE::HISIMHV1;
    case TYPE::NMOS_HISIMHV2:
    case TYPE::PMOS_HISIMHV2:        return MODEL_TYPE::HISIMHV2;

    default:
        wxFAIL_MSG( "Unhandled SIM_MODEL type in SIM_MODEL_NGSPICE" );
        return MODEL_TYPE::NONE;
    }
}


bool SIM_MODEL_NGSPICE::getIsOtherVariant()
{
    switch( GetType() )
    {
    case TYPE::PNP_GUMMELPOON:
    case TYPE::PNP_VBIC:
    case TYPE::PNP_HICUM2:
    case TYPE::PJFET_SHICHMANHODGES:
    case TYPE::PJFET_PARKERSKELLERN:
    case TYPE::PMES_STATZ:
    case TYPE::PMES_YTTERDAL:
    case TYPE::PMES_HFET1:
    case TYPE::PMES_HFET2:
    case TYPE::PMOS_MOS1:
    case TYPE::PMOS_MOS2:
    case TYPE::PMOS_MOS3:
    case TYPE::PMOS_BSIM1:
    case TYPE::PMOS_BSIM2:
    case TYPE::PMOS_MOS6:
    case TYPE::PMOS_BSIM3:
    case TYPE::PMOS_MOS9:
    case TYPE::PMOS_B4SOI:
    case TYPE::PMOS_BSIM4:
    case TYPE::PMOS_B3SOIFD:
    case TYPE::PMOS_B3SOIDD:
    case TYPE::PMOS_B3SOIPD:
    case TYPE::PMOS_HISIM2:
    case TYPE::PMOS_HISIMHV1:
    case TYPE::PMOS_HISIMHV2:
        return true;

    default:
        return false;
    }
}
