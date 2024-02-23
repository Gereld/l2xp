#include "MainPanel.h"

#include "CrashReportDialog.h"
#include "Document.h"

#include <Gx/layout/All.h>
#include <wex/layout/Wnd.h>
#include <wex/misc.h>
#include <wtx/LayoutFactory.h>

MainPanel::MainPanel( HWND hWndParent ) {
	Create( hWndParent );
}

BOOL MainPanel::OnInitDialog( ATL::CWindow wndFocus, LPARAM lInitParam ) {
	wtx::LayoutFactory f( m_hWnd, 4 );
	
	std::shared_ptr<layout::ILayout> pSniffer = f.vert()
		% f.check_box( "Dump packets", IDE_CHECK_DUMP ).add_style( 0, WS_VISIBLE )
		% f.check_box( "Generate log file", IDE_CHECK_LOG ).add_style( 0, WS_VISIBLE ) 
	;

	std::shared_ptr<layout::ILayout> pReplay = f.vert()
		% ( f.horz()
			% f.text( "Dump file :" ).id( IDE_TEXT_REPLAY ).add_style( 0, WS_VISIBLE )
			% layout::full_size( f.edit( "", IDE_EDIT_DUMP ).add_style( 0, WS_VISIBLE ) )
			% f.button( "...", IDE_BTN_BROWSE ).add_style( 0, WS_VISIBLE )
		)
		% layout::align( layout::ALIGN_HVCENTER, f.button( "Replay", IDE_BTN_REPLAY ).add_style( 0, WS_VISIBLE ) )
	;

	std::shared_ptr<layout::ILayout> pLayout = f.vert()
		% ( f.horz()
		% layout::full_size( f.vert()
				% f.radio( "Sniffer mode", IDE_RADIO_SNIFFER ).add_style( WS_GROUP )
				% f.radio( "Replay mode", IDE_RADIO_REPLAY )
			)
			//% f.button( "Test", IDE_BTN_TEST )
		)
		% layout::maximum( pSniffer, pReplay )
	;

	SetLayout( f.border( pLayout ) );

	m_pRadioMode = std::make_shared<wex::RadioGroup<bool>>( m_hWnd );
	m_pRadioMode->Add( IDE_RADIO_SNIFFER, true );
	m_pRadioMode->Add( IDE_RADIO_REPLAY, false );
	m_pRadioMode->SetSelection( true );

	update_radio_mode();

	return FALSE;
}

void MainPanel::OnTest( UINT uNotifyCode, int nID, ATL::CWindow wndCtl ) {
	//CrashReportDialog dlg;

	//dlg.SetMessage( "Message." );

	//std::string text;
	//for( int i = 0; i < 100; ++i ) {
	//	text += "Text.\r\n";
	//}
	//dlg.SetDetails( text );

	//dlg.DoModal();

	int * p = NULL;
	*p = 1;
}

void MainPanel::OnRadioMode( UINT uNotifyCode, int nID, ATL::CWindow wndCtl ) {
	update_radio_mode();

	get_document()->change_mode( m_pRadioMode->GetSelection() );
}

void MainPanel::update_radio_mode() {
	if( m_pRadioMode->GetSelection() ) {
		GetDlgItem(	IDE_CHECK_DUMP ).ShowWindow( SW_SHOW );
		GetDlgItem(	IDE_CHECK_LOG ).ShowWindow( SW_SHOW );

		GetDlgItem(	IDE_EDIT_DUMP ).ShowWindow( SW_HIDE );
		GetDlgItem(	IDE_TEXT_REPLAY ).ShowWindow( SW_HIDE );
 		GetDlgItem(	IDE_BTN_BROWSE ).ShowWindow( SW_HIDE );
		GetDlgItem(	IDE_BTN_REPLAY ).ShowWindow( SW_HIDE );
	}
	else {
		GetDlgItem(	IDE_EDIT_DUMP ).ShowWindow( SW_SHOW );
		GetDlgItem(	IDE_TEXT_REPLAY ).ShowWindow( SW_SHOW );
 		GetDlgItem(	IDE_BTN_BROWSE ).ShowWindow( SW_SHOW );
		GetDlgItem(	IDE_BTN_REPLAY ).ShowWindow( SW_SHOW );

		GetDlgItem(	IDE_CHECK_DUMP ).ShowWindow( SW_HIDE );
		GetDlgItem(	IDE_CHECK_LOG ).ShowWindow( SW_HIDE );
	}
}

void MainPanel::OnReplay( UINT uNotifyCode, int nID, ATL::CWindow wndCtl ) {
	get_document()->start_replay();
}

void MainPanel::OnBrowse( UINT uNotifyCode, int nID, ATL::CWindow wndCtl ) {
	WTL::CFileDialog dlg( true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Dump File (*.bin) \0*.bin\0\0" );

	if( dlg.DoModal() == IDOK ) {
		get_document()->m_dump_source = dlg.m_szFileName;
		SetDlgItemText( IDE_EDIT_DUMP, get_document()->m_dump_source.c_str() );
	}
}

void MainPanel::OnDump( UINT uNotifyCode, int nID, ATL::CWindow wndCtl ) {
	get_document()->set_dump( IsDlgButtonChecked( IDE_CHECK_DUMP ) == BST_CHECKED );
}

void MainPanel::OnLog( UINT uNotifyCode, int nID, ATL::CWindow wndCtl ) {
	get_document()->set_log( IsDlgButtonChecked( IDE_CHECK_LOG ) == BST_CHECKED );
}
