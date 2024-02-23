#pragma once

#include "Panel.h"
#include <wex/RadioGroup.h>

class MainPanel : public Panel {
	enum {
		IDE_START = 1000,
		IDE_RADIO_SNIFFER,
		IDE_RADIO_REPLAY,
		IDE_BTN_TEST,
		IDE_CHECK_DUMP,
		IDE_CHECK_LOG,
		IDE_EDIT_DUMP,
		IDE_TEXT_REPLAY,
		IDE_BTN_BROWSE,
		IDE_BTN_REPLAY,
	};

public:
	MainPanel( HWND hWndParent );

protected:
	virtual BOOL OnInitDialog( ATL::CWindow wndFocus, LPARAM lInitParam );
	virtual void OnTest( UINT uNotifyCode, int nID, ATL::CWindow wndCtl );
	virtual void OnRadioMode( UINT uNotifyCode, int nID, ATL::CWindow wndCtl );
	virtual void OnReplay( UINT uNotifyCode, int nID, ATL::CWindow wndCtl );
	virtual void OnBrowse( UINT uNotifyCode, int nID, ATL::CWindow wndCtl );
	virtual void OnDump( UINT uNotifyCode, int nID, ATL::CWindow wndCtl );
	virtual void OnLog( UINT uNotifyCode, int nID, ATL::CWindow wndCtl );

	void update_radio_mode();

	BEGIN_MSG_MAP_EX( CharacterPanel )
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_HANDLER_EX( IDE_BTN_TEST, BN_CLICKED, OnTest );
		COMMAND_HANDLER_EX( IDE_RADIO_SNIFFER, BN_CLICKED, OnRadioMode );
		COMMAND_HANDLER_EX( IDE_RADIO_REPLAY, BN_CLICKED, OnRadioMode );
		COMMAND_HANDLER_EX( IDE_BTN_REPLAY, BN_CLICKED, OnReplay );
		COMMAND_HANDLER_EX( IDE_BTN_BROWSE, BN_CLICKED, OnBrowse );
		COMMAND_HANDLER_EX( IDE_CHECK_DUMP, BN_CLICKED, OnDump );
		COMMAND_HANDLER_EX( IDE_CHECK_LOG, BN_CLICKED, OnLog );
		CHAIN_MSG_MAP( Panel )
	END_MSG_MAP()

	std::shared_ptr<wex::RadioGroup<bool> > m_pRadioMode;
};
