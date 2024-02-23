#include "l2xp.h"

#include "Document.h"
#include "MainFrame.h"
#include "StackWalker.h"
#include "CrashReportDialog.h"

#include <boost/algorithm/string/replace.hpp>
#include <fstream>

class MyStackWalker : public StackWalker {
public:
	std::string m_string;

public:
	virtual void OnLoadModule( LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size, DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion ) {
	}

	virtual void OnOutput( LPCSTR szText ) {
		m_string += szText;
	}
};

LONG WINAPI exception_filter( __in struct _EXCEPTION_POINTERS * ExceptionInfo ) {
	MyStackWalker sw; 
	sw.ShowCallstack( GetCurrentThread(), ExceptionInfo->ContextRecord );

	std::ofstream log( "d:/log.txt" );
	log << sw.m_string;

	boost::algorithm::replace_all( sw.m_string, "\n", "\r\n" );

	CrashReportDialog dlg;
	dlg.SetMessage( "Unhandled exception." );
	dlg.SetDetails( sw.m_string );
	dlg.DoModal();

	// do DrWatson crap
	// return EXCEPTION_CONTINUE_SEARCH; 

	// just exit: this might prevent some cleanup routines from running,
	// but then again we're crashing so does it matter?
	return EXCEPTION_EXECUTE_HANDLER;
}

int set_exception_filter( void ) {
	SetUnhandledExceptionFilter( exception_filter );
	return 1;
}

int exception_filter_set = set_exception_filter(); // or can just call from main()
/**/

// Global Variables:
WTL::CAppModule _Module;

static Document m_document;
Document * get_document() {
	return &m_document;
}

int APIENTRY _tWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow )
{
	_Module.Init( NULL, hInstance );

	::INITCOMMONCONTROLSEX icex;
	// Ensure that the common control DLL is loaded. 
	icex.dwSize = sizeof( INITCOMMONCONTROLSEX );
	icex.dwICC  = ICC_BAR_CLASSES | ICC_COOL_CLASSES;
	::InitCommonControlsEx( &icex );

	setlocale( LC_CTYPE, ".ACP" );
	
	MainFrame wndMain;
 
    // Create the main window
	if( wndMain.CreateEx() == NULL ) {
        return 1;       // Window creation failed
	}
 
	wndMain.SetWindowText( "l2xp" );

    // Show the window
	wndMain.SetWindowPos( NULL, 0, 0, 1, 1, SWP_NOMOVE | SWP_NOZORDER );
    wndMain.ShowWindow( nCmdShow );
    wndMain.UpdateWindow();

	get_document()->m_pFrame = &wndMain;
	get_document()->start_capture();

	//get_document()->set_name( Connection( Endpoint( 0, 0, 0, 0, 0 ), Endpoint( 1, 0, 0, 0, 0 ) ), "Alessa" );

	HACCEL hAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDC_L2XP ) );

	// Main message loop:
	MSG msg;
	while( GetMessage( &msg, NULL, 0, 0 ) ) {
		if( !TranslateAccelerator( msg.hwnd, hAccelTable, &msg ) ) {
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	_Module.Term();

	return (int) msg.wParam;
}
