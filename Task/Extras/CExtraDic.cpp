//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Extra Dictionary
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/CExtraDic.cpp,v $
// $Revision: 1.9 $
// $Date: 2006/03/17 04:22:18 $
// $Author: akaiaphanu $
//
//*****************************************************************************
// $Log: CExtraDic.cpp,v $
// Revision 1.9  2006/03/17 04:22:18  akaiaphanu
// add usleep for music player
//
// Revision 1.8  2006/03/06 11:31:14  akaiaphanu
// working
//
// Revision 1.7  2006/03/06 11:18:22  akaiaphanu
// working UI
//
// Revision 1.6  2006/03/03 10:02:40  akaiaphanu
// working scrollbar
//
// Revision 1.5  2006/03/03 05:15:12  akaiaphanu
// working text scroll
//
// Revision 1.4  2006/02/28 07:08:20  akaiaphanu
// working touchpad
//
// Revision 1.3  2006/02/28 04:46:11  akaiaphanu
// working kor,eng filename
//
// Revision 1.2  2006/02/27 02:40:17  akaiaphanu
// working
//
// Revision 1.14  2006/02/23 01:56:58  akaiaphanu
// working search function
//
// Revision 1.13  2006/02/22 07:55:52  akaiaphanu
// working text resource , cleaning
//
// Revision 1.12  2006/02/22 04:16:28  akaiaphanu
// working selectbar, itemlist
//
// Revision 1.11  2006/02/21 05:04:46  akaiaphanu
// exception message : case not k-dic
//
// Revision 1.10  2006/02/21 02:04:43  akaiaphanu
// working title, list manager
//
// Revision 1.9  2006/02/20 08:06:47  akaiaphanu
// change to array of structure from double linked list
//
// Revision 1.8  2006/02/20 03:04:30  akaiaphanu
// add function for modekey
//
// Revision 1.7  2006/02/20 01:33:56  akaiaphanu
// add ListManager , BitIO
//
// Revision 1.6  2006/02/14 08:37:16  akaiaphanu
// working bottom guide , scrollbar , box
//
// Revision 1.5  2006/02/11 12:40:14  akaiaphanu
// working dictionary keyboard
//
// Revision 1.4  2006/02/11 09:16:28  akaiaphanu
// working text resource
//
// Revision 1.3  2006/02/11 07:19:11  akaiaphanu
// work for dictionary
//
// Revision 1.2  2006/02/06 04:17:15  akaiaphanu
// add to history function
//
// Revision 1.1  2006/01/27 04:38:14  akaiaphanu
// added dictionary
//
//
//
//*****************************************************************************


#include "CExtraDic.h"

EXTRA_DIC_CONFIG	 _ExDicPro;
extern EXTRA_CONFIG _ExtraProperty;
unsigned long startTime, endTime;

int	strpos(char *buf, char ch)
{
	int pos = 0;
	char *tmp = buf;
	while(*tmp != '\0')
	{
		if(*tmp == ch) //TAB character
			return pos;
		pos++;
		tmp++;
	}
	
	return pos;
}

int	strncpy2(char *dst, char *src, int start, int count)
{
	char *tmp = dst;
	int i;
	
	if((start + count) > strlen(src)) return -1;
		
	src += start;
	for(i = 0; i < count; i++)
		*tmp++ = *src++;
	return 0;
}		

int	strncpy3(char *dst, char *src, int count)
{
	char *tmp = dst, *tmp2 = src;
	int i;
	
	for(i = 0; i < count; i++)
		*tmp++ = *tmp2++;
	return 0;
}		

int	strcmp2(char *s1, char *s2)
{
	int s1len = strlen(s1);
	int s2len = strlen(s2);
	int found = 1, i;
	char	*tmp1 = s1, *tmp2 = s2;

	if(s1len != s2len)
		return 0;
			
	for(i = 0; i < s1len; i++)
	{
		if(*tmp1 != *tmp2)
			found = 0;
		tmp1++;
		tmp2++;
	}
	return found;
}
int Extra_TimeCal(int mode)
{
	struct timeval tv_cur = { 0, 0 };
	struct timeval tv_last;

	tv_cur = tv_last;
	
	gettimeofday( &tv_cur, NULL );
	
	switch(mode)
	{
		case 0 : /* start */
			startTime = tv_cur.tv_sec;
			break;
		case 1 : /* end */
			endTime = tv_cur.tv_sec;
			fprintf(stderr, "Time = %d Sec\n", endTime - startTime);
			break;
		default :
			break;
	}

	return 0;
}

int	fatfs_fgets(RMuint8 *buf, int size, RMuint32 fd)
{
	RMuint8 *tmp = buf;
	RMuint32	currpos = 0, nread = 0, i;
	
	fatfs_ftell(fd, &currpos); /* 현재 위치  저장 */
	
	fatfs_fread(fd, buf, size, &nread);
	
	if(nread == 0) 
		return 0;

	for(i = 0; i < nread; i++)
	{
		if(*(buf + i) == 0x0a) 
		{
			fatfs_fseek(fd, currpos + i + 1, SEEK_SET);
			*(buf + i) = 0;
			return 1;
		}
	}
	return 1;
}

u32	fgets_mem(u8	 *output, u32 lpos, u8 *input)
{
	u32	crlf = 0, totalline = 0;
	u8 *backup = input;
	u8 *ret = output;
	while(*input != 0x00)
		if(*input++ == 0x0A) totalline++;

	//printf("totalline = %d, lpos[%d]\n", totalline, lpos);
	if(totalline < lpos) return 0;

	input = backup;
	
	if(lpos == 1) {
		while(*input != 0x00 && *input != 0x0A)
			*output++ = *input++;
	}
	else {
		crlf = 1;
		while(*input != 0x00)
		{
			if(*input++ == 0x0a)
				crlf++;
			if(crlf == lpos) break;
		}
		while(*input != 0x00 && *input != 0x0A)
			*output++ = *input++;
	}
	output = ret;
	return 1;
}

int ExDic_Init(void)
{
	_ExDicPro.m_fExit = 				false;
	_ExDicPro.m_Resume = 			false;
	_ExDicPro.m_ExitState = 			0;
	_ExDicPro.m_found = 				false;
	_ExDicPro.m_isExist_DicData = 		false;
	_ExDicPro.m_isLoad_DicData = 		false;
	
	_ExDicPro.m_go_search = 			false;
	_ExDicPro.m_down_pressed = 		false;
	_ExDicPro.m_dic_cur_state = 		DIC_STATE_NO_DATA;
	_ExDicPro.m_dic_pre_state = 		DIC_STATE_KEYBOARD_KOR;
	
	_ExDicPro.m_text_display = 		NULL;
	
	_ExDicPro.m_searched_text[0] = 	'\0';
	memset(_ExDicPro.m_text_line_index, 0x00, 100);
	memset(_ExDicPro.m_last_filename, 0x00, 512);
	memset(_ExDicPro.m_word, 0x00, 1024);
	
	_ExDicPro.m_current_text_index = 	0;
	_ExDicPro.m_total_text_lines = 		0;
	_ExDicPro.m_current_text_line = 	0;
	_ExDicPro.m_visible_lines = 		DIC_OUTWIN_VISIBLE_LINES;
	_ExDicPro.m_line_length = 			DIC_OUTWIN_VISIBLE_LINE_LEN;
	
	_ExDicPro.m_total_page = 					0;
	_ExDicPro.m_current_page = 				0;
	
	_ExDicPro.m_neighbor_word_list_idx=		0;
	_ExDicPro.m_neighbor_word_list_preidx=		0;
	_ExDicPro.m_neighbor_word_list_nextidx=	0;
								
	_ExDicPro.m_pScrollbar3 = 		new CScrollBar3( 312,  47, 165,  1, 1, 1 );
	_ExDicPro.m_pDicKeyboard = 		new DicKeyboard(9,34,302,194);
	_ExDicPro.m_pHistory = 			new HistoryManager(9,34,302,194);
	_ExDicPro.m_pDM = 				new DataManager(9,34,302,194);
	_ExDicPro.m_pLM = 				new ListManager(9,34,302,194);
	_ExDicPro.m_pMBx_NoDic = 		new CMsgBox( ML_INFORMATION, _ORANGE, ML_NO_DICDATA, 244, 123, 0, DL_BTN_OK_ONLY);
	_ExDicPro.m_pMBx_NoSelectDic = 	new CMsgBox( ML_INFORMATION, _ORANGE, ML_NO_SELECT_DIC, 244, 123, 0, DL_BTN_OK_ONLY);
	_ExDicPro.m_pMBx_NoKDicType =	new CMsgBox( ML_INFORMATION, _ORANGE, ML_NO_KDIC_TYPE, 244, 123, 0, DL_BTN_OK_ONLY);
	_ExDicPro.m_text_display = 		(uchar_t*) malloc(sizeof(uchar_t)*50);

	_ExDicPro.m_sx_scroll = 				0;
	_ExDicPro.m_loadable_word_count =		100;

	//fprintf(stderr,"ExDic : Init........................................................[ OK ]\n");
}

int ExDic_Free_WordList(void)
{
	if(_ExDicPro.m_neighbor_word_list_idx >0 )
	{
		//fprintf(stderr,"FREE : m_neighbor_word_list\n");
		while(_ExDicPro.m_neighbor_word_list_idx--)
		{
			free(_ExDicPro.m_neighbor_word_list[_ExDicPro.m_neighbor_word_list_idx]);
		}
		_ExDicPro.m_neighbor_word_list_idx = 0;
	}
	
	if(_ExDicPro.m_neighbor_word_list_preidx > 0)
	{
		//fprintf(stderr,"FREE : m_neighbor_word_prelist\n");
		while(_ExDicPro.m_neighbor_word_list_preidx--)
		{
			free(_ExDicPro.m_neighbor_word_prelist[_ExDicPro.m_neighbor_word_list_preidx]);
		}
		_ExDicPro.m_neighbor_word_list_preidx = 0;
	}

	if(_ExDicPro.m_neighbor_word_list_nextidx > 0)
	{
		//fprintf(stderr,"FREE : m_neighbor_word_nextlist\n");
		while(_ExDicPro.m_neighbor_word_list_nextidx--)
		{
			free(_ExDicPro.m_neighbor_word_nextlist[_ExDicPro.m_neighbor_word_list_nextidx]);
		}
		_ExDicPro.m_neighbor_word_list_nextidx = 0;
	}

	return 0;

}

