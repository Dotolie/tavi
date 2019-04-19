#include <Task/tavi_global.h>
#include <Lib/LibWideString.h>
#include <Lib/LibText.h>

extern CTextEngine_BDF* pT;
int ConvertUnicode( const char* sz, int slen, uchar_t* unicode, int dlen, int charset )
{
	int len;

	if( slen == 0 ) {
		memset( unicode, 0, dlen );
		return 0;
	}
	
	switch( charset ) {
	case NLS_ENGLISH:
		str2ustr( unicode, (char*)sz, slen );
		len = slen;
		break;
	case NLS_KSC5601:
		len = pT->ConvertEUC8StrToUnicode16Str( unicode, (char*)sz );
		break;
	case NLS_JIS:
		str2ustr( unicode, (char*)sz, slen );
		len = slen;
		break;
	}
	len = ustrlen( unicode );

	return len;
}
