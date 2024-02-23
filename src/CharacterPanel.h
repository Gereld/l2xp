#pragma once

#include "Panel.h"
#include "Character.h"

class CharacterPanel : public Panel {
	enum {
		IDE_START = 1000,
		IDE_TEXT_NAME,
		IDE_TEXT_TARGET_ID,

		IDE_BTN_RESET_XP_SP,
		IDE_TEXT_XP,
		IDE_TEXT_SP,

		IDE_TEXT_ADENA,
		IDE_BTN_RESET_ADENA,

		IDE_TEXT_HIT_COUNT,
		IDE_TEXT_DAMAGE,
		IDE_TEXT_NORMAL_DAMAGE,
		IDE_TEXT_DPS,

		IDE_TEXT_PCRIT_RATE,
		IDE_TEXT_PCRIT_DAMAGE,
		IDE_TEXT_PCRIT_MULTIPLIER,

		IDE_TEXT_MCRIT_RATE,
		IDE_TEXT_MCRIT_DAMAGE,
		IDE_TEXT_MCRIT_MULTIPLIER,

		IDE_CHECK_COMBAT_MODE,
		IDE_BTN_RESET_ALL,
	};

public:
	Character * m_pCharacter = nullptr;

public:
	CharacterPanel( HWND hWndParent, Character * pCharacter );

	void update();

protected:
	virtual BOOL OnInitDialog( ATL::CWindow wndFocus, LPARAM lInitParam );
	void OnReset_XP_SP( UINT uNotifyCode, int nID, ATL::CWindow wndCtl );
	void OnReset_Adena( UINT uNotifyCode, int nID, ATL::CWindow wndCtl );
	void OnReset_All( UINT uNotifyCode, int nID, ATL::CWindow wndCtl );
	void OnCombatMode( UINT uNotifyCode, int nID, ATL::CWindow wndCtl );

	void set_text( UINT nID, const std::string & text );
	void set_text( UINT nID, double value );

	BEGIN_MSG_MAP_EX( CharacterPanel )
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_HANDLER_EX( IDE_BTN_RESET_XP_SP, BN_CLICKED, OnReset_XP_SP );
		COMMAND_HANDLER_EX( IDE_BTN_RESET_ADENA, BN_CLICKED, OnReset_Adena );
		COMMAND_HANDLER_EX( IDE_BTN_RESET_ALL, BN_CLICKED, OnReset_All );
		COMMAND_HANDLER_EX( IDE_CHECK_COMBAT_MODE, BN_CLICKED, OnCombatMode );
		CHAIN_MSG_MAP( Panel )
	END_MSG_MAP()
};