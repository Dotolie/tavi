//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : ImageViewer Info. Window
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/ImageViewer/IVInfoWnd.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: IVInfoWnd.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.6  2005/08/25 04:42:11  zstein
// - fixed bugs
//
// Revision 1.5  2005/08/03 03:41:11  zstein
// - updated
//
// Revision 1.4  2005/07/20 11:53:06  zstein
// - Updated UI
//
// Revision 1.3  2005/07/20 11:47:19  zstein
// - Updated UI.
//
// Revision 1.2  2005/07/20 06:13:44  zstein
// - updated UI.
//
// Revision 1.1  2005/04/27 13:29:37  zstein
// Image information window.
//
//
//*******************************************************************************
#include "IVInfoWnd.h"
#include "../Lib/LibImage.h"
#include <Component/String.h>
#include <Component/FileUtil.h>
#include "IViewer.h"


// Macros
#define MARGIN					5	
#define SCROLLBAR_WIDTH		6
#define SPACING					4


// Extern Variables
extern CTextEngine_BDF* pT;


// Local Variables
struct exifmatch {
	char* fname; // fieldname
	int id;
};

static struct exifmatch s_exifmatch[] = {
	{ "FNumber", ML_PV_META_INFO_FSTOP },
	{ "ExposureTime", ML_PV_META_INFO_SHUTTER_SPEED },
	{ "Model", ML_PV_META_INFO_CAMERA_MODEL },
	{ "ExposureBiasValue", ML_PV_META_INFO_EXPOSURE_BIAS },
	{ "WhiteBalance", ML_PV_META_INFO_WHITE_BALANCE },
	{ "FocalLength", ML_PV_META_INFO_FOCAL_LENGTH },
	{ "MeteringMode", ML_PV_META_INFO_METERING_MODE },
	{ "ISOSpeedRatings", ML_PV_META_INFO_ISO_SPEED_RATE },
	{ "DateTime",ML_PV_META_INFO_DATE_PHOTO },
	{ "Flash", ML_PV_META_INFO_FLASH },
	{ 0, 0 }
};

static int Match( const char* name )
{
	int i=0;
	char* pName;
	int ret = -1;

	if( !name ) return -1;
	
	while( (pName = s_exifmatch[i].fname) ) {
		if( strcmp(pName, name) == 0 ) {
			ret = s_exifmatch[i].id;
			break;
		}
		i++;
	}

	return ret;
}

CIVInfoWnd::CIVInfoWnd( CWindow* parent, const CString& szPath, int nMedia ) :
	CModalWnd( parent ),
	m_nMedia( nMedia )
{
	m_nBackgroundColor = 24;
	m_nExitResID = ML_EXIT;
	m_szTitle.SetString( (char*)pT->GetTextRes(ML_META_INFO), true );

	// Create scrollbar
	m_pScrollBar = new CVScrollBar( this, 0, 0 );
	m_pScrollBar->SetBorderWidth( 1 );
	m_pScrollBar->SetScrollBackgroundColor( 28 );
	m_pScrollBar->SetScrollColor( 17 ); // White
	m_pScrollBar->SetBorderColor( 24 );

	switch( nMedia ) {
	case MEDIA_JPEG:
		ParseEXIF( szPath );
		break;
	}

	m_nLine = 0;
}

CIVInfoWnd::~CIVInfoWnd( void )
{
}

void CIVInfoWnd::InsertProp( struct exifprop *list )
{
	while (list) {
		/* Take care of point-and-shoot values. */
		if (list->lvl == ED_PAS) list->lvl = ED_CAM;

		/* For now, just treat overridden & bad values as verbose. */
		if (list->lvl == ED_OVR || list->lvl == ED_BAD)
			list->lvl = ED_VRB;

		if (list->lvl == ED_CAM || list->lvl == ED_IMG) {
			int id = Match( list->name );
			if( id >= 0 ) {
				struct EXIFInfo* pInfo = new struct EXIFInfo;
				pInfo->m_nID = id;
				pInfo->m_szValue = list->str;
				m_List.Add( pInfo );
			}
		}
		list = list->next;
	}
}

void CIVInfoWnd::ParseEXIF( const CString& path )
{
	int mark, first;
	unsigned int len, rlen;
	unsigned char *exifbuf;
	struct exiftags *t;

	first = 0;
	exifbuf = NULL;

	int fp;

	fp = exif_fopen( (const char*)path );
	if( fp < 0 ) {
		printf( "failed to open file: 0x%x\n", -fp );
		return;
	}

	struct EXIFInfo* pInfo = new struct EXIFInfo;
	extern int jpeg_real_height;
	extern int jpeg_real_width;
	char buf[10];
	pInfo->m_nID = ML_WIDTH;
	sprintf( buf, "%d", jpeg_real_width );
	pInfo->m_szValue = buf;
	m_List.Add( pInfo );
	pInfo = new struct EXIFInfo;
	pInfo->m_nID = ML_HEIGHT;
	sprintf( buf, "%d", jpeg_real_height );
	pInfo->m_szValue = buf;
	m_List.Add( pInfo );

	while (jpegscan(fp, &mark, &len, !(first++))) {
		if (mark != JPEG_M_APP1) {
			if (exif_fseek(fp, len, SEEK_CUR))
				break;
			continue;
		}

		exifbuf = (unsigned char *)malloc(len);
		if (!exifbuf)
			break;

		rlen = exif_fread( (char*)exifbuf, (int)len, fp );
		if (rlen != len) {
			exifwarn("error reading JPEG (length mismatch)");
			free(exifbuf);
			return;
		}

		t = exifparse(exifbuf, len);

		if (t && t->props) {
			InsertProp( t->props );
		}
		exiffree(t);
		free(exifbuf);
	}

	m_pScrollBar->SetStart( 0 );
	m_pScrollBar->SetTotal( m_List.Count() );
}