int ExDic_Free(void)
{
	if(_ExDicPro.m_text_display)	/* text */
	{
		//fprintf(stderr, "FREE :1: TEXT_DISPLAY\n");
		free(_ExDicPro.m_text_display);
	}
	if(_ExDicPro.m_pDicKeyboard)	/* dictionary keyboard */
	{
		//fprintf(stderr, "DELETE :2: KEYBOARD\n");
		delete(_ExDicPro.m_pDicKeyboard);
	}
	if(_ExDicPro.m_pScrollbar3)	/* scrollbar */
	{
		//fprintf(stderr, "DELETE :3: SCROLLBAR\n");
		delete(_ExDicPro.m_pScrollbar3);
	}
	if(_ExDicPro.m_pMBx_NoDic)	/* message box no dic */
	{
		//fprintf(stderr, "DELETE :4: MSG BOX NO DIC\n");
		delete(_ExDicPro.m_pMBx_NoDic);
	}
	if(_ExDicPro.m_pHistory)	/* history manager */
	{
		//fprintf(stderr, "DELETE :5: HISTORY MANAGER\n");
		delete(_ExDicPro.m_pHistory);
	}
	if(_ExDicPro.m_pDM)	/* data manager */
	{
		//fprintf(stderr, "DELETE :6: DATA MANAGER\n");
		delete(_ExDicPro.m_pDM);
	}
	if(_ExDicPro.m_pLM)	/* list manager */
	{
		//fprintf(stderr, "DELETE :7: LIST MANAGER\n");
		delete(_ExDicPro.m_pLM);
	}
	if(_ExDicPro.m_pMBx_NoSelectDic)	/* message box no select dic */
	{
		//fprintf(stderr, "DELETE :8: MSG BOX NO SELECT DIC\n");
		delete(_ExDicPro.m_pMBx_NoSelectDic);
	}
	if(_ExDicPro.tDicI)	/* tDicI */
	{
		//fprintf(stderr, "DELETE :9: DIC INFO STRUCTURE \n");
		free(_ExDicPro.tDicI);
	}
	if(_ExDicPro.m_pMBx_NoKDicType)
	{
		//fprintf(stderr, "DELETE :10: NO KDIC TYPE \n");
		delete(_ExDicPro.m_pMBx_NoKDicType);
	}
	ExDic_Free_WordList();

	return 0;
	
}

int ExDic_MainScreen(void)
{
	pMpegDecoder->TAVI_InitSystemPalette();
	TAVI_LoadPaletteTransparencyData(TRUE);
	showbackground(9);
	op_ClearAreaOSD( 0, 0, 320, 240 );

	/* Top Panel */
	g_TopPanel->SetTitle(ML_DIC);
	g_TopPanel->SetLogo(CTopPanel::LOGO_ID_ETC);
	g_TopPanel->SetBatteryLevel( (CTopPanel::BatteryLevel)CalcBatteryLevel() );
	g_TopPanel->SetVisible( true );
	g_TopPanel->Show();
	//fprintf(stderr,"ExDic : Main Screen\n");
	return 0;

}

int ExDic_FinishScreen(void)
{
	ClearOsd();
	//fprintf(stderr,"ExDic : Finish Screen\n");
	return 0;
}

int ExDic_Draw_History_Box(void)
{
	//fprintf(stderr,"ExDic : ExDic_Draw_History_Box\n");
	_ExDicPro.m_pHistory->Show();
	return 0;
}
int ExDic_Draw_DM_Box(void)
{
	//fprintf(stderr,"ExDic : ExDic_Draw_DM_Box\n");
	_ExDicPro.m_pDM->LoadFromFile();
	_ExDicPro.m_pDM->Show();
	return 0;
}
int ExDic_Draw_LM_Box(void)
{
	//fprintf(stderr,"ExDic : ExDic_Draw_LM_Box\n");
	_ExDicPro.m_pLM->Show();
	return 0;
}
int ExDic_Load_WNameToLM (void)
{
	//fprintf(stderr,"ExDic : ExDic_Load_WNameToLM\n");
	//printf("[1pre][%s]\n", _ExDicPro.m_neighbor_word_prelist[1]);
	//printf("[1nxt][%s]\n", _ExDicPro.m_neighbor_word_nextlist[1]);
	
	_ExDicPro.m_pLM->Init_Member();
	int i=0;
	for(i = 0; i < _ExDicPro.m_neighbor_word_list_preidx; i++)
	{
		_ExDicPro.m_pLM->Append(_ExDicPro.m_neighbor_word_prelist[i]);
		//fprintf(stderr,"ExDic_Load_WNameToLM : [%s]\n",_ExDicPro.m_neighbor_word_prelist[i]);
	}
	//fprintf(stderr,"=========================================\n");
	for(i = 0; i < _ExDicPro.m_neighbor_word_list_nextidx; i++)
	{
		_ExDicPro.m_pLM->Append(_ExDicPro.m_neighbor_word_nextlist[i]);
		//fprintf(stderr,"ExDic_Load_WNameToLM : [%s]\n",_ExDicPro.m_neighbor_word_nextlist[i]);
	}
	return 0;
}
int ExDic_Load_OneSection(char* file_path, int which_section)
{
	u32 				i, nread = 0, nwrite = 0;
	u8	*			key;
	u8 				output[8192];
	RMuint32 		fd = 0, fd_tmp = 0;
	BIT_FILE	*		bit_file;
	FATFS_ERROR 	err;
	char	*			keyname, *keyData, cpos, lpos = 1;

	//fprintf(stderr,"ExDic : ExDic_Load_OneSection\n");

	if( (err = fatfs_wfopen ((RMuint16*)file_path, _O_RDONLY, &fd)) > 0 )
	{	
		fprintf(stderr, "ExDic_Load_OneSection : fopen : error\n");
		fatfs_fclose(fd);
		return -1;
	}
	
	fatfs_fseek(fd, _ExDicPro.tDicI[which_section].dwIdxpos, SEEK_SET);
	key = (u8 *)malloc(_ExDicPro.tDicI[which_section].size);
	
	if( (err = fatfs_fread( fd, (RMuint8*)key, _ExDicPro.tDicI[which_section].size, &nread )) > 0 )
	{	
		fprintf(stderr, "ExDic_Load_OneSection : fread : error\n");
		fatfs_fclose(fd);
		return -1;
	}
	
	bit_file = InitBitFile(bit_file);
	memset(output, 0, 8192);
	memcpy(bit_file->buf, key, _ExDicPro.tDicI[which_section].size);
	bit_file->size = _ExDicPro.tDicI[which_section].size;
	ExpandMem(bit_file, output);

	free(bit_file);
	free(key);

	key = (u8 *)malloc(8192);
	memset(key, 0, 8192);
	lpos = 1;
	while(fgets_mem(key, lpos++, output) != 0)
	{
	
		if(strpos((char *)key, 0x09) != 0)
		{
			cpos = strpos((char *)key, 0x09);
			_ExDicPro.m_neighbor_word_list
				[_ExDicPro.m_neighbor_word_list_idx] = (char *)malloc(cpos + 2);
			memset(_ExDicPro.m_neighbor_word_list[_ExDicPro.m_neighbor_word_list_idx],
				0x00, cpos + 2);
			strncpy3(_ExDicPro.m_neighbor_word_list[_ExDicPro.m_neighbor_word_list_idx++],
				(char *)key, cpos);	
		}
		if(_ExDicPro.m_neighbor_word_list_idx>_ExDicPro.m_loadable_word_count )
		{
			free(key);
			fatfs_fclose(fd);
			return -1;
		}
		memset(key, 0, 8192);
	}
	free(key);
	fatfs_fclose(fd);
	return which_section;
}

int ExDic_Re_Search(void)
{
	int 		section_cur_idx=0, section_total_idx=0;
	int		rank=0;
	char 	file_path[512];
	
	memset(file_path, 0, 512);
	memcpy(file_path,_ExDicPro.m_last_filename, 512);
	
	if(_ExDicPro.m_search_case == 0)
	{
		//fprintf(stderr,"ExDic_Re_Search : Case 0 \n");
		for(section_cur_idx=0; section_cur_idx< _ExDicPro.reclength; section_cur_idx++)
		{
			if(ExDic_Load_OneSection(file_path, section_cur_idx) == -1)
				break;
		}
		
	}
	else if(_ExDicPro.m_search_case == 1)
	{
		//fprintf(stderr,"ExDic_Re_Search : Case 1 \n");
		for(section_cur_idx=_ExDicPro.m_tDicI_Cur_Idx; section_cur_idx< _ExDicPro.reclength; section_cur_idx++)
		{
			if((_ExDicPro.m_tDicI_Cur_Idx=ExDic_Load_OneSection(file_path, section_cur_idx)) == -1)
				break;
		}	
	}
	else if(_ExDicPro.m_search_case == 2)
	{
		fprintf(stderr,"ExDic_Re_Search : Case 2 \n");
//		for(section_cur_idx=_ExDicPro.m_tDicI_Cur_Idx; section_cur_idx< _ExDicPro.reclength; section_cur_idx++)
//		{
//			if((_ExDicPro.m_tDicI_Cur_Idx=ExDic_Load_OneSection(file_path, section_cur_idx)) == -1)
//				break;
//		}
	}
		
	
	return 0;
}

int ExDic_IsSearchMore(void)
{
	int 		section_cur_idx=0, section_total_idx=0;
	int		rank=0;
	char 	file_path[512];
	
	memset(file_path, 0, 512);
	memcpy(file_path,_ExDicPro.m_last_filename, 512);
	
	if(_ExDicPro.m_search_case == DIC_SRCH_CASE_IN_SECTION)
	{
		rank = _ExDicPro.m_pLM->Set_CloseByWord(_ExDicPro.m_word);
		if(rank <= 30)
		{
			fprintf(stderr,"ExDic_IsSearchMore : Case 1 : Rank=[%d]\n",rank);
		}
		else if(rank >= 70)
		{
			//fprintf(stderr,"ExDic_IsSearchMore : Case 1 : Rank=[%d]\n",rank);
			_ExDicPro.m_loadable_word_count = 150;
			for(section_cur_idx=_ExDicPro.m_tDicI_Cur_Idx+1; section_cur_idx< _ExDicPro.reclength; section_cur_idx++)
			{
				if((_ExDicPro.m_tDicI_Cur_Idx=ExDic_Load_OneSection(file_path, section_cur_idx)) == -1)
					break;
			}
		}
		else
			fprintf(stderr,"ExDic_IsSearchMore : Can not need to search\n");	
	}
	
	return 0;
}

