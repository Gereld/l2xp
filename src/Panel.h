#pragma once

#include <Gx/layout/Layout.h>

class Panel : public WTL::CIndirectDialogImpl<Panel>, public layout::ILayout {
public:
	Panel();
	virtual ~Panel();

	virtual layout::Size PreferedSize() const;
	virtual layout::Size MinimumSize() const;
	virtual void Update( const layout::Rect & rect );

	void SetLayout( const std::shared_ptr<layout::ILayout> & pLayout );

private:
	void OnSize( UINT nType, CSize size );

	BEGIN_MSG_MAP( Panel )
//		MSG_WM_SIZE( OnSize )
	END_MSG_MAP()

	BEGIN_DIALOG( 0, 0, 0, 0 )
	DIALOG_STYLE( WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | DS_SHELLFONT )
	DIALOG_FONT( 8, "MS Shell Dlg" );
	END_DIALOG()

	BEGIN_CONTROLS_MAP()
	END_CONTROLS_MAP()
	
	std::shared_ptr<layout::ILayout> m_pLayout;
};
