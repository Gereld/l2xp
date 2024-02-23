#pragma once

#include <Gx/layout/Layout.h>
#include <Gx/layout/Placeholder.h>

class CrashReportDialog : public WTL::CIndirectDialogImpl<CrashReportDialog> {
	enum {
		IDE_START = 1000,
		IDE_BTN_DETAILS,
		IDE_EDIT_DETAILS,
		IDE_BTN_COPY,
	};

public:
	CrashReportDialog( HWND hWndParent = NULL );
	~CrashReportDialog();

	void SetMessage( const std::string & message );
	void SetDetails( const std::string & details );

private:
	BOOL OnInitDialog( ATL::CWindow wndFocus, LPARAM lInitParam );
	void OnSysCommand( UINT nID, CPoint point );
	void OnGetMinMaxInfo( MINMAXINFO * lpMMI );
	void OnSize( UINT nType, CSize size );
	void OnClose( UINT uNotifyCode, int nID, ATL::CWindow wndCtl );
	void OnDetails( UINT uNotifyCode, int nID, ATL::CWindow wndCtl );
	void OnCopy( UINT uNotifyCode, int nID, ATL::CWindow wndCtl );

	void adjust_to_minimum_size( const layout::Size & size = layout::Size( -1, -1 ) );

	BEGIN_MSG_MAP_EX( CrashReportDialog )
		MSG_WM_INITDIALOG( OnInitDialog )
		MSG_WM_SYSCOMMAND( OnSysCommand )
		MSG_WM_SIZE( OnSize )
		MSG_WM_GETMINMAXINFO( OnGetMinMaxInfo )
		COMMAND_HANDLER_EX( IDOK, BN_CLICKED, OnClose );
		COMMAND_HANDLER_EX( IDE_BTN_DETAILS, BN_CLICKED, OnDetails );
		COMMAND_HANDLER_EX( IDE_BTN_COPY, BN_CLICKED, OnCopy );
	END_MSG_MAP()

	BEGIN_DIALOG( 0, 0, 100, 100 )
	DIALOG_CAPTION( "Crash Report" )
	DIALOG_STYLE( WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | DS_SHELLFONT | DS_MODALFRAME | DS_3DLOOK )
//	DIALOG_EXSTYLE( WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME | WS_EX_CONTROLPARENT )
	DIALOG_FONT( 8, "MS Shell Dlg" );
	END_DIALOG()

	BEGIN_CONTROLS_MAP()
	END_CONTROLS_MAP()
	
	std::shared_ptr<layout::ILayout> m_pLayout;
	std::shared_ptr<layout::Placeholder> m_pPlaceholder;
	std::shared_ptr<layout::ILayout> m_pBottom;
	WTL::CEdit m_details_ctrl;
	std::string m_details_str;
	std::string m_message;
};