int ExDic_SrchFailCase_Proc(int cur_mode, int pre_mode)
{
	ExDic_Re_Search();

	switch(_ExDicPro.m_search_case)
	{
		case DIC_SRCH_CASE_OUT_SECTION :
			_ExDicPro.m_pLM->Load_List(_ExDicPro.m_neighbor_word_list,
				_ExDicPro.m_neighbor_word_list_idx);
			break;
			
		case DIC_SRCH_CASE_IN_SECTION :	
			_ExDicPro.m_pLM->Load_List(_ExDicPro.m_neighbor_word_list,
				_ExDicPro.m_neighbor_word_list_idx);
			ExDic_IsSearchMore();
			_ExDicPro.m_pLM->Load_List(_ExDicPro.m_neighbor_word_list,
				_ExDicPro.m_neighbor_word_list_idx);
			_ExDicPro.m_pLM->Set_CloseByWord(_ExDicPro.m_word);
			break;
			
		case DIC_SRCH_CASE_INTER_SECTION :
			ExDic_Load_WNameToLM();
			_ExDicPro.m_pLM->Set_CloseByWord(_ExDicPro.m_word);
			break;
			
		default : break;
	}
	
	ExDic_Draw_LM_Box();
	ExDic_Draw_Text();
	_ExDicPro.m_dic_cur_state = cur_mode;
	_ExDicPro.m_dic_pre_state = pre_mode;


}

int ExDic_Search_Word(void)
{
	RMuint32 		fd = 0, fd_tmp = 0;
	BIT_FILE	*		bit_file;
	FATFS_ERROR 	err;
	char	*			keyname;
	char *			keyData;
	char 			cpos, lpos = 1;
	char 			file_path[512];
	u8*				key;
	u8 				output[8192];
	u32 				i, nread = 0, nwrite = 0;
	int 				section_cur_idx=0, section_total_idx=0;
	int				rank=0;

	//fprintf(stderr,"ExDic : ExDic_Search_Word\n");
	ExDic_Free_WordList();
	
	_ExDicPro.m_search_case = 0;
	_ExDicPro.m_found = false;	
	
	/* Open _ExDicPro.m_last_filename */
	//fprintf(stderr,"\nlast_filename : \n");
	//for(i=0; i<50;i++)
	//	fprintf(stderr,"[0x%x]",_ExDicPro.m_last_filename[i]);
	memset(file_path, 0, 512);
	memcpy(file_path,_ExDicPro.m_last_filename, 512);
	
	if( (err = fatfs_wfopen ((RMuint16*)file_path, _O_RDONLY, &fd)) > 0 )
	{	
		fprintf(stderr, "\nExDic_Search_Word : fopen0 : error\n");
		fatfs_fclose(fd);
		return -1;
	}
	
	/* Search loop */
	//fprintf(stderr,"Start search....word=[%s], reclength=[%d]\n", _ExDicPro.m_word, _ExDicPro.reclength);
	
	for(i = 0; i < _ExDicPro.reclength; i++)
	{
		if( strcmp(_ExDicPro.m_word, _ExDicPro.tDicI[i].startchar) >= 0 
			&& (strcmp(_ExDicPro.m_word, _ExDicPro.tDicI[i].endchar) <= 0 ))
		{
			_ExDicPro.m_search_case = 1;
			//fprintf(stderr,"search_case = 1 : startchar = %s, endchar = %s\n", 
			//	_ExDicPro.tDicI[i].startchar, _ExDicPro.tDicI[i].endchar);
			
			fatfs_fseek(fd, _ExDicPro.tDicI[i].dwIdxpos, SEEK_SET);
			key = (u8 *)malloc(_ExDicPro.tDicI[i].size);
			
			if( (err = fatfs_fread( fd, (RMuint8*)key, _ExDicPro.tDicI[i].size, &nread )) > 0 )
			{	
				fprintf(stderr, "ExDic_Search_Word : fread : error\n");
				fatfs_fclose(fd);
				return -1;
			}
			
			bit_file = InitBitFile(bit_file);
			memset(output, 0, 8192);
			memcpy(bit_file->buf, key, _ExDicPro.tDicI[i].size);
			bit_file->size = _ExDicPro.tDicI[i].size;
			ExpandMem(bit_file, output);

			free(bit_file);
			free(key);
			fatfs_fclose(fd);
			key = (u8 *)malloc(8192);
			memset(key, 0, 8192);
			lpos = 1;
			while(fgets_mem(key, lpos++, output) != 0)
			{
				//printf("%s\n", key);
				if(strpos((char *)key, 0x09) != 0)
				{
					cpos = strpos((char *)key, 0x09);
					keyname = (char *)malloc(cpos + 2);
					memset(keyname, 0, cpos + 2);
					strncpy3(keyname, (char *)key, cpos);

					//fprintf(stderr,"Keyname = %s\n", keyname);
					//_ExDicPro.m_neighbor_word_list[_ExDicPro.m_neighbor_word_list_idx++] = keyname;
					_ExDicPro.m_tDicI_Cur_Idx = i;
					
					if(strcmp(_ExDicPro.m_word, keyname) == 0)
					{
						keyData = (char *)malloc(strlen((char *)key) - cpos);
						memset(keyData, 0, strlen((char *)key) - cpos);
						strncpy2(keyData, (char *)key, cpos + 1, strlen((char *)key) - cpos - 1);

						//fprintf(stderr,"========== Searched Text =============\n");
						//fprintf(stderr,"%s\n", keyData);
						//fprintf(stderr,"=================================\n");

						memset(_ExDicPro.m_searched_text,0,8192);
						sprintf(_ExDicPro.m_searched_text,"%s",keyData);
						_ExDicPro.m_found = true;
						free(keyData);
						free(keyname);
						break;
					}
					free(keyname);
				}
				memset(key, 0, 8192);
			}
			free(key);
			break;
		}
		else if(	i < (_ExDicPro.reclength - 1)
				&& strcmp(_ExDicPro.m_word, _ExDicPro.tDicI[i].endchar) > 0 
				&& strcmp(_ExDicPro.m_word, _ExDicPro.tDicI[i+1].startchar) < 0)
		{

			_ExDicPro.m_search_case = 2;

			//fprintf(stderr,"search_case = 2 : endchar = %s, startchar = %s\n", 
			//	_ExDicPro.tDicI[i].endchar, _ExDicPro.tDicI[i+1].startchar);
			
			fatfs_fseek(fd, _ExDicPro.tDicI[i].dwIdxpos, SEEK_SET);
			key = (u8 *)malloc(_ExDicPro.tDicI[i].size);
			
			if( (err = fatfs_fread( fd, (RMuint8*)key, _ExDicPro.tDicI[i].size, &nread )) > 0 )
			{	
				fprintf(stderr, "ExDic_Search_Word : fread : error\n");
				fatfs_fclose(fd);
				return -1;
			}
			
			bit_file = InitBitFile(bit_file);
			memset(output, 0, 8192);
			memcpy(bit_file->buf, key, _ExDicPro.tDicI[i].size);
			bit_file->size = _ExDicPro.tDicI[i].size;
			ExpandMem(bit_file, output);

			free(bit_file);
			free(key);
		
			key = (u8 *)malloc(8192);
			memset(key, 0, 8192);
			lpos = 1;
			while(fgets_mem(key, lpos++, output) != 0)
			{
				if(strpos((char *)key, 0x09) != 0)
				{
					cpos = strpos((char *)key, 0x09);
					_ExDicPro.m_neighbor_word_prelist[_ExDicPro.m_neighbor_word_list_preidx] = (char *)malloc(cpos + 2);
					memset(_ExDicPro.m_neighbor_word_prelist[_ExDicPro.m_neighbor_word_list_preidx], 0, cpos + 2);
					strncpy3(_ExDicPro.m_neighbor_word_prelist[_ExDicPro.m_neighbor_word_list_preidx++], (char *)key, cpos);
					_ExDicPro.m_tDicI_Cur_Idx = i;	
					//fprintf(stderr,"Keyname = %s[%d]\n", 
					//	_ExDicPro.m_neighbor_word_prelist[_ExDicPro.m_neighbor_word_list_preidx],
					//	_ExDicPro.m_neighbor_word_list_preidx);	
				}
				memset(key, 0, 8192);
			}
			free(key);

			for(i += 1; i < _ExDicPro.reclength; i++)
			{
				fatfs_fseek(fd, _ExDicPro.tDicI[i].dwIdxpos, SEEK_SET);
				key = (u8 *)malloc(_ExDicPro.tDicI[i].size);
				
				if( (err = fatfs_fread( fd, (RMuint8*)key, _ExDicPro.tDicI[i].size, &nread )) > 0 )
				{	
					fprintf(stderr, "ExDic_Search_Word : fread : error\n");
					fatfs_fclose(fd);
					return -1;
				}
				
				bit_file = InitBitFile(bit_file);
				memset(output, 0, 8192);
				memcpy(bit_file->buf, key, _ExDicPro.tDicI[i].size);
				bit_file->size = _ExDicPro.tDicI[i].size;
				ExpandMem(bit_file, output);
				free(bit_file);
				free(key);
				
				key = (u8 *)malloc(8192);
				memset(key, 0, 8192);
				lpos = 1;
				while(fgets_mem(key, lpos++, output) != 0)
				{
					
					if(strpos((char *)key, 0x09) != 0)
					{
						cpos = strpos((char *)key, 0x09);
						_ExDicPro.m_neighbor_word_nextlist[_ExDicPro.m_neighbor_word_list_nextidx] = (char *)malloc(cpos + 2);
						memset(_ExDicPro.m_neighbor_word_nextlist[_ExDicPro.m_neighbor_word_list_nextidx], 0, cpos + 2);
						strncpy3(_ExDicPro.m_neighbor_word_nextlist[_ExDicPro.m_neighbor_word_list_nextidx++], (char *)key, cpos);
						_ExDicPro.m_tDicI_Cur_Idx = i;		

						//fprintf(stderr,"Keyname = %s[%d]\n",
						//	_ExDicPro.m_neighbor_word_nextlist[_ExDicPro.m_neighbor_word_list_nextidx],
						//	_ExDicPro.m_neighbor_word_list_nextidx);		
					}
					memset(key, 0, 8192);
				}
				free(key);
				
				if(_ExDicPro.m_neighbor_word_list_nextidx > _ExDicPro.m_loadable_word_count)
					break;
					
			}
			fatfs_fclose(fd);
			break;
		}
	}

	if(!_ExDicPro.m_found)
	{
		if(_ExDicPro.m_search_case == 0)
		{	fatfs_fclose(fd); }
		fprintf(stderr,"ExDic_Search_Word : Failed\n");
	}
	else
	{
		fprintf(stderr,"ExDic_Search_Word : [%s]\n",_ExDicPro.m_word);
	}

	return 1;
	
}

