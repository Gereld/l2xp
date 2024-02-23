#pragma once

#include <atlctrls.h>
#include <atlsplit.h>
#include <Gx/layout/Layout.h>
#include <Gx/layout/Placeholder.h>

class Character;
class CharacterPanel;

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

class MainFrame : public WTL::CFrameWindowImpl<MainFrame> {
public:
	DECLARE_FRAME_WND_CLASS( _T( "l2xp_MainFrame" ), 0 );

	typedef WTL::CFrameWindowImpl<MainFrame> Base;

public:
	MainFrame();
	~MainFrame();

	void UpdateLayout( BOOL bResizeBars = TRUE );

	void update_character( Character * pCharacter );
	void remove_character( Character * pCharacter );

private:
	LRESULT	OnCreate( LPCREATESTRUCT lpsc );
	void	OnSize( UINT wParam, const CSize & size );
	LRESULT OnDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
	void OnGetMinMaxInfo( MINMAXINFO * lpMMI );
	LRESULT OnUser( UINT uMsg, WPARAM wParam, LPARAM lParam );

	BEGIN_MSG_MAP_EX( MainFrame )
		MESSAGE_HANDLER_EX( WM_USER + 1, OnUser )
		MSG_WM_CREATE( OnCreate )
		MSG_WM_GETMINMAXINFO( OnGetMinMaxInfo )
		MESSAGE_HANDLER( WM_DESTROY, OnDestroy )
		CHAIN_MSG_MAP( WTL::CFrameWindowImpl<MainFrame> )
	END_MSG_MAP()

private:
	std::shared_ptr<layout::ILayout> m_pLayout;
	std::shared_ptr<layout::Placeholder> m_pPlaceholder;
	
	typedef std::map<Character *, std::shared_ptr<CharacterPanel> > Panel_map;
	Panel_map m_panels;

	void update_layout();
	void rebuild();
	void adjust_to_minimum_size();
	void update_character_( Character * pCharacter );
};
