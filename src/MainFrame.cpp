#include "resource.h"
#include "MainFrame.h"

#include "Panel.h"
#include "Character.h"
#include "CharacterPanel.h"
#include "MainPanel.h"

#include <Gx/layout/All.h>
#include <wex/layout/Wnd.h>
#include <wex/misc.h>
#include <wtx/LayoutFactory.h>

using namespace layout;

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

layout::Size status_bar_minimum_size( HWND hWnd ) {
	wex::WindowRect rect( hWnd );
	return layout::Size( 0, rect.Height() );
}

MainFrame::MainFrame() {
}

MainFrame::~MainFrame() {
}

LRESULT MainFrame::OnCreate( LPCREATESTRUCT lpsc ) {
	wtx::LayoutFactory f( m_hWnd, 0 );

	m_pPlaceholder = std::make_shared<layout::Placeholder>();

	std::shared_ptr<layout::ILayout> pVert = f.vert()
		% std::make_shared<MainPanel>( m_hWnd )
		% f.separator_horz()
		% full_size( m_pPlaceholder	)
	;

	m_pLayout = pVert;

	rebuild();

	update_layout();

	return 0;
}

LRESULT MainFrame::OnDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled ) {
	LRESULT lResult = __super::OnDestroy( uMsg, wParam, lParam, bHandled );
	//DestroyWindow();
	return lResult;
}

void MainFrame::OnGetMinMaxInfo( MINMAXINFO * lpMMI ) {
	if( m_pLayout ) {
		layout::Size minSize = m_pLayout->MinimumSize();
		CRect rc( 0, 0, minSize.x(), minSize.y() );
		//CRect rc( 0, 0, 200, 300 );
		::AdjustWindowRectEx( rc, GetStyle(), GetMenu() != NULL, GetExStyle() );
		lpMMI->ptMinTrackSize.x = rc.Width();
		lpMMI->ptMinTrackSize.y = rc.Height();
	}
}

// ************************************************************************************************

void MainFrame::UpdateLayout( BOOL bResizeBars ) {
	update_layout();
	__super::UpdateLayout( bResizeBars );
}

// ************************************************************************************************

void MainFrame::update_layout() {
	if( m_pLayout ) {
		wex::ClientRect rc( m_hWnd );
		m_pLayout->Update( layout::Rect( layout::Point( rc.left, rc.top ), layout::Size( rc.Width(), rc.Height() ) ) );
	}
}

void MainFrame::update_character( Character * pCharacter ) {
	SendMessage( WM_USER + 1, reinterpret_cast<WPARAM>( pCharacter ), 0 );
}

void MainFrame::remove_character( Character * pCharacter ) {
}

void MainFrame::rebuild() {
	if( m_panels.empty() ) {
		m_pPlaceholder->SetLayout( std::make_shared<CharacterPanel>( m_hWnd, nullptr ) );
	}
	else {
		wtx::LayoutFactory f( m_hWnd, 0 );

		layout::Stream<layout::Vert> s = f.vert();

		int n = 0;
		for( auto i = m_panels.begin(); i != m_panels.end(); ++i ) {
			Panel_map::value_type & v = *i;
			
			if( n != 0 ) {
				s % f.separator_horz();
			}

			s % v.second;

			n++;
		}

		m_pPlaceholder->SetLayout( s );
	}

	adjust_to_minimum_size();
}

void MainFrame::adjust_to_minimum_size() {
	layout::Size minSize( 100, 100 );
	if( m_pLayout ) {
		minSize = m_pLayout->MinimumSize();
	}
	CRect rc( 0, 0, minSize.x(), minSize.y() );
	::AdjustWindowRectEx( rc, GetStyle(), GetMenu() != NULL, GetExStyle() );
	SetWindowPos( NULL, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE | SWP_NOZORDER );
	if( m_pLayout ) {
		m_pLayout->Update( layout::Rect( layout::Point( 0, 0 ), layout::Point( minSize.x(), minSize.y() ) ) );
	}
}

LRESULT MainFrame::OnUser( UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	update_character_( reinterpret_cast<Character *>( wParam ) );
	return 0;
}

void MainFrame::update_character_( Character * pCharacter ) {
	std::shared_ptr<CharacterPanel> pPanel;

	auto pos = m_panels.find( pCharacter );
	if( pos == m_panels.end() ) {
		pPanel = std::make_shared<CharacterPanel>( m_hWnd, pCharacter );
		m_panels[pCharacter] = pPanel;
		rebuild();
	}
	else {
		pPanel = pos->second;
	}

	pPanel->update();
}