int ExDic_Draw_Scrollbar(RMuint32 total, RMuint32 start, RMuint32 maxvisible )
{
	if(((start+maxvisible)-total) <10 && ((start+maxvisible)-total) >= 0)
	{
		_ExDicPro.m_pScrollbar3->ReDraw(total, total, maxvisible);
	}
	else
	{
		_ExDicPro.m_pScrollbar3->ReDraw(total, start, maxvisible);
	//	fprintf(stderr,"total = %d\t start = %d\t maxvisible = %d\n",total, start, maxvisible);
	}
	return 0;

}

int ExDic_Chk_Text2(void)
{
	int index=0;
	int x=10, y=30;
	int i = 0;
	int able_line_length = _ExDicPro.m_line_length;
	int line_length_count =0;
	
	_ExDicPro.m_total_text_lines = 0;
	
	while(i <1024)		/* init : m_line_position_info[100] */
	{
		 _ExDicPro.m_line_position_info[i++] = 0x00;
		// fprintf(stderr,"[%d]%d ", j-1, _ExDicPro.m_line_position_info[j-1]);
	}
	
	while(_ExDicPro.m_searched_text[index] != '\0')
	{
		/* case  one byte */
		if(_ExDicPro.m_searched_text[index] < 0x80)
		{
			line_length_count++;
			_ExDicPro.m_couple_info[index] = 0x00;
			
			if(((line_length_count%able_line_length) == 0) && (line_length_count != 0))
			{
				_ExDicPro.m_line_position_info[++_ExDicPro.m_total_text_lines] = index;
				line_length_count = 0;
			}
			else if((_ExDicPro.m_searched_text[index] == 0x5C) && (_ExDicPro.m_searched_text[index+1] == 0x6E))
			{
				_ExDicPro.m_searched_text[index] = ' ';
				_ExDicPro.m_searched_text[++index] = ' ';
				_ExDicPro.m_line_position_info[++_ExDicPro.m_total_text_lines] = index;
				line_length_count = 0; 
			}
		}
		/* case  two bytes */
		else
		{
			line_length_count++;
			if(_ExDicPro.m_couple_info[index-1] == 0x00)
			{
				_ExDicPro.m_couple_info[index] = 0x01;
			}
			else if(_ExDicPro.m_couple_info[index-1] == 0x01)
			{
				_ExDicPro.m_couple_info[index] = 0x02;
			}
			else if(_ExDicPro.m_couple_info[index-1] == 0x02)
			{
				_ExDicPro.m_couple_info[index] = 0x01;
			}
			
			if(((line_length_count%able_line_length) == 0) && (line_length_count != 0))
			{
				line_length_count = 0;
				if(_ExDicPro.m_couple_info[index] == 0x02)
				{
					_ExDicPro.m_line_position_info[++_ExDicPro.m_total_text_lines] = index-1;	
				}
				else
				{
					_ExDicPro.m_line_position_info[++_ExDicPro.m_total_text_lines] = index;
				}
			}
		}

		index++;

	}
	
	_ExDicPro.m_line_position_info[++_ExDicPro.m_total_text_lines] = index;
	_ExDicPro.m_current_page = 0;
	
	i = _ExDicPro.m_total_text_lines / 10;
	
	if( _ExDicPro.m_total_text_lines <= 10)
		_ExDicPro.m_total_page = 0;
	else if( _ExDicPro.m_total_text_lines > 10)
	{
		if((_ExDicPro.m_total_text_lines % 10) == 0)
			_ExDicPro.m_total_page = i-1;
		else
			_ExDicPro.m_total_page = i;
	}
	
	
#if 0	
	fprintf(stderr,"m_total_text_lines =%d lines\n",_ExDicPro.m_total_text_lines);
	fprintf(stderr,"m_total_page        =%d pages\n",_ExDicPro.m_total_page);
	fprintf(stderr,"line_position_info[]\n");
	for(i=0; i <=_ExDicPro.m_total_text_lines; i++)
	{
		fprintf(stderr,"[%3d]=%4d ",  i,_ExDicPro.m_line_position_info[i]);
		if((i%10) == 0)
			fprintf(stderr,"\n");
	}

	fprintf(stderr,"\nm_searched_text = \n");
	for(i=0; i <100; i++)
	{
		fprintf(stderr,"[0x%x] ", _ExDicPro.m_searched_text[i]);
		if((i%10) == 0)
			fprintf(stderr,"\n");
	}
#endif
	return 0;
	

}

int ExDic_Draw_ScrollTxt(int title_width)
{
	int end_pos =0, start_pos=0;
	
	if( (end_pos=pT->GetEndXPosOfTxtBuf()) > (title_width-(start_pos=pT->GetStartXPosOfTxtBuf())) )
	{
		int lenth = pT->GetEndXPosOfTxtBuf() + 20;

		if( _ExDicPro.m_sx_scroll == 0 ||_ExDicPro.m_sx_scroll >= lenth )
		{
			_ExDicPro.m_sx_scroll = 0;
			pT->DrawScrollTxt(_ExDicPro.m_sx_scroll, title_width);
			usleep(10000000); /* 1,000,000 usec = 1sec ?? */
		}
		else
		{
			pT->DrawScrollTxt(_ExDicPro.m_sx_scroll, title_width);
		}
		
		_ExDicPro.m_sx_scroll++;	
	}	

	return 1;
}

int ExDic_Draw_TextTitle(void)
{
	int x=10, y=30;
	char word[256];
	char page[15];
	int str_lenth=0;
	
	memset(word, 0, 256);
	memset(page, 0, 15);

	/* draw word & page info */
	sprintf(word,"%s",_ExDicPro.m_word);
	sprintf(page,"[ %2d/%2d ]",_ExDicPro.m_current_page+1, _ExDicPro.m_total_page+1);
	pT->DrawEUC8Text( "[",
						x,
						y,
						_WHITE,
						_WHITE,
						12,
						DT_TRANSPARENCY );
	//pT->DrawScrollTxt(0, DIC_OUTWIN_TITLE_SCROLL_W);
	str_lenth=pT->DrawEUC8Text( word,
						x+8,
						y,
						_WHITE,
						DIC_OUTWIN_TITLE_COLOR,
						12,
						DT_TRANSPARENCY|DT_SCROLL_TITLE);
	if(str_lenth > 210)
	{
		pT->DrawEUC8Text( "]",
							x+8+220+8,
							y,
							_WHITE,
							_WHITE,
							12,
							DT_TRANSPARENCY );
	}
	else
	{
		pT->DrawEUC8Text( "]",
							x+8+str_lenth+4-20,
							y,
							_WHITE,
							_WHITE,
							12,
							DT_TRANSPARENCY );
	}
	pT->DrawEUC8Text( page,
						320-55,
						y,
						_WHITE,
						_WHITE,
						12,
						DT_TRANSPARENCY );

	return 1;
}

