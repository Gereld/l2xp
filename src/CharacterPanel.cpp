#include "CharacterPanel.h"
#include "Document.h"

#include <Gx/layout/All.h>
#include <wex/layout/Wnd.h>
#include <wex/misc.h>
#include <wtx/LayoutFactory.h>

#include <boost/format.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

// ************************************************************************************************

namespace {

std::string format( double value ) {
	if( value < 1000 ) {
		return boost::lexical_cast<std::string>( floor( value ) );
	}
	value /= 1000;
	if( value < 1000 ) {
		return (boost::format( "%.1f" ) % value).str() + "k";
	}
	value /= 1000;
	return (boost::format( "%.1f" ) % value).str() + "m";
}

std::string format( double v, double v_per_hour ) {
	return format( v ) + " (" + format( v_per_hour ) + "/h)";
}

std::string format_damage( double value ) {
	return boost::lexical_cast<std::string>( int( value ) );
}

} // namespace 

// ************************************************************************************************

CharacterPanel::CharacterPanel( HWND hWndParent, Character * pCharacter ) {
	Create( hWndParent );
	m_pCharacter = pCharacter;
}

BOOL CharacterPanel::OnInitDialog( ATL::CWindow wndFocus, LPARAM lInitParam ) {
	wtx::LayoutFactory f( m_hWnd, 4 );
	
	unsigned int text_width = 100;

	using namespace boost::placeholders;

	boost::function<layout::SizeItem ( const std::shared_ptr<layout::ILayout> & )> c1 = 
		boost::bind( 
			&layout::full_size, 
			boost::bind( &layout::Modifier::Filter, std::shared_ptr<layout::Modifier>( new layout::ModifierSameWidth() ), _1 )
		); 
	boost::function<layout::SizeItem ( const std::shared_ptr<layout::ILayout> & )> c2 = 
		boost::bind( 
			&layout::full_size, 
			boost::bind( &layout::Modifier::Filter, std::shared_ptr<layout::Modifier>( new layout::ModifierSameWidth() ), _1 )
		); 
	boost::function<layout::SizeItem ( const std::shared_ptr<layout::ILayout> & )> c3 = 
		boost::bind( 
			&layout::full_size, 
			boost::bind( &layout::Modifier::Filter, std::shared_ptr<layout::Modifier>( new layout::ModifierSameWidth() ), _1 )
		); 

	std::shared_ptr<layout::ILayout> pLayout = f.vert()
		% ( f.horz()
			% layout::full_size( f.text( "Not connected" ).id( IDE_TEXT_NAME ) )
			% layout::full_size( f.text( "" ).id( IDE_TEXT_TARGET_ID ) )
		)
		% ( f.horz() 
			% c1( f.horz() % f.text( "XP:" ) % layout::full_size( layout::min_minimum_size( layout::Size( text_width, 0 ), f.text().id( IDE_TEXT_XP ) ) ) )
			% f.separator_vert()
			% c2( f.horz() % f.text( "SP:" ) % layout::full_size( layout::min_minimum_size( layout::Size( text_width, 0 ), f.text().id( IDE_TEXT_SP ) ) ) )
			% f.separator_vert()
			% c3( f.horz() % f.text( "Adena:" ) % layout::full_size( layout::min_minimum_size( layout::Size( text_width, 0 ), f.text().id( IDE_TEXT_ADENA ) ) ) )
		)
		% ( f.horz() 
			% c1( f.vert() 
				% ( f.horz() % f.text( "Hit:" ) % layout::full_size( f.text().id( IDE_TEXT_HIT_COUNT ) ) )
				% ( f.horz() % f.text( "Dmg:" ) % layout::full_size( f.text().id( IDE_TEXT_DAMAGE ) ) )
				% ( f.horz() % f.text( "Dmg (wo/Crit):" ) % layout::full_size( f.text().id( IDE_TEXT_NORMAL_DAMAGE ) ) )
				% ( f.horz() % f.text( "DPS:" ) % layout::full_size( f.text().id( IDE_TEXT_DPS ) ) )
			)
			% f.separator_vert()
			% c2( f.vert()
				% ( f.horz() % f.text( "P.Crit:" ) % layout::full_size( f.text().id( IDE_TEXT_PCRIT_RATE ) ) )
				% ( f.horz() % f.text( "Dmg:" ) % layout::full_size( f.text().id( IDE_TEXT_PCRIT_DAMAGE ) ) )
				% ( f.horz() % f.text( "Mul:" ) % layout::full_size( f.text().id( IDE_TEXT_PCRIT_MULTIPLIER ) ) )
			)
			% f.separator_vert()
			% c3( f.vert()
				% ( f.horz() % f.text( "M.Crit:" ) % layout::full_size( f.text().id( IDE_TEXT_MCRIT_RATE ) ) )
				% ( f.horz() % f.text( "Dmg:" ) % layout::full_size( f.text().id( IDE_TEXT_MCRIT_DAMAGE ) ) )
				% ( f.horz() % f.text( "Mul:" ) % layout::full_size( f.text().id( IDE_TEXT_MCRIT_MULTIPLIER ) ) )
			)
		)
		% ( f.horz()
			% layout::full_size( f.check_box( "Combat mode only", IDE_CHECK_COMBAT_MODE ) )
			% f.button( "Reset", IDE_BTN_RESET_ALL )
		)
	;

	SetLayout( f.border( pLayout ) );
	update();

	return FALSE;
}

