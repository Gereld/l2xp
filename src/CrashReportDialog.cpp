#include "CrashReportDialog.h"

#include <Gx/layout/All.h>
#include <wex/layout/Wnd.h>
#include <wex/misc.h>
#include <wtx/LayoutFactory.h>

// ************************************************************************************************

CrashReportDialog::CrashReportDialog( HWND hWndParent ) {
}

CrashReportDialog::~CrashReportDialog() {
}

BOOL CrashReportDialog::OnInitDialog( ATL::CWindow wndFocus, LPARAM lInitParam ) {
	m_details_ctrl.Create( m_hWnd, NULL, m_details_str.c_str(), WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VSCROLL | ES_READONLY | ES_MULTILINE | ES_WANTRETURN | WS_TABSTOP, WS_EX_CLIENTEDGE, IDE_EDIT_DETAILS );
	m_details_ctrl.SetFont( GetFont() );

	wtx::LayoutFactory f( m_hWnd, 4 );
	
	std::shared_ptr<layout::ILayout> pTop = f.vert()
		% layout::full_size( f.vert() % f.text( m_message ) )
		% layout::align( layout::ALIGN_RIGHT, f.horz()
			% f.button( "Close", IDOK )
			% layout::full_size( f.button( "Details...", IDE_BTN_DETAILS ) )
		) 
	;
	pTop = layout::min_minimum_size( layout::Size( 300, 100 ), pTop );

	// +toto+2024.02.20
	m_pBottom = f.vert()
		% layout::full_size( layout::min_minimum_size( layout::Size( 0, 100 ), wex::layout::wnd( m_details_ctrl.m_hWnd, layout::Size( 0, 0 ) ) ) )
		% layout::align( layout::ALIGN_RIGHT, f.button( "Copy to clipboard", IDE_BTN_COPY ).add_style( 0, WS_VISIBLE )	)
	;

	m_pPlaceholder = std::make_shared<layout::Placeholder>();

	std::shared_ptr<layout::ILayout> pLayout = f.vert()
		% pTop
		% layout::full_size( m_pPlaceholder );
	;

	m_pLayout = f.border( pLayout );
	adjust_to_minimum_size();

	GetDlgItem( IDOK ).SetFocus();
	CenterWindow( m_hWnd );

	return FALSE;
}

void CrashReportDialog::OnSysCommand( UINT nID, CPoint point ) {
	if( nID == SC_CLOSE ) {
		EndDialog( IDCANCEL );
	}
	else {
		SetMsgHandled( false );
	}
}

void CrashReportDialog::OnSize( UINT nType, CSize size ) {
	if( m_pLayout ) {
		m_pLayout->Update( layout::Rect( layout::Point( 0, 0 ), layout::Size( size.cx, size.cy ) ) );
	}
}

void CrashReportDialog::OnGetMinMaxInfo( MINMAXINFO * lpMMI ) {
	if( m_pLayout ) {
		layout::Size minSize = m_pLayout->MinimumSize();
		CRect rc( 0, 0, minSize.x(), minSize.y() );
		::AdjustWindowRectEx( rc, GetStyle(), GetMenu() != NULL, GetExStyle() );
		lpMMI->ptMinTrackSize.x = rc.Width();
		lpMMI->ptMinTrackSize.y = rc.Height();
	}
}

void CrashReportDialog::OnClose( UINT uNotifyCode, int nID, ATL::CWindow wndCtl ) {
	EndDialog( IDOK );
}

void CrashReportDialog::OnDetails( UINT uNotifyCode, int nID, ATL::CWindow wndCtl ) {
	if( !m_pPlaceholder->GetLayout() ) {
		m_pPlaceholder->SetLayout( m_pBottom );
		GetDlgItem( IDE_EDIT_DETAILS ).ShowWindow( SW_SHOW );
		GetDlgItem( IDE_BTN_COPY ).ShowWindow( SW_SHOW );
	}
	else {
		m_pPlaceholder->SetLayout( NULL );
		GetDlgItem( IDE_EDIT_DETAILS ).ShowWindow( SW_HIDE );
		GetDlgItem( IDE_BTN_COPY ).ShowWindow( SW_HIDE );
	}

	wex::ClientRect rc( m_hWnd );
	adjust_to_minimum_size( layout::Size( rc.Width(), 0 ) );
}

void CrashReportDialog::OnCopy( UINT uNotifyCode, int nID, ATL::CWindow wndCtl ) {
	m_details_ctrl.SetSel( 0, -1 );
	m_details_ctrl.Invalidate();
	m_details_ctrl.Copy();
}

// ************************************************************************************************

void CrashReportDialog::adjust_to_minimum_size( const layout::Size & size ) {
	layout::Size minSize( 100, 100 );
	if( m_pLayout ) {
		minSize = m_pLayout->MinimumSize();
	}
	CRect rc( 
		0, 
		0, 
		(size.x() == -1) ? minSize.x() : std::max<int>( minSize.x(), size.x() ),
		(size.y() == -1) ? minSize.y() : std::max<int>( minSize.y(), size.y() ) 
	);
	::AdjustWindowRectEx( rc, GetStyle(), GetMenu() != NULL, GetExStyle() );
	SetWindowPos( NULL, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE | SWP_NOZORDER );
}

void CrashReportDialog::SetDetails( const std::string & details ) {
	m_details_str = details;
}

void CrashReportDialog::SetMessage( const std::string & message ) {
	m_message = message;
}