int ExDic_Draw_Text(void)
{
	int x=10, y=30;
	int i = 0, j = 0, line=0;
	int start_position = 0;
	char tmp[80];

	if(!_ExDicPro.m_found)
	{
		_ExDicPro.m_total_text_lines = 0;
		_ExDicPro.m_current_text_line = 0;
		return -1;
	}

	ExDic_Draw_TextTitle();
	memset(tmp, 0, 80);
	y += 22;
	
	if(_ExDicPro.m_current_page == 0)
	{ 	
		if(_ExDicPro.m_total_text_lines > _ExDicPro.m_visible_lines)
		{
			for(i = 0; i< _ExDicPro.m_visible_lines; i++)
			{	
				strncpy(tmp, _ExDicPro.m_searched_text+
					_ExDicPro.m_line_position_info[_ExDicPro.m_current_text_line],
					_ExDicPro.m_line_position_info[_ExDicPro.m_current_text_line+1] 
					-_ExDicPro.m_line_position_info[_ExDicPro.m_current_text_line] );
				pT->DrawEUC8Text( tmp, x, y, _WHITE, _WHITE,12, DT_TRANSPARENCY);
				y += 16;
				_ExDicPro.m_current_text_line++;
				memset(tmp,0, 80);
			}
		}
		else
		{ 
			for(i = 0; i< _ExDicPro.m_total_text_lines; i++)
			{	
				strncpy(tmp, _ExDicPro.m_searched_text+
					_ExDicPro.m_line_position_info[_ExDicPro.m_current_text_line],
					_ExDicPro.m_line_position_info[_ExDicPro.m_current_text_line+1] 
					-_ExDicPro.m_line_position_info[_ExDicPro.m_current_text_line] );
				pT->DrawEUC8Text( tmp, x, y, _WHITE, _WHITE,12, DT_TRANSPARENCY);
				y += 16;
				_ExDicPro.m_current_text_line++;
				memset(tmp,0, 80);
			}
		}
		
	}
	else
	{
		_ExDicPro.m_current_text_line = (_ExDicPro.m_visible_lines * _ExDicPro.m_current_page);
		if(_ExDicPro.m_total_page > _ExDicPro.m_current_page)
		{
			for(i = 0; i< _ExDicPro.m_visible_lines; i++)
			{	
				strncpy(tmp, _ExDicPro.m_searched_text+
					_ExDicPro.m_line_position_info[_ExDicPro.m_current_text_line],
					_ExDicPro.m_line_position_info[_ExDicPro.m_current_text_line+1] 
					-_ExDicPro.m_line_position_info[_ExDicPro.m_current_text_line] );
				pT->DrawEUC8Text( tmp, x, y, _WHITE, _WHITE,12, DT_TRANSPARENCY);
				y += 16;
				_ExDicPro.m_current_text_line++;
				memset(tmp,0, 80);
			}
		}
		else
		{
			line = _ExDicPro.m_total_text_lines - _ExDicPro.m_current_text_line;
			for(i = 0; i< line; i++)
			{	
				strncpy(tmp, _ExDicPro.m_searched_text+
					_ExDicPro.m_line_position_info[_ExDicPro.m_current_text_line],
					_ExDicPro.m_line_position_info[_ExDicPro.m_current_text_line+1] 
					-_ExDicPro.m_line_position_info[_ExDicPro.m_current_text_line] );
				pT->DrawEUC8Text( tmp, x, y, _WHITE, _WHITE,12, DT_TRANSPARENCY);
				y += 16;
				_ExDicPro.m_current_text_line++;
				memset(tmp,0, 80);
			}

		}
	}
	pMpegDecoder->RefreshOsd();
	return 1;

}
void ExDic_DrawBottomGuidePannel( void)
{

	int tw1, tw2, tw3, x1, x2, x3, x4;
	int AddSpace;
	int img_w = 24;
	int img_h = 14;
	int img_space = 6;
	int item_space = 11;
	int shift_val = 9;
	int btn_y_pos = 223;
	
		
	DrawBottomGuideBGPannel( SKIP_OSD_REFRESH );

	tw1 = pT->GetStringWidth( (U32)ML_HOME, 12, 0 );
	tw2 = pT->GetStringWidth( (U32)ML_INPUT, 12, 0 );
	tw3 = pT->GetStringWidth( (U32)ML_LOAD, 12, 0 );
	
	x1 = shift_val + img_w + img_space;
	x2 = x1 + tw1 + item_space + img_w + img_space;
	x3 = x2 + tw2 + item_space + img_w + img_space;
	x4 = x3 + tw3 + item_space + img_w + img_space;
	/* draw home  */
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 6, shift_val, btn_y_pos, img_w, img_h );
	pT->DrawResText( ML_HOME, x1, btn_y_pos+2, _WHITE, 12, DT_TRANSPARENCY );			
	/* draw edit */	
	AddSpace=(x1 + tw1 + item_space);
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 0, AddSpace, btn_y_pos, img_w, img_h );
	pT->DrawResText( ML_INPUT, x2, btn_y_pos+2, _WHITE, 12, DT_TRANSPARENCY );
	/* draw load */	
	AddSpace=(x2 + tw2 + item_space);
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 3, AddSpace, btn_y_pos, img_w, img_h );
	pT->DrawResText( ML_LOAD, x3, btn_y_pos+2, _WHITE, 12, DT_TRANSPARENCY );
	/* draw history */	
	AddSpace=(x3 + tw3 + item_space);
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 5, AddSpace, btn_y_pos, img_w, img_h );
	pT->DrawResText( ML_HISTORY, x4, btn_y_pos+2, _WHITE, 12, DT_TRANSPARENCY );			
			
	pMpegDecoder->RefreshOsd();

}
int ExDic_Draw_OutWin_Bg(void)
{
	_ExDicPro.m_current_text_line = _ExDicPro.m_current_page * 10;
	
	op_ClearAreaOSD( 0, 25, 320, 215 );
	
	int csx, csy;
	if(_ExDicPro.m_found)
	{
		DrawBoxWithoutRefresh( DIC_OUTWIN_TITLE_COLOR, 0, 25, 320, 21);
		DrawBoxWithoutRefresh( DIC_OUTWIN_BASE_COLOR, 0, 46, 320, 170);
	}
	/* Draw scrollbar */
	if( !(_ExDicPro.m_total_text_lines < _ExDicPro.m_visible_lines) )
	{
		ExDic_Draw_Scrollbar(_ExDicPro.m_total_text_lines,
									_ExDicPro.m_current_text_line,
									_ExDicPro.m_visible_lines);
	}
	/* Draw bottom guide  */
	ExDic_DrawBottomGuidePannel();

	return 0;

}


int ExDic_Draw_OutputWin(void)
{
	_ExDicPro.m_go_search = false;
	_ExDicPro.m_current_text_index = 0;
		
	if(_ExDicPro.m_dic_pre_state == DIC_STATE_KEYBOARD_ENGU ||
		_ExDicPro.m_dic_pre_state == DIC_STATE_KEYBOARD_ENGL )
	{
	
		ExDic_Chk_Text2();
		ExDic_Draw_OutWin_Bg();
		ExDic_Draw_Text();
	}
	else if(_ExDicPro.m_dic_pre_state == DIC_STATE_KEYBOARD_DGT)
	{
	
		ExDic_Chk_Text2();
		ExDic_Draw_OutWin_Bg();
		ExDic_Draw_Text();
	}
	else if(_ExDicPro.m_dic_pre_state == DIC_STATE_KEYBOARD_KOR)
	{
		ExDic_Chk_Text2();
		ExDic_Draw_OutWin_Bg();
		ExDic_Draw_Text();
	}
		
	return 0;

}

int ExDic_Set_Keyboard(void)
{
	int  idx=0;
	
	for(idx=0; idx<510; idx+=2)
	{
		if(_ExDicPro.m_last_filename[idx]=='e' && _ExDicPro.m_last_filename[idx+2]=='n')
			return DIC_STATE_KEYBOARD_ENGL;
		else if(_ExDicPro.m_last_filename[idx]=='E' && _ExDicPro.m_last_filename[idx+2]=='n')
			return DIC_STATE_KEYBOARD_ENGL;
		else if(_ExDicPro.m_last_filename[idx]=='k' && _ExDicPro.m_last_filename[idx+2]=='o')
			return DIC_STATE_KEYBOARD_KOR;
		else if(_ExDicPro.m_last_filename[idx]=='K' && _ExDicPro.m_last_filename[idx+2]=='o')
			return DIC_STATE_KEYBOARD_KOR;
	}
	
	/* default value */
	return DIC_STATE_KEYBOARD_ENGL;	


}