void CharacterPanel::update() {
	std::string name = "Unknown";

	double xp = 0;
	double xp_per_hour = 0;

	double sp = 0;
	double sp_per_hour = 0;

	double adena = 0;
	double adena_per_hour = 0;

	std::string last_target_id;

	__int64 hit_count = 0;
	double average_damage = 0;
	double average_normal_damage = 0;
	double damage_per_second = 0;

	std::string pcrit_rate;
	double pcrit_damage = 0;
	double pcrit_multiplier = 0;

	std::string mcrit_rate;
	double mcrit_damage = 0;
	double mcrit_multiplier = 0;

	if( m_pCharacter ) {
		name = m_pCharacter->get_name();

		xp = double( m_pCharacter->get_xp() );
		xp_per_hour = m_pCharacter->get_xp_per_hour();

		sp = m_pCharacter->get_sp();
		sp_per_hour = m_pCharacter->get_sp_per_hour();

		adena = double( m_pCharacter->get_adena() );
		adena_per_hour = m_pCharacter->get_adena_per_hour();

		last_target_id = boost::lexical_cast<std::string>( m_pCharacter->get_last_target_id() );

		hit_count = m_pCharacter->get_hit_count();
		average_damage = m_pCharacter->get_average_damage();
		average_normal_damage = m_pCharacter->get_average_normal_damage();
		damage_per_second = m_pCharacter->get_damage_per_second();

		pcrit_rate = (boost::format( "%.1f%% (%d)" ) % (m_pCharacter->get_pcrit_rate() * 100) % m_pCharacter->get_pcrit_count()).str();
		pcrit_damage = m_pCharacter->get_pcrit_damage();
		pcrit_multiplier = m_pCharacter->get_pcrit_multiplier();

		mcrit_rate = (boost::format( "%.1f%% (%d)" ) % (m_pCharacter->get_mcrit_rate() * 100) % m_pCharacter->get_mcrit_count()).str();
		mcrit_damage = m_pCharacter->get_mcrit_damage();
		mcrit_multiplier = m_pCharacter->get_mcrit_multiplier();
	}

	set_text( IDE_TEXT_NAME, name );

	set_text( IDE_TEXT_XP, format( xp, xp_per_hour ) );
	set_text( IDE_TEXT_SP, format( sp, sp_per_hour ) );
	set_text( IDE_TEXT_ADENA, format( adena, adena_per_hour ) );
	set_text( IDE_TEXT_TARGET_ID, last_target_id );

	set_text( IDE_TEXT_HIT_COUNT, boost::lexical_cast<std::string>( hit_count ) );
	set_text( IDE_TEXT_DAMAGE, format_damage( average_damage ) );
	set_text( IDE_TEXT_NORMAL_DAMAGE, format_damage( average_normal_damage ) );
	set_text( IDE_TEXT_DPS, (boost::format( "%.1f" ) % damage_per_second).str() );

	set_text( IDE_TEXT_PCRIT_RATE, pcrit_rate );
	set_text( IDE_TEXT_PCRIT_DAMAGE, format_damage( pcrit_damage ) );
	set_text( IDE_TEXT_PCRIT_MULTIPLIER, (boost::format( "%.1f" ) % pcrit_multiplier).str() );

	set_text( IDE_TEXT_MCRIT_RATE, mcrit_rate );
	set_text( IDE_TEXT_MCRIT_DAMAGE, format_damage( mcrit_damage ) );
	set_text( IDE_TEXT_MCRIT_MULTIPLIER, (boost::format( "%.1f" ) % mcrit_multiplier).str() );
}

void CharacterPanel::OnReset_XP_SP( UINT uNotifyCode, int nID, ATL::CWindow wndCtl ) {
	if( m_pCharacter ) {
		get_document()->reset_xp( m_pCharacter );
		get_document()->reset_sp( m_pCharacter );
		get_document()->reset_damage( m_pCharacter );
	}
}

void CharacterPanel::OnReset_Adena( UINT uNotifyCode, int nID, ATL::CWindow wndCtl ) {
	if( m_pCharacter ) {
		get_document()->reset_adena( m_pCharacter );
	}
}

void CharacterPanel::set_text( UINT nID, const std::string & text ) {
	SetDlgItemText( nID, text.c_str() );
}

void CharacterPanel::set_text( UINT nID, double value ) {
	std::string text = (boost::format( "%.2f" ) % value).str();
	set_text( nID, text );
}

void CharacterPanel::OnReset_All( UINT uNotifyCode, int nID, ATL::CWindow wndCtl ) {
	if( m_pCharacter ) {
		get_document()->reset_all( m_pCharacter );
	}
}

void CharacterPanel::OnCombatMode( UINT uNotifyCode, int nID, ATL::CWindow wndCtl ) {
	if( m_pCharacter ) {
		get_document()->set_combat_mode( m_pCharacter, IsDlgButtonChecked( IDE_CHECK_COMBAT_MODE ) == BST_CHECKED );
	}
}