void CIVInfoWnd::ParseBMP( const CString& path )
{
}

void CIVInfoWnd::OnKeypressed( const KeyEvent& evt )
{
	TMsgEntry msg;
	switch( evt.value ) {
	case VK_ESC:
		msg.ssToTaskId = m_nTaskID;
		msg.ulMsgId = MSG_WINDOW;
		msg.usParam1 = m_nWindowID;
		msg.usParam2 = WINDOW_KILL;
		MSG_Send( m_nTaskID, &msg );		
		break;
	case VK_UP:
		ScrollUp();
		break;
	case VK_DOWN:
		ScrollDown();
		break;
	}
}

void CIVInfoWnd::OnMouseEvent( const MouseEvent& evt )
{
	if( evt.x > 0 ) ScrollDown();
	else if( evt.x < 0 ) ScrollUp();
}

void CIVInfoWnd::ScrollDown( void )
{
	if( m_nLine+m_pScrollBar->GetViewCount() > m_List.Count() ) return;

	m_nLine++;
	DrawTagInfo();
}

void CIVInfoWnd::ScrollUp( void )
{
	if( m_nLine == 0 ) return;
	m_nLine--;
	DrawTagInfo();
}

void CIVInfoWnd::DrawTagInfo( void )
{
	switch( m_nMedia ) {
	case MEDIA_JPEG:
		DrawEXIFTag();
		break;
	}
	m_pScrollBar->UpdateScrollBar( m_nLine );
}

void CIVInfoWnd::DrawEXIFTag( void )
{
	int count = m_pScrollBar->GetViewCount();
	struct EXIFInfo* pInfo;
	int offset = MARGIN;
	int hoffset = 120;
	int w, len, w2, x;
	uchar_t wbuf[100];

	if( m_List.Count() == 0 ) return;

	w2 = m_rcText.GetWidth() - hoffset;

	if( count > m_List.Count() ) count = m_List.Count();
	if( m_nLine+count > m_List.Count() ) m_nLine = m_List.Count() - count;

	g_SysPixmap.Fill( m_rcText.x1, m_rcText.y1, m_rcText.GetWidth(), m_rcText.GetHeight(),
		m_nBackgroundColor );
	for( int i=0; i<count; i++ ) {
		pInfo = m_List[m_nLine+i];
		if( !pInfo ) break;
		// draw title
		pT->DrawResText( 
			pInfo->m_nID, 
			m_rcText.x1+MARGIN, 
			m_rcText.y1+offset, 
			_WHITE, 12, DT_TRANSPARENCY );

		// draw value
		w = pT->GetStringWidth( (char*)pInfo->m_szValue, 12, DT_TRANSPARENCY );
		len = pInfo->m_szValue.GetSize();
		w2 = m_rcText.GetWidth() - hoffset;
		str2ustr( wbuf, (char*)pInfo->m_szValue, len );
		if( w+hoffset > m_rcText.GetWidth() ) {
			len = pT->GetBoundaryLength( wbuf, len, 12, DT_TRANSPARENCY, w2 );
		}
		x = m_rcText.x1+hoffset;
		for( int j=0; j<len; j++ ) {
			x += pT->DrawCharacter( 
				wbuf[j], 
				x, 
				m_rcText.y1+offset,
				_WHITE,
				12,
				DT_TRANSPARENCY );
		}

		offset += 12 + SPACING;
	}
	
}

void CIVInfoWnd::OnPaint( const CRect& rc )
{
	CModalWnd::OnPaint( rc );
	DrawTagInfo();
	op_Flush();
}

void CIVInfoWnd::Resize( const CRect& rc, bool fUpdate )
{
	CWindow::Resize( rc, fUpdate );

	int h = 16; // font height + spacing
	int hbutton = 20;
	int margin = MARGIN;

	m_rcText.SetRect( rc.x1+margin, rc.y1+m_nTitleHeight+margin, 
		rc.x2-margin-SCROLLBAR_WIDTH-2, 
		rc.y2-margin-hbutton ); // 20 -button height
	int count = m_rcText.GetHeight() / h;
	m_pScrollBar->SetViewCount( count );
	
	m_pScrollBar->Resize( 
		CRect(rc.GetWidth()-margin-SCROLLBAR_WIDTH,
			margin+m_nTitleHeight,
			rc.GetWidth()-margin+1,
			rc.GetHeight()-margin-hbutton ), false );
}