int ExDic_OnKeyPress( int key )
{
	CEvent evt;
	CListItem* puzItem;
	CCMListItem* pCMItem;
	CListItem* pListItem;
	int vkey;
	CNode< CWindow >* pNode;
	CComplexListView* pCMView;
	CWindow *pwnd;
	CWindow *pComponentwnd;
	int lang = 0;
		
	vkey = MapToVirtualKey( key );
	evt.type = EVENT_KEY;
	evt.u.key.value = vkey;
	evt.u.key.pressed = KEY_PRESSED;
	
	switch( vkey )
	{
		
		/* Sound Key */
		case VK_F1:
			if(_ExDicPro.m_dic_cur_state == DIC_STATE_OUTWIN)
			{
				ExDic_Draw_History_Box();
				_ExDicPro.m_dic_cur_state = DIC_STATE_HISTORY;

			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGU ||
					_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGL ||
					_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_KOR||
					_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_DGT)
			{
				if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_DGT)
					lang = DIC_STATE_KEYBOARD_KOR;
				else
					lang = _ExDicPro.m_dic_cur_state +1;
				
				switch(lang)
				{
					case 1:  	_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_KOR;
							_ExDicPro.m_pDicKeyboard->SetKBD(DIC_SEARCH_KOR);
							_ExDicPro.m_pDicKeyboard->Set_Member(6,true);
							_ExDicPro.m_pDicKeyboard->ReDraw();
							_ExDicPro.m_pDicKeyboard->Set_Member(6,false);
							break;
							
					case 2:  	_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_ENGU;
							_ExDicPro.m_pDicKeyboard->SetKBD(DIC_SEARCH_ENGU);
							_ExDicPro.m_pDicKeyboard->Set_Member(5,true);
							_ExDicPro.m_pDicKeyboard->ReDraw();
							break;
							
					case 3:  	_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_ENGL;
							_ExDicPro.m_pDicKeyboard->SetKBD(DIC_SEARCH_ENGL);
							_ExDicPro.m_pDicKeyboard->Set_Member(5,true);
							_ExDicPro.m_pDicKeyboard->ReDraw();
							break;
							
					case 4:  	_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_DGT;
							_ExDicPro.m_pDicKeyboard->SetKBD(DIC_SEARCH_DGT);
							_ExDicPro.m_pDicKeyboard->Set_Member(5,true);
							_ExDicPro.m_pDicKeyboard->ReDraw();
							break;
							
					default :
							break;

				}


			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_LISTMANAGER)
			{
				ExDic_Draw_History_Box();
				_ExDicPro.m_dic_cur_state = DIC_STATE_HISTORY;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_NO_DATA)
			{
				;
			}
			break;

		/* Menu Key */
		case VK_F2:
			if(_ExDicPro.m_dic_cur_state == DIC_STATE_OUTWIN)
			{
				ExDic_Draw_DM_Box();
				_ExDicPro.m_dic_cur_state = DIC_STATE_DATAMANAGER;
			}
			else if(   _ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_KOR		||
					_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGU   	||
					_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGL 	||
					_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_DGT		)
			{
				
				switch(_ExDicPro.m_dic_cur_state)
				{
					case 1:  
					_ExDicPro.m_pDicKeyboard->GoSearch(DIC_SEARCH_KOR);
					if(_ExDicPro.m_go_search)
					{
						ExDic_Search_Word();
						if(_ExDicPro.m_found)
						{
							_ExDicPro.m_pHistory->Append(_ExDicPro.m_word);
							_ExDicPro.m_total_text_lines = 0;
							_ExDicPro.m_current_text_line = 0;
							ExDic_Draw_OutputWin();
							_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
						}
						else
						{
							ExDic_SrchFailCase_Proc(DIC_STATE_LISTMANAGER,DIC_STATE_KEYBOARD_KOR);
						}
					}
					_ExDicPro.m_go_search = false;
					break;
							
					case 2:  
					_ExDicPro.m_pDicKeyboard->GoSearch(DIC_SEARCH_ENGU);
					if(_ExDicPro.m_go_search)
					{
						ExDic_Search_Word();
						if(_ExDicPro.m_found)
						{
							_ExDicPro.m_pHistory->Append(_ExDicPro.m_word);
							_ExDicPro.m_total_text_lines = 0;
							_ExDicPro.m_current_text_line = 0;
							ExDic_Draw_OutputWin();
							_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
						}
						else
						{
							ExDic_SrchFailCase_Proc(DIC_STATE_LISTMANAGER,DIC_STATE_KEYBOARD_ENGU);
						}
					}
					_ExDicPro.m_go_search = false;
					break;
							
					case 3:  	
					_ExDicPro.m_pDicKeyboard->GoSearch(DIC_SEARCH_ENGL);
					if(_ExDicPro.m_go_search)
					{
						ExDic_Search_Word();
						if(_ExDicPro.m_found)
						{
							_ExDicPro.m_pHistory->Append(_ExDicPro.m_word);
							_ExDicPro.m_total_text_lines = 0;
							_ExDicPro.m_current_text_line = 0;
							ExDic_Draw_OutputWin();
							_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
						}
						else
						{
							ExDic_SrchFailCase_Proc(DIC_STATE_LISTMANAGER,DIC_STATE_KEYBOARD_ENGL);
						}
					}
					_ExDicPro.m_go_search = false;
					break;
							
					case 4:  	
					_ExDicPro.m_pDicKeyboard->GoSearch(DIC_SEARCH_DGT);
					if(_ExDicPro.m_go_search)
					{
						ExDic_Search_Word();
						if(_ExDicPro.m_found)
						{
							_ExDicPro.m_pHistory->Append(_ExDicPro.m_word);
							_ExDicPro.m_total_text_lines = 0;
							_ExDicPro.m_current_text_line = 0;
							ExDic_Draw_OutputWin();
							_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
						}
						else
						{
							ExDic_SrchFailCase_Proc(DIC_STATE_LISTMANAGER,DIC_STATE_KEYBOARD_DGT);
						}
					}
					_ExDicPro.m_go_search = false;
					break;
							
					default :
							break;

				}
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_LISTMANAGER)
			{
				ExDic_Draw_DM_Box();
				_ExDicPro.m_dic_cur_state = DIC_STATE_DATAMANAGER;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_NO_DATA)
			{
				;
			}
			break;

		case VK_UP:
			if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGU ||
				_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGL )
			{
				_ExDicPro.m_pDicKeyboard->CursorUp();
				_ExDicPro.m_pDicKeyboard->ReDraw();
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_KOR)
			{
				_ExDicPro.m_pDicKeyboard->CursorUp();
				_ExDicPro.m_pDicKeyboard->ReDraw();
				
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_DGT)
			{
				_ExDicPro.m_pDicKeyboard->CursorUp();
				_ExDicPro.m_pDicKeyboard->ReDraw();
				
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_OUTWIN)
			{
				if(_ExDicPro.m_current_page > 0)
					_ExDicPro.m_current_page--;
				ExDic_Draw_OutWin_Bg();
				ExDic_Draw_Text();
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_HISTORY)
			{
				_ExDicPro.m_pHistory->Up();
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_DATAMANAGER)
			{
				_ExDicPro.m_pDM->Up();
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_LISTMANAGER)
			{
				_ExDicPro.m_pLM->Up();
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_NO_DATA)
			{
				;
			}
			break;
					
		case VK_DOWN:
			if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGU ||
				_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGL )
			{
				_ExDicPro.m_pDicKeyboard->CursorDown();
				_ExDicPro.m_pDicKeyboard->ReDraw();
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_KOR)
			{
				_ExDicPro.m_pDicKeyboard->CursorDown();
				_ExDicPro.m_pDicKeyboard->ReDraw();
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_DGT)
			{
				_ExDicPro.m_pDicKeyboard->CursorDown();
				_ExDicPro.m_pDicKeyboard->ReDraw();
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_OUTWIN)
			{
				if(_ExDicPro.m_total_page > _ExDicPro.m_current_page)
				{	_ExDicPro.m_current_page++; }
				_ExDicPro.m_down_pressed = true;
				ExDic_Draw_OutWin_Bg();
				ExDic_Draw_Text();
				_ExDicPro.m_down_pressed = false;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_HISTORY)
			{
				_ExDicPro.m_pHistory->Down();
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_DATAMANAGER)
			{
				_ExDicPro.m_pDM->Down();
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_LISTMANAGER)
			{
				_ExDicPro.m_pLM->Down();
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_NO_DATA)
			{
				;
			}
			break;
			
					
		case VK_LEFT:
			if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGU ||
				_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGL )
			{
					_ExDicPro.m_pDicKeyboard->CursorLeft();
					_ExDicPro.m_pDicKeyboard->ReDraw();
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_KOR)
			{
				_ExDicPro.m_pDicKeyboard->CursorLeft();
					_ExDicPro.m_pDicKeyboard->ReDraw();
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_DGT)
			{
				_ExDicPro.m_pDicKeyboard->CursorLeft();
					_ExDicPro.m_pDicKeyboard->ReDraw();
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_OUTWIN)
			{
				;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_HISTORY)
			{
				;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_LISTMANAGER)
			{
				;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_NO_DATA)
			{
				;
			}
			break;
					
		case TAVI_KEY_LRIGHT:
			if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGU ||
				_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGL )
			{
					_ExDicPro.m_pDicKeyboard->CursorLongRight();
					_ExDicPro.m_pDicKeyboard->ReDraw();
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_OUTWIN)
			{
				;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_HISTORY)
			{
				;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_LISTMANAGER)
			{
				;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_NO_DATA)
			{
				;
			}
			break;
					
		case VK_RIGHT:
			if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGU ||
				_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGL )
			{
					_ExDicPro.m_pDicKeyboard->CursorRight();
					_ExDicPro.m_pDicKeyboard->ReDraw();
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_KOR)
			{
				_ExDicPro.m_pDicKeyboard->CursorRight();
					_ExDicPro.m_pDicKeyboard->ReDraw();
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_DGT)
			{
				_ExDicPro.m_pDicKeyboard->CursorRight();
					_ExDicPro.m_pDicKeyboard->ReDraw();
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_OUTWIN)
			{
				;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_HISTORY)
			{
				;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_LISTMANAGER)
			{
				;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_NO_DATA)
			{
				;
			}
			break;

		case VK_ENTER:
			if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGL )
			{
				_ExDicPro.m_pDicKeyboard->CursorOK(DIC_KEY_NORMAL);
			
				if(_ExDicPro.m_go_search)
				{
					ExDic_Search_Word();
					if(_ExDicPro.m_found)
					{
						_ExDicPro.m_pHistory->Append(_ExDicPro.m_word);
						ExDic_Draw_OutputWin();
						_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
					}
					else
					{
						ExDic_SrchFailCase_Proc(DIC_STATE_LISTMANAGER,DIC_STATE_KEYBOARD_ENGL);
					}
				}
				_ExDicPro.m_go_search = false;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGU )
			{
				_ExDicPro.m_pDicKeyboard->CursorOK(DIC_KEY_NORMAL);
			
				if(_ExDicPro.m_go_search)
				{
					ExDic_Search_Word();
					if(_ExDicPro.m_found)
					{
						_ExDicPro.m_pHistory->Append(_ExDicPro.m_word);
						ExDic_Draw_OutputWin();
						_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
					}
					else
					{
						ExDic_SrchFailCase_Proc(DIC_STATE_LISTMANAGER,DIC_STATE_KEYBOARD_ENGU);
					}
				}
				_ExDicPro.m_go_search = false;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_KOR)
			{
				_ExDicPro.m_pDicKeyboard->CursorOK(DIC_KEY_NORMAL);
				
				if(_ExDicPro.m_go_search)
				{
					
					ExDic_Search_Word();
					if(_ExDicPro.m_found)
					{
						_ExDicPro.m_pHistory->Append(_ExDicPro.m_word);
						ExDic_Draw_OutputWin();
						_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
					}
					else
					{
						ExDic_SrchFailCase_Proc(DIC_STATE_LISTMANAGER,DIC_STATE_KEYBOARD_KOR);
					}
				}
				_ExDicPro.m_go_search = false;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_DGT)
			{
				_ExDicPro.m_pDicKeyboard->CursorOK(DIC_KEY_NORMAL);
				
				if(_ExDicPro.m_go_search)
				{
					
					ExDic_Search_Word();
					if(_ExDicPro.m_found)
					{
						_ExDicPro.m_pHistory->Append(_ExDicPro.m_word);
						ExDic_Draw_OutputWin();
						_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
					}
					else
					{
						ExDic_SrchFailCase_Proc(DIC_STATE_LISTMANAGER,DIC_STATE_KEYBOARD_DGT);
					}
				}
				_ExDicPro.m_go_search = false;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_OUTWIN)
			{
				op_ClearAreaOSD( 0, 25, 320, 215 );
				
				switch(_ExDicPro.m_dic_pre_state)
				{
					case 1:
							_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_KOR;
							_ExDicPro.m_pDicKeyboard->Show();
							break;
					case 2:  	
							_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_ENGU;
							_ExDicPro.m_pDicKeyboard->Set_Member(5,true);
							_ExDicPro.m_pDicKeyboard->Show();
							break;
					case 3:  	
							_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_ENGL;
							_ExDicPro.m_pDicKeyboard->Set_Member(5,true);
							_ExDicPro.m_pDicKeyboard->Show();
							break;
					case 4: 
							_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_DGT;
							_ExDicPro.m_pDicKeyboard->Set_Member(5,true);
							_ExDicPro.m_pDicKeyboard->Show();
							break;
					default :
							break;


				}
				
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_HISTORY)
			{
				strcpy(_ExDicPro.m_word, _ExDicPro.m_pHistory->Ok());
				ExDic_Search_Word();
				if(_ExDicPro.m_found)
				{
					_ExDicPro.m_pHistory->Append(_ExDicPro.m_word);
					ExDic_Draw_OutputWin();
					_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
				}
				else
				{
					ExDic_SrchFailCase_Proc(DIC_STATE_LISTMANAGER,DIC_STATE_KEYBOARD_KOR);
				}
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_DATAMANAGER)
			{
				int ret_ok=0;
				
				if((ret_ok=_ExDicPro.m_pDM->Ok()) == -1)
				{
					fprintf(stderr,"file open error\n");
					break;
				}
				else if(ret_ok == -2)
				{
					fprintf(stderr,"Not KDIC\n");
					_ExDicPro.m_pMBx_NoKDicType->Show();
					_ExDicPro.m_dic_cur_state = DIC_STATE_NOT_KDIC_TYPE;
					break;
				}
					
				op_ClearAreaOSD( 0, 25, 320, 215 );
				switch(ExDic_Set_Keyboard())
				{
					case 1:  	_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_KOR;
							_ExDicPro.m_pDicKeyboard->SetKBD(DIC_SEARCH_KOR);
							_ExDicPro.m_pDicKeyboard->Show();
							break;
					case 2:  	_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_ENGU;
							_ExDicPro.m_pDicKeyboard->SetKBD(DIC_SEARCH_ENGU);
							_ExDicPro.m_pDicKeyboard->Set_Member(5,true);
							_ExDicPro.m_pDicKeyboard->Show();
							break;
					case 3:  	_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_ENGL;
							_ExDicPro.m_pDicKeyboard->SetKBD(DIC_SEARCH_ENGL);
							_ExDicPro.m_pDicKeyboard->Set_Member(5,true);
							_ExDicPro.m_pDicKeyboard->Show();
							break;
					case 4:  	_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_DGT;
							_ExDicPro.m_pDicKeyboard->SetKBD(DIC_SEARCH_DGT);
							_ExDicPro.m_pDicKeyboard->Set_Member(5,true);
							_ExDicPro.m_pDicKeyboard->Show();
							break;
					default :
							break;


				}
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_LISTMANAGER)
			{
				strcpy(_ExDicPro.m_word, _ExDicPro.m_pLM->Ok());
				//_ExDicPro.m_pLM->Delete();
				ExDic_Search_Word();
				if(_ExDicPro.m_found)
				{
					_ExDicPro.m_pHistory->Append(_ExDicPro.m_word);
					ExDic_Draw_OutputWin();
					_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
				}
				else
				{
					ExDic_Draw_LM_Box();
					ExDic_Draw_Text();
					_ExDicPro.m_dic_cur_state = DIC_STATE_LISTMANAGER;
				}
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_NO_DATA)
			{
				_ExtraProperty.m_SubDicState = DIC_EXIT_NORMAL;
				_ExDicPro.m_fExit = true;
				return -1;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_NOT_SELECT_DATA ||
				_ExDicPro.m_dic_cur_state == DIC_STATE_NOT_KDIC_TYPE )
			{
				ExDic_Draw_DM_Box();
				_ExDicPro.m_dic_cur_state = DIC_STATE_DATAMANAGER;
			}
			break;			
					
		/* Mode Short */
		case VK_F3:		DEBUGMSG(1,("Mode KEY pressed\n"));
			if(_ExDicPro.m_dic_cur_state == DIC_STATE_OUTWIN)
			{
						_ExtraProperty.m_SubDicState = DIC_EXIT_BACKGROUND;
						_ExDicPro.m_fExit = true;
						DEBUGMSG(1,("Out Extra_OnKeyPress() as Background\n"));
						return -1;
			}
						break;
		/* Exit Short */
		case VK_ESC:	
			if(_ExDicPro.m_dic_cur_state == DIC_STATE_OUTWIN)
			{
				_ExDicPro.m_pDicKeyboard->Key_Buffer_Clear(0);
				_ExDicPro.m_pDicKeyboard->Key_Index_Clear(0);
				_ExtraProperty.m_SubDicState = DIC_EXIT_NORMAL;
				_ExDicPro.m_fExit = true;
				return -1;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_HISTORY)
			{
				ExDic_Draw_OutWin_Bg();
				ExDic_Draw_Text();
				_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_DATAMANAGER)
			{
				if(_ExDicPro.m_isLoad_DicData)
				{
					_ExDicPro.m_pDM->Delete();
					ExDic_Draw_OutWin_Bg();
					ExDic_Draw_Text();
					_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
				}
				else
				{
					_ExDicPro.m_pMBx_NoSelectDic->Show();
					_ExDicPro.m_dic_cur_state = DIC_STATE_NOT_SELECT_DATA;
				}
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_LISTMANAGER)
			{
				//_ExDicPro.m_pLM->Delete();
				_ExDicPro.m_total_text_lines = 0;
				_ExDicPro.m_current_text_line = 0;
				ExDic_Draw_OutWin_Bg();
				_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_NO_DATA)
			{
				_ExtraProperty.m_SubDicState = DIC_EXIT_NORMAL;
				_ExDicPro.m_fExit = true;
				return -1;
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_NOT_SELECT_DATA ||
				_ExDicPro.m_dic_cur_state == DIC_STATE_NOT_KDIC_TYPE )
			{
				ExDic_Draw_DM_Box();
				_ExDicPro.m_dic_cur_state = DIC_STATE_DATAMANAGER;
			}
			else
			{
				_ExDicPro.m_pDicKeyboard->Exit();
				ExDic_Search_Word();
				if(_ExDicPro.m_found)
				{
					ExDic_Draw_OutputWin();
					_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
				}
				else
				{
					ExDic_Draw_LM_Box();
					ExDic_Draw_Text();
					_ExDicPro.m_dic_cur_state = DIC_STATE_LISTMANAGER;
				}
				
			}
			break;
			
		/* Mode Long */				
		case TAVI_KEY_LMODE:
			if(_ExDicPro.m_dic_cur_state == DIC_STATE_OUTWIN)
			{
				_ExtraProperty.m_SubDicState = DIC_EXIT_SWITCH;
				_ExDicPro.m_fExit = true;	
				return -1;
			}
			break;
			
		/* Exit Long */		
		case VK_EXT_ESC:	DEBUGMSG(1,("Exit Long KEY pressed\n"));
			/* case alarm working value : 5 */
			_ExtraProperty.m_SubDicState = DIC_EXIT_ALARM;
			_ExDicPro.m_fExit = true;
			DEBUGMSG(1,("Out Extra_OnKeyPress() as ExitAndSwitch\n"));
			return -1;
			break;
						
		/* Up volume on the background mode */
		case TAVI_KEY_JOG1_UP:
						TAVI_BG_Volume( 1 );
						break;
		/* Down volume on the background mode */
		case TAVI_KEY_JOG1_DOWN:
						TAVI_BG_Volume( 0 );
						break;
		/* Move to next music on the background mode */				
		case TAVI_KEY_JOG2_UP:
						TAVI_BG_Next(  );
						break;
		/* Move to prev music on the background mode */				
		case TAVI_KEY_JOG2_DOWN:
						TAVI_BG_Prev(  );
						break;
		/* Play & Pause music on the background mode */				
		case TAVI_KEY_JOG2_ENTER:
						TAVI_BG_PlayPause(  );
						break;
		
						
		default: DEBUGMSG(1,("Other KEY pressed\n"));
			break;
		
	}

	return 0;
			
}

