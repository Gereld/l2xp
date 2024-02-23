#include "Panel.h"

#include <wex/misc.h>
#include <Gx/layout/Layout.h>
#include <Gx/layout/Types.h>

Panel::Panel() {
}

Panel::~Panel() {
	if( IsWindow() ) {
		DestroyWindow();
	}
}

layout::Size Panel::PreferedSize() const {
	return MinimumSize();
}

layout::Size Panel::MinimumSize() const {
	if( m_pLayout ) {
		return m_pLayout->MinimumSize();
	}

	return layout::Size( 0, 0 );
}

void Panel::Update( const layout::Rect & rect ) {
	// +fix+2006.02.10 : qd on ajoute une page les fenètres sont détruites mais les instances persistent.
	if( m_hWnd == NULL ) {
		return;
	}

	SetWindowPos( NULL, rect.x(), rect.y(), rect.w(), rect.h(), SWP_NOZORDER );

	if( m_pLayout ) {
		// +modif+2006.02.08 : qd la fenètre ne change pas de taille WM_SIZE n'est pas envoyé
		wex::ClientRect cr( m_hWnd );

		if( (cr.Width() == rect.w()) && (cr.Height() == rect.h()) ) {
			m_pLayout->Update( layout::Rect( layout::Point( 0, 0 ), layout::Size( rect.w(), rect.h() ) ) );
		}
	}
}

void Panel::SetLayout( const std::shared_ptr<layout::ILayout> & pLayout ) {
	m_pLayout = pLayout;
}


void Panel::OnSize( UINT nType, CSize size ) {
	if( m_pLayout ) {
		m_pLayout->Update( layout::Rect( layout::Point( 0, 0 ), layout::Size( size.cx, size.cy ) ) );
	}
}