int ExDic_MsgProc( int ssTaskId, U32 ssMsgId, int usParam1, int usParam2 )
{
	MouseEvent mevt;
	KeyEvent key;
	switch( ssMsgId )
	{
		case MSG_CREATE:		
		case MSG_DRAW:			
		case MSG_TIMER:			
		case MSG_SETFOCUS : 	
		case MSG_KILLFOCUS: 	
		case MSG_DESTROY:		
		case MSG_BUTTON_DN:
			break;
			
		case MSG_BUTTON_UP: 
			if (usParam1 == TAVI_KEY_SHUTDOWN)
			{
				_ExtraProperty.m_SubDicState = DIC_EXIT_NORMAL;
				_ExDicPro.m_fExit = true;
				return -1;
			}
			return ExDic_OnKeyPress(usParam1);
			break;
						
		case MSG_BATTERY:
			break;
				
		case MSG_TOUCHPAD:
		 	if(_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGU ||
				_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGL ||
				_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_KOR||
				_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_DGT	)
		 	{
				if(usParam1 == -1)
					_ExDicPro.m_pDicKeyboard->CursorOK(DIC_KEY_DELETE);
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_HISTORY)
			{
				if(usParam1 == -1)
				{
					_ExDicPro.m_pHistory->Up();
				}
				else if(usParam1 == 1)
				{
					_ExDicPro.m_pHistory->Down();
				}
				else if(usParam2 == 1)
				{
					strcpy(_ExDicPro.m_word, _ExDicPro.m_pHistory->Ok());
					ExDic_Search_Word();
					if(_ExDicPro.m_found)
					{
						_ExDicPro.m_pHistory->Append(_ExDicPro.m_word);
						ExDic_Draw_OutputWin();
						_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
					}
					else
					{
						ExDic_SrchFailCase_Proc(DIC_STATE_LISTMANAGER,DIC_STATE_KEYBOARD_KOR);
					}
				}
					
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_DATAMANAGER)
			{
				if(usParam1 == -1)
				{
					_ExDicPro.m_pDM->Up();
				}
				else if(usParam1 == 1)
				{
					_ExDicPro.m_pDM->Down();
				}
				else if(usParam2 == 1)
				{
					int ret_ok=0;
					
					if((ret_ok=_ExDicPro.m_pDM->Ok()) == -1)
					{
						fprintf(stderr,"file open error\n");
						break;
					}
					else if(ret_ok == -2)
					{
						fprintf(stderr,"Not KDIC\n");
						_ExDicPro.m_pMBx_NoKDicType->Show();
						_ExDicPro.m_dic_cur_state = DIC_STATE_NOT_KDIC_TYPE;
						break;
					}
						
					op_ClearAreaOSD( 0, 25, 320, 215 );
					switch(ExDic_Set_Keyboard())
					{
						case 1:  	_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_KOR;
								_ExDicPro.m_pDicKeyboard->SetKBD(DIC_SEARCH_KOR);
								_ExDicPro.m_pDicKeyboard->Show();
								break;
						case 2:  	_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_ENGU;
								_ExDicPro.m_pDicKeyboard->SetKBD(DIC_SEARCH_ENGU);
								_ExDicPro.m_pDicKeyboard->Set_Member(5,true);
								_ExDicPro.m_pDicKeyboard->Show();
								break;
						case 3:  	_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_ENGL;
								_ExDicPro.m_pDicKeyboard->SetKBD(DIC_SEARCH_ENGL);
								_ExDicPro.m_pDicKeyboard->Set_Member(5,true);
								_ExDicPro.m_pDicKeyboard->Show();
								break;
						case 4:  	_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_DGT;
								_ExDicPro.m_pDicKeyboard->SetKBD(DIC_SEARCH_DGT);
								_ExDicPro.m_pDicKeyboard->Set_Member(5,true);
								_ExDicPro.m_pDicKeyboard->Show();
								break;
						default :
								break;


					}
				}
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_LISTMANAGER)
			{
				if(usParam1 == -1)
				{
					_ExDicPro.m_pLM->Up();
				}
				else if(usParam1 == 1)
				{
					_ExDicPro.m_pLM->Down();
				}
				else if(usParam2 == 1)
				{
					strcpy(_ExDicPro.m_word, _ExDicPro.m_pLM->Ok());
					
					ExDic_Search_Word();
					if(_ExDicPro.m_found)
					{
						_ExDicPro.m_pHistory->Append(_ExDicPro.m_word);
						ExDic_Draw_OutputWin();
						_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
					}
					else
					{
						ExDic_Draw_LM_Box();
						ExDic_Draw_Text();
						_ExDicPro.m_dic_cur_state = DIC_STATE_LISTMANAGER;
					}
				}
			}
			break;
			
		default:
			return false;
			break;
	}
	pMpegDecoder->RefreshOsd (); /* refresh screen */							

	return 0;
}

int ExDic_Go_DicLoad(void)
{
	_ExDicPro.m_pDM->LoadFromFile();
	_ExDicPro.m_pDM->Show();
	_ExDicPro.m_dic_cur_state = DIC_STATE_DATAMANAGER;
	
	return 0;
}
int ExDic_Chk_DicData(void)
{
	_ExDicPro.m_pDM->LoadFromFile();
	return 0;
}
int ExDic_Get_SaveDicName(void)
{
	if(_ExDicPro.m_pDM->Load_LastAccess_File() == 1)
	{
		return 1;
	}
	else
	{
		_ExDicPro.m_LastAccessData = false;
		return -1;

	}
}
int ExDic_Chk_HistoryData(void)
{
	/* print finaly searched word */
	if(_ExDicPro.m_pHistory->Chk_History(HISTORY_CHK_LOAD_STATE))
	{
		_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
		strcpy(_ExDicPro.m_word ,
			_ExDicPro.m_pHistory->GetWord(_ExDicPro.m_pHistory->Chk_History(HISTORY_CHK_LOAD_COUNT)));

		if(_ExDicPro.m_isLoad_DicData == false)
		{
			fprintf(stderr,"[HISTORY] LOADING...  %s\n",_ExDicPro.m_last_filename);
			if(_ExDicPro.m_pDM->Load_Pdb(_ExDicPro.m_last_filename) < 0)
				return -1; /* load pdb error */
		}
		
		ExDic_Search_Word();
		
		if(_ExDicPro.m_found)
		{
			ExDic_Chk_Text2();
			ExDic_Draw_OutWin_Bg();
			ExDic_Draw_Text();
		}
		else
		{
			ExDic_SrchFailCase_Proc(DIC_STATE_LISTMANAGER,DIC_STATE_KEYBOARD_KOR);
		}
	}
	else
	{
		_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_KOR;
		_ExDicPro.m_pDicKeyboard->Show();
	}
	return 1;
}
int ExDic_Load_SaveDic(void)
{
	char 			tmp_filename[512];
	uchar_t 			tmp_ufilename[512];
	char				file_path[512];
	RMuint32			fd=0;
	FATFS_ERROR		err;
	
	memset(tmp_ufilename, 0, 512);
	memset(tmp_ufilename, 0, 512);
	ustrcpy(tmp_ufilename, (uchar_t*)(_ExDicPro.m_last_filename+(strlen(DIC_DB_PATH)*2)));
	memcpy((void*)tmp_filename,
				(void*)( _ExDicPro.m_last_filename+(strlen(DIC_DB_PATH)*2)),
				512-(strlen(DIC_DB_PATH)*2));
	//fprintf(stderr,"\ntmp_filename\n");
	//for(int i=0; i<50;i++)
	//{
	//	fprintf(stderr,"[0x%x]",tmp_filename[i]);
	//}
	
	memset(file_path, 0, 512);
	memcpy(file_path,_ExDicPro.m_last_filename, 512);
	
	if( (err = fatfs_wfopen ((RMuint16*)file_path, _O_RDONLY, &fd)) > 0 )
	{	
		fprintf(stderr, "\nExDic_Load_SaveDic : fopen : error\n");
		_ExDicPro.m_LastAccessData = false;
		return -2; /* Search error */
	}
	else
	{
		fatfs_fclose(fd);
		fprintf(stderr,"LOADING...[Saved dic name: %s]\n",tmp_filename);
		if(_ExDicPro.m_pDM->Load_Pdb(tmp_filename) < 0)
		{
			_ExDicPro.m_LastAccessData = false;
			return -1; /* Load pdb error */
		}
		_ExDicPro.m_LastAccessData = true;
		_ExDicPro.m_isLoad_DicData = true;
	}

	return 1;
}

int ExDic_SaveHDD(void)
{
	/* Save last access file */
	if(_ExDicPro.m_pDM->Save_LastAccess_File() == 1)
		fprintf(stderr, "SAVE : LastAccessFile : Successed\n");
	/* Save History */
	if(_ExDicPro.m_pHistory->SaveToFile() == 0)
		fprintf(stderr, "SAVE : History : Successed\n");
	
	return 1;
}

extern int ExDic_Task(void)
{
	TMsgEntry tMsg;
	extern int handle_key;

	ExDic_Init();
	//fprintf(stderr, "Task : Pass...................................................... 0\n");
	ExDic_Chk_DicData();
	//fprintf(stderr, "Task : Pass...................................................... 1\n");
	ExDic_MainScreen();
	//fprintf(stderr, "Task : Pass...................................................... 2\n");
	if(_ExDicPro.m_isExist_DicData)
	{
		if(ExDic_Get_SaveDicName() == 1)
		{
	//fprintf(stderr, "Task : Pass...................................................... 3\n");
			if(ExDic_Load_SaveDic() < 0)
			{
	//fprintf(stderr, "Task : Pass...................................................... 4\n");
				ExDic_Go_DicLoad();
			}
			else
			{
	//fprintf(stderr, "Task : Pass...................................................... 5\n");
				if(ExDic_Chk_HistoryData() < 0)
				{
	//fprintf(stderr, "Task : Pass...................................................... 6\n");
					ExDic_Go_DicLoad();
				}
			}
		}
		else
		{
			ExDic_Go_DicLoad();
		}
	}
	else
	{
		ExDic_Draw_OutWin_Bg();
		_ExDicPro.m_pMBx_NoDic->Show();
		_ExDicPro.m_dic_cur_state = DIC_STATE_NO_DATA;
	}
	
//	ioctl( handle_key, TAVI_KEY_IOCS_REPEATMODE, 0 );
//	ioctl( handle_key, TAVI_KEY_IOCS_SINGLEEVENT, 0 );
	
	
//	SetTaviKeyMode(TK_REPEAT); /* Key & TouchPad */
	mep_report( MEP_REPORT_RELATIVE);

	_ExDicPro.m_fExit = false;	/* Set Exit State */
	/* Task resume for background */
	if( _ExDicPro.m_Resume == true) _ExDicPro.m_Resume = false;
	
	ClearKeyBuffer();
	
	//TAVI_CanSleep(27); /* Start HDD Sleep*/
	//fprintf(stderr, "Task : Pass...................................................... 7\n");
	while( _ExDicPro.m_fExit == false )
	{
		if( (_ExDicPro.m_key_idle=MSG_Peek( 7, &tMsg )) == 0)	 /* Message peek ,  Extra Task ID = 7 */
		{
			 _ExDicPro.m_sx_scroll = 0;
			 
			if(ExDic_MsgProc( tMsg.ssToTaskId, tMsg.ulMsgId, tMsg.usParam1, tMsg.usParam2 ) == -1)
			{
				DEBUGMSG(1,("Break while loop\n"));
				break;
			}
		}
		else if(_ExDicPro.m_key_idle == -1)
		{ 
			if(_ExDicPro.m_dic_cur_state == DIC_STATE_OUTWIN)
			{
				usleep(10000); /* 10,000 usec = 0.01sec */
				ExDic_Draw_ScrollTxt(DIC_OUTWIN_TITLE_SCROLL_W);
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_LISTMANAGER)
			{
				usleep(10000); /* 10,000 usec = 0.01sec */
				ExDic_Draw_ScrollTxt(DIC_SELECTBAR_TEXT_SCROLL_W);
			}
			else if(_ExDicPro.m_dic_cur_state == DIC_STATE_HISTORY)
			{
				usleep(10000); /* 10,000 usec = 0.01sec */
				ExDic_Draw_ScrollTxt(DIC_SELECTBAR_TEXT_SCROLL_W);
			}
		}
			
		/*  Process to keyboard input timer */
		if(	_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGU 	||
			_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_ENGL		||
			_ExDicPro.m_dic_cur_state == DIC_STATE_KEYBOARD_DGT	)
		{
			if( (_ExDicPro.m_pDicKeyboard->Get_Member(2) == true) ) /*   2 = m_Time_Proc */
			{
				_ExDicPro.m_pDicKeyboard->Check_Time_Remain();
				/* 1 = m_isTimeRemain */
				if(_ExDicPro.m_pDicKeyboard->Get_Member(1) == false)
				{
					/* 3 = m_Key_Selected */
					_ExDicPro.m_pDicKeyboard->Set_Member(4 , 1);
					_ExDicPro.m_pDicKeyboard->ReDraw();
					_ExDicPro.m_pDicKeyboard->Set_Member(4 , 0);
				}
					
			}
		}
		
		usleep(1000);	/* 1,000,000 usec = 1sec */		
	}
	
	ExDic_SaveHDD();
	ExDic_FinishScreen();
//	TAVI_CannotSleep(17); /* Stop HDD Sleep*/
	ExDic_Free();
		
//	ioctl( handle_key, TAVI_KEY_IOCS_REPEATMODE, 1 );
//	ioctl( handle_key, TAVI_KEY_IOCS_SINGLEEVENT, 1 );

	return 0;

}


