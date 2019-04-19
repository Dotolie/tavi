//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : File Manager Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/DataManager.cpp,v $
// $Revision: 1.4 $
// $Date: 2006/03/06 11:18:22 $
// $Author: akaiaphanu $
//
//*****************************************************************************
// $Log: DataManager.cpp,v $
// Revision 1.4  2006/03/06 11:18:22  akaiaphanu
// working UI
//
// Revision 1.3  2006/02/28 04:46:11  akaiaphanu
// working kor,eng filename
//
// Revision 1.2  2006/02/27 02:40:17  akaiaphanu
// working
//
// Revision 1.10  2006/02/22 04:16:28  akaiaphanu
// working selectbar, itemlist
//
// Revision 1.9  2006/02/21 05:04:46  akaiaphanu
// exception message : case not k-dic
//
// Revision 1.8  2006/02/21 02:04:43  akaiaphanu
// working title, list manager
//
// Revision 1.7  2006/02/20 08:06:47  akaiaphanu
// change to array of structure from double linked list
//
// Revision 1.6  2006/02/20 03:04:30  akaiaphanu
// add function for modekey
//
// Revision 1.5  2006/02/20 01:33:56  akaiaphanu
// add ListManager , BitIO
//
// Revision 1.4  2006/02/14 08:37:16  akaiaphanu
// working bottom guide , scrollbar , box
//
// Revision 1.3  2006/02/11 12:40:14  akaiaphanu
// working dictionary keyboard
//
// Revision 1.2  2006/02/11 07:19:11  akaiaphanu
// work for dictionary
//
// Revision 1.1  2006/02/09 07:45:58  akaiaphanu
// add to Data manager class
//
//
//*****************************************************************************

#include "DataManager.h"
extern EXTRA_DIC_CONFIG	 _ExDicPro;
u16 SwapWord21(u16 r)
{
	return (r>>8) + (r<<8);
}

u16 SwapWord12(u16 r)
{
	return r;  
}

u32 SwapLong4321(u32 r)
{
	return  ((r>>24) & 0xFF) + (r<<24) + ((r>>8) & 0xFF00) + ((r<<8) & 0xFF0000);
}

u32 SwapLong1234(u32 r)
{
	return r;
}
DataManager::DataManager(int x, int y, int width, int height )
{
	m_visibleline = DM_VISIBLE_LINE;
	
	m_Xpos = x;
	m_Ypos = y;
	m_Width = width;
	m_Height = height;

	m_Title_Xpos = m_Xpos + 10;
	m_Title_Ypos = m_Ypos + 5;
	m_Title_Height = 21;

	m_Text_Xpos = m_Title_Xpos;
	m_Text_Ypos = m_Ypos + m_Title_Height + 10;

	m_SelectBar_Xpos = m_Text_Xpos - 5;
	m_SelectBar_Ypos = m_Text_Ypos - 5;
	m_SelectBar_Height = 22;
	m_SelectBar_Width = m_Width -10 - 8-9;

	m_ScrollBar_Width = 8;
	m_ScrollBar_Xpos = m_Xpos + m_Width-5 - m_ScrollBar_Width;
	m_ScrollBar_Ypos = m_SelectBar_Ypos-1;
	m_ScrollBar_Height = m_SelectBar_Height * m_visibleline-3;
	
	
	m_pScrollbar3 = new CScrollBar3( m_ScrollBar_Xpos,  m_ScrollBar_Ypos, m_ScrollBar_Height,  1, 1, 1 );
	m_total_index = 0;
	m_cur_index = 1;
	m_cur_page = 1;
	m_count = 0;
	m_Head = NULL;
	m_Tail = NULL;
	
}
DataManager::~DataManager(void)
{
	DMNode * CurNode = m_Head->pNext;
	DMNode * NextNode;
	
	while(CurNode != m_Tail)
	{
		CurNode ->pPrev->pNext = CurNode->pNext;
		CurNode ->pNext->pPrev = CurNode->pPrev;
		NextNode = CurNode->pNext;
		//fprintf(stderr,"DataM ->[%d] %s\n", CurNode->num,CurNode->file_name);
		free(CurNode);
		CurNode = NextNode;
	}
	
	free(m_Head);
	free(m_Tail);
	delete(m_pScrollbar3);
}
int  DataManager::Load_LastAccess_File(void)
{
	RMuint32 fd = 0, i = 0, j=0, nread = 0;
	FATFS_ERROR err;
	uchar_t tmp[512];
	memset(tmp,0,512);

	fatfs_fopen ((RMuint8*)DM_DATA_FILE_PATH, _O_RDONLY, &fd);
	if(fd == 0)
	{	
			fprintf(stderr, "Data Manager : Load_LastAccess_File : fopen error \n");
			return -1;
	}
	//memset(_ExDicPro.m_last_ufilename,0x00,512);
	memset(_ExDicPro.m_last_filename,0x00,512);
	if( (err = fatfs_fread( fd, (RMuint8*)&_ExDicPro.m_last_filename, sizeof(_ExDicPro.m_last_filename), &nread )) > 0 )
	{	
		fprintf(stderr, "Data Manager : Load_LastAccess_File : fread error \n");
		fatfs_fclose(fd);
		return -1;
	}
	
	fatfs_fclose(fd);

	fprintf(stderr,"\nlast_filename : nread=%d\n", nread);
	//for(i=0; i<50;i++)
	//	fprintf(stderr,"[0x%x]",_ExDicPro.m_last_filename[i]);

	fprintf(stderr, "\nData Manager : Load_LastAccess_File : Success\n");

	return 1;

}

int  DataManager::Save_LastAccess_File(void)
{
	RMuint32 fd = 0, nwrite = 0, i=0;
	RMuint32 len = 0;
	
	fatfs_fopen ((RMuint8*)DM_DATA_FILE_PATH, _O_WRONLY|_O_CREAT|_O_TRUNC, &fd);
	
	if(fd == 0)
	{
		fprintf(stderr, "Data Manager : Save_LastAccess_File : fopen error\n");
		return -1;
	}

	fatfs_fwrite( fd, (RMuint8*)&_ExDicPro.m_last_filename, sizeof(_ExDicPro.m_last_filename), &nwrite );
	fatfs_fclose(fd);

	fprintf(stderr,"\nlast_ufilename : nwrite=%d\n",nwrite);
	//for(i=0; i<40;i++)
	//	fprintf(stderr,"[0x%x]",_ExDicPro.m_last_filename[i]);

	fprintf(stderr, "Data Manager : Save_LastAccess_File : Success\n");
	return 1;

}
	
int DataManager::Chk_isKDicType(char* filename)
{
	FATFS_ERROR 	err;
	char 			tmp[512];
	char				file_path[512];
	RMuint32 		fd = 0, fsize=0;
	u32 				nread = 0;
	u32 				value = 0;
	int				i=0,j=0;
	
	memset(file_path,0,512);
	memset(tmp,0,512);
	sprintf(tmp,"%s",DM_DATA_DIR_PATH);
	for(i=0, j=0; j<strlen(DM_DATA_DIR_PATH);j++)
	{
		file_path[i++] = tmp[j];
		file_path[i++] = 0x00;
	}
	memcpy(file_path+(strlen(DM_DATA_DIR_PATH)*2),filename,512-(strlen(DM_DATA_DIR_PATH)*2));
	//fprintf(stderr,"\nfile_path : \n");
	//for(i=0;i<40;i++)
	//{
	//	fprintf(stderr,"[0x%x]",file_path[i]);
	//}
	fatfs_wfopen ((RMuint16*)file_path, _O_RDONLY, &fd);
	
	if(fd == 0)
	{	
			fprintf(stderr, "Data Manager  : Chk_isKDicType : fopen error \n");
			return -1;
	}
	fatfs_fseek(fd,0, SEEK_END);
	fatfs_ftell(fd, &fsize);
	fatfs_fseek(fd, 0, SEEK_SET);
	if( (err = fatfs_fread( fd, (RMuint8*)&_ExDicPro.tDicH, DOCHEADSZ, &nread )) > 0 )
	{	
		fprintf(stderr, "Data Manager : Doc Head Size : fread error \n");
		fatfs_fclose(fd);
		return -1;
	}
	/*  Is KDIC ? */
	
	value = SwapLong4321(_ExDicPro.tDicH.dwType);
	if(value != 0x4b646963)
	{	
		fprintf(stderr,"NO!! K-DIC :: %s\n",file_path);
		fatfs_fclose(fd);
		return -1;
	}
	fatfs_fclose(fd);
	return 1;

}

void DataManager::LoadFromFile(void)
{
	if(m_Head)
		free(m_Head);
	if(m_Tail)
		free(m_Tail);
	
	m_Head = (DMNode*)malloc(sizeof(DMNode));
	m_Tail = (DMNode*)malloc(sizeof(DMNode));
	m_Head->pNext = m_Tail;
	m_Head->pPrev = m_Tail;
	m_Tail->pNext = NULL;
	m_Tail->pPrev = NULL;
	m_Head->num = 0x00;
	m_Tail->num = 0x00;

	RMuint32 fd = 0;
	int i=0, j=0;
	FATFS_ERROR err;
	FATFS_DIR_ENTRY_INFO* pdir_info;
	char tmp[512];
	uchar_t utmp[512];
	int ret=0;
	
	m_total_index = 0;
	m_count = 0;
	
	pdir_info = new FATFS_DIR_ENTRY_INFO;
	
	err = fatfs_dopen((RMuint8*)DM_DATA_DIR_PATH, &fd);

	if( err == FATFS_ERROR_NO_ERROR ) 
	{
		while( 1 ) 
		{
			err = fatfs_dread( fd, pdir_info);
			if( err != FATFS_ERROR_NO_ERROR ) break;
			if( pdir_info->attributes&ATTR_DIRECTORY ) continue;

			memset(tmp,0,512);
			memset(utmp,0,512);
			ustrcpy(utmp, (uchar_t*)pdir_info->name);	
			memcpy(tmp,pdir_info->name,512);
			//fprintf(stderr,"\npdir_info->name\n");
			//for(i=0; i<20; i++)
			//{
			//	fprintf(stderr,"[0x%x]",pdir_info->name[i]);
			//}
			//fprintf(stderr,"\nufilename\n");
			//for(i=0; i<20; i++)
			//{
			//	fprintf(stderr,"[0x%x]",utmp[i]);
			//}
			//fprintf(stderr,"\nfilename\n");
			//for(i=0; i<20; i++)
			//{
			//	fprintf(stderr,"[0x%x]",tmp[i]);
			//}
			if((ret=Chk_isKDicType(tmp)) == 1)
			{
				Append(tmp,utmp);
			}
		}
	}
	if(!m_total_index)
		_ExDicPro.m_isExist_DicData = false;
	else
		_ExDicPro.m_isExist_DicData = true;

	fatfs_dclose( fd );
	delete(pdir_info);


}
int DataManager::Append(char* filename, uchar_t* ufilename )
{
	DMNode * TmpNode = NULL;
	if(!(TmpNode = (DMNode*)malloc(sizeof(DMNode))))
	{	
		fprintf(stderr, "malloc failed\n");
		return -1;
	}
	if(m_Tail->pPrev== NULL)
	{
		m_Head->pNext = TmpNode;
		TmpNode->pNext = m_Head->pPrev;
		m_Tail->pNext = m_Head;
		m_Tail->pPrev = TmpNode;
		TmpNode ->pPrev = m_Head;
		TmpNode ->num = ++m_count;
		memcpy(TmpNode->file_name, filename, 512);
		ustrcpy(TmpNode->ufile_name, ufilename);
	}
	else
	{
		TmpNode->pNext = m_Tail;
		TmpNode->pPrev = m_Tail->pPrev;
		m_Tail->pPrev->pNext = TmpNode;
		m_Tail->pPrev = TmpNode;
		TmpNode->num = ++m_count;
		memcpy(TmpNode->file_name, filename, 512);
		ustrcpy(TmpNode->ufile_name, ufilename);
	}
	m_total_index++;

	return 0;
}
DMNode * DataManager::Search(int pos, uchar_t* uword)
{
	DMNode * TmpNode;
	TmpNode = m_Head->pNext;
	
	if(ustrcmp(uword,(uchar_t*)"")==0)
	{
		while(TmpNode != m_Tail)
		{
			if(TmpNode->num == pos)
			{
				return TmpNode;
			}

			TmpNode = TmpNode->pNext;
		}
	}
	else
	{
		while(TmpNode != m_Tail)
		{
			if(ustrcmp(TmpNode->ufile_name, uword)==0)
			{
				return TmpNode;
			}

			TmpNode = TmpNode->pNext;
		}
	}
	return NULL;
}
char* DataManager::GetFilename(int index)
{	DMNode * TmpNode;
	TmpNode = Search(index,(uchar_t*)"");
	return TmpNode->file_name;
}
void DataManager::Delete(void)
{
	return ;
}
void DataManager::DrawSelectbar(void)
{
	int y;

	if((m_cur_index%m_visibleline) != 0)
		y = ((m_cur_index%m_visibleline)*m_SelectBar_Height)-m_SelectBar_Height;
	else
		y = (m_visibleline*m_SelectBar_Height)-m_SelectBar_Height;
	
	if(!m_total_index)
	{
		char NoDataExist[20];
		//sprintf( NoDataExist, "%s", "No Dictionary Data" );
		//pT->DrawEUC8Text( NoDataExist, 
		//	0, 120,
		//	(U16)_WHITE, 
		//	(U16)16, 
		//	(U32)DT_TRANSPARENCY|DT_ALIGN_CENTER);
	}	
	else
	{
		DrawBoxWithoutRefresh( HBOX_SELECT_BAR_COLOR, 
								m_SelectBar_Xpos, 
								m_SelectBar_Ypos + y, 
								m_SelectBar_Width,
								m_SelectBar_Height-1 );

		DrawBoxWithoutRefresh( HBOX_SELECT_BAR_SHADOW_COLOR, 
								m_SelectBar_Xpos+1, 
								m_SelectBar_Ypos + y + m_SelectBar_Height-1, 
								m_SelectBar_Width-2,
								1 );
		/* bar top left point */
		DrawBoxWithoutRefresh( HBOX_BASE_COLOR, 
								m_SelectBar_Xpos, 
								m_SelectBar_Ypos+y, 
								1,
								1 );
		/* bar top right point */
		DrawBoxWithoutRefresh( HBOX_BASE_COLOR, 
								m_SelectBar_Xpos+m_SelectBar_Width-1,
								m_SelectBar_Ypos+y,
								1,
								1 );
		/* bar bottom left point */
		DrawBoxWithoutRefresh( HBOX_BASE_COLOR, 
								m_SelectBar_Xpos, 
								m_SelectBar_Ypos+y+m_SelectBar_Height-2,
								1,
								1);
		/* bar bottom right point */
		DrawBoxWithoutRefresh( HBOX_BASE_COLOR, 
								m_SelectBar_Xpos+m_SelectBar_Width-1,
								m_SelectBar_Ypos+y+m_SelectBar_Height-2,
								1,
								1);
	}

	return ;
}

void DataManager::DrawScrollbar(int total, int start, int maxvisible)
{

	m_pScrollbar3->ReDraw(total, start-1, maxvisible);
	return ;
}
void DataManager::DrawGuide( void)
{

	int tw1, tw2, x1, x2;
	int AddSpace;
	int img_w = 24;
	int img_h = 14;
	int img_space = 6;
	int item_space = 11;
	int shift_val = 9;
	int btn_y_pos = m_Ypos + m_Height -img_h - 6;
	
	tw1 = pT->GetStringWidth( (U32)ML_SELECT, 12, 0 );
	tw2 = pT->GetStringWidth( (U32)ML_CANCEL, 12, 0 );
	int start_xpos = (m_Xpos + 5 + m_SelectBar_Width)-(img_w*2+tw1+tw2+img_space*2+item_space);
	x1 = start_xpos + img_w + img_space;
	x2 = x1 + tw1 + item_space + img_w + img_space;

	/* draw select  */
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 0, start_xpos, btn_y_pos, img_w, img_h );
	pT->DrawResText( ML_SELECT, x1, btn_y_pos+2, _WHITE, 12, DT_TRANSPARENCY );			
	/* draw cancel */	
	AddSpace=(x1 + tw1 + item_space);
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 2, AddSpace, btn_y_pos, img_w, img_h );
	pT->DrawResText( ML_CANCEL, x2, btn_y_pos+2, _WHITE, 12, DT_TRANSPARENCY );
	

}
void DataManager::DrawBox(void)
{
	int csx, csy;
		
	DrawBoxWithoutRefresh( DBOX_TITLE_COLOR, m_Xpos+4, m_Ypos, m_Width-8, 1); 
	DrawBoxWithoutRefresh( DBOX_TITLE_COLOR, m_Xpos+4, m_Ypos+1, m_Width-8, 3);
	DrawBoxWithoutRefresh( DBOX_TITLE_COLOR, m_Xpos+1, m_Ypos+2, m_Width-2, m_Title_Height);
	DrawBoxWithoutRefresh( DBOX_TITLE_COLOR, m_Xpos, m_Ypos+4, 1, m_Height-8);
	DrawBoxWithoutRefresh( DBOX_TITLE_COLOR, m_Xpos+m_Width-1, m_Ypos+4, 1, m_Height-8);

	csx = m_Xpos+1;
	csy = m_Ypos+1+m_Title_Height;
		
	DrawBoxWithoutRefresh( DBOX_BASE_COLOR, csx, csy, m_Width-2, m_Height-1-m_Title_Height-4 );
	DrawBoxWithoutRefresh( DBOX_BASE_COLOR, m_Xpos+4, csy+m_Height-1-m_Title_Height-4, m_Width-8, 3 );
	DrawBoxWithoutRefresh( DBOX_BASE_COLOR, m_Xpos+4, csy+m_Height-2-m_Title_Height, m_Width-8, 1 );

	DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 0, m_Xpos, m_Ypos, 4,4 );
	DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 1, m_Xpos+m_Width-4, m_Ypos, 4,4 );
	DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 2, m_Xpos, m_Ypos+m_Height-4, 4,4 );
	DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 3, m_Xpos+m_Width-4, m_Ypos+m_Height-4, 4,4 );

	DrawGuide();
	
	return ;
}
void DataManager::DrawPage(int cur_page, int visibleline)
{
	DMNode * TmpNode;
	int ypos = m_Text_Ypos;
	
	if(cur_page == 1)
		TmpNode = m_Head->pNext;
	else
	{
		int start_pos= ((cur_page-1)*visibleline)+1;
		TmpNode = Search(start_pos, (uchar_t*)"");
	}

	while(TmpNode != m_Tail)
	{
		
		if(TmpNode->num == m_cur_index)
		{
			
			pT->DrawUC16Text( TmpNode->ufile_name, 
								m_Text_Xpos, 
								ypos, 
								_BLACK,
								DBOX_SELECTED_BAR_COLOR,
								12, 
								DT_TRANSPARENCY|DT_SCROLL_SELECTBAR );
			ypos += 22;
			
			TmpNode = TmpNode->pNext;
		}
		else
		{
			
			pT->DrawUC16Text( TmpNode->ufile_name, 
								m_Text_Xpos,
								ypos, 
								_WHITE, 
								DBOX_BASE_COLOR,
								12, 
								DT_TRANSPARENCY|DT_SCROLL_SELECTBAR  );
			ypos += 22;
			
			TmpNode = TmpNode->pNext;

		}
		
		if(((TmpNode->pPrev->num)%m_visibleline) == 0)
			break;
		
		
	}

	return ;
}

int DataManager::DrawTitle(int xpos, int ypos)
{
	pT->DrawResText( ML_LOAD_DIC, xpos, ypos, _WHITE, 12, DT_TRANSPARENCY );
	return 0;
}
int DataManager::Show(void)
{
	op_ClearAreaOSD( 0, 25, 320, 215 );
	DrawBox();
	DrawTitle(m_Title_Xpos,m_Title_Ypos);
	
	if(!(m_total_index <= m_visibleline))
	{	
		DrawScrollbar(m_total_index, m_cur_index, m_visibleline);
	}
	
	DrawSelectbar();
	DrawPage(m_cur_page,m_visibleline);
	pMpegDecoder->RefreshOsd();
	return 0;
}

void DataManager::Counting()
{
return ;

}
void DataManager::Up(void)
{
//	fprintf(stderr,"Up()...idx = [%d] page = [%d]\n", m_cur_index, m_cur_page);
	if(m_cur_index == 1)
	{	return ; }
	else
	{
		if(m_cur_page > 1)
		{
			if((m_cur_index%m_visibleline)==1 && (m_cur_index != 1))
			{	m_cur_page--; }
		}	
		m_cur_index--;
		Show();
	}
	return ;
}
void DataManager::Down()
{
//	fprintf(stderr,"Down()...idx = [%d] page = [%d]\n", m_cur_index, m_cur_page);
	if(m_cur_index == m_total_index)
	{	return ; }
	else
	{
		if((m_visibleline*m_cur_page) <= m_total_index)
		{
			if((m_cur_index%m_visibleline)==0 && (m_cur_index != 1))
			{	m_cur_page++; }
		}	
		m_cur_index++;
		Show();
	}
	return ;
}
void DataManager::Left()
{
return ;
}
void DataManager::Right()
{
return ;
}
int DataManager::strpos(char *buf, char ch)
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

int DataManager::strncpy2(char *dst, char *src, int start, int count)
{
	char *tmp = dst;
	int i;
	
	if((start + count) > strlen(src)) return -1;
		
	src += start;
	for(i = 0; i < count; i++)
		*tmp++ = *src++;
	return 0;
}		

int DataManager::strcmp2(char *s1, char *s2)
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

int DataManager::Load_Pdb(char * filename)
{
	u8				du[8], *key;
	char 			file_path[512];
	char				tmp[512];
	uchar_t 			DicName[128];
	RMuint32 		fd = 0;
	FATFS_ERROR 	err;
	u32 				value;
	u32				dummy, n, i,j, fsize, currpos, nread = 0;

	if(_ExDicPro.tDicI)
	{
		fprintf(stderr,"FREE : DIC INFO STRUCTURE\n");
		free(_ExDicPro.tDicI);
	}
		
	memset(file_path,0,512);
	memset(tmp,0,512);
	sprintf(tmp,"%s",DM_DATA_DIR_PATH);
	for(i=0, j=0; j<strlen(DM_DATA_DIR_PATH);j++)
	{
		file_path[i++] = tmp[j];
		file_path[i++] = 0x00;
	}
	memcpy((file_path+(strlen(DM_DATA_DIR_PATH)*2)),filename,512-(strlen(DM_DATA_DIR_PATH)*2));
	//fprintf(stderr,"\nfile_path : \n");
	//for(i=0;i<40;i++)
	//{
	//	fprintf(stderr,"[0x%x]",file_path[i]);
	//}
	fatfs_wfopen ((RMuint16*)file_path, _O_RDONLY, &fd);
	
	if(fd == 0)
	{	
			fprintf(stderr, "Load_Pdb : fopen error \n");
			return -1;
	}
	memset(_ExDicPro.m_last_filename,0,512);
	memcpy((void*)_ExDicPro.m_last_filename, (void*)file_path,512);
	//fprintf(stderr,"\nlast_filename : \n");
	//for(i=0; i<50;i++)
	//	fprintf(stderr,"[0x%x]",_ExDicPro.m_last_filename[i]);
	//fprintf(stderr,"\nfile_path : \n");
	//for(i=0; i<50;i++)
	//	fprintf(stderr,"[0x%x]",file_path[i]);
	//i=0;

	fatfs_fseek(fd,0, SEEK_END);
	fatfs_ftell(fd, &fsize);
	fatfs_fseek(fd, 0, SEEK_SET);
	if( (err = fatfs_fread( fd, (RMuint8*)&_ExDicPro.tDicH, DOCHEADSZ, &nread )) > 0 )
	{	
		fprintf(stderr, "Load_Pdb : Doc Head Size : fread error \n");
		fatfs_fclose(fd);
		return -1;
	}
	/*  Is KDIC ? */
	
	value = SwapLong4321(_ExDicPro.tDicH.dwType);
	if(value != 0x4b646963)
	{	
		fprintf(stderr,"Not Kdic\n");
		fatfs_fclose(fd);
		return -2;
	}
	
	
	printf("sizeof(struct tDocHeader) = %d[%x]\n", sizeof(struct tDocHeader), sizeof(struct tDocHeader));

	if(nread)
	{
		printf("success file reading\n");
		printf("sName = %s\n", _ExDicPro.tDicH.sName);	// 32 32
		printf("dwUnknown1 = %08x\n",		SwapLong4321(_ExDicPro.tDicH.dwUnknown1)) ;	//  4 36
		printf("dwTime1 = %08x\n", 		SwapLong4321(_ExDicPro.tDicH.dwTime1));		//  4 40
		printf("dwTime2 = %08x\n", 		SwapLong4321(_ExDicPro.tDicH.dwTime2));		//  4 44
		printf("dwTime3 = %08x\n", 		SwapLong4321(_ExDicPro.tDicH.dwTime3));		//  4 48
		printf("dwLastSync = %08x\n", 	SwapLong4321(_ExDicPro.tDicH.dwLastSync));	//  4 52
		printf("ofsSort = %08x\n", 		SwapLong4321(_ExDicPro.tDicH.ofsSort));		//  4 56
		printf("ofsCategories = %08x\n", 	SwapLong4321(_ExDicPro.tDicH.ofsCategories));	//  4 60
		printf("dwCreator = %08x\n", 		SwapLong4321(_ExDicPro.tDicH.dwCreator));	//  'Kdic' or 'Dict'
		printf("dwType = %08x\n", 		SwapLong4321(_ExDicPro.tDicH.dwType));		//  'EJ  ' or 'Kdic'
		printf("dwUnknown2 = %08x\n", 	SwapLong4321(_ExDicPro.tDicH.dwUnknown2));	//  4 72
		printf("dwUnknown3 = %08x\n", 	SwapLong4321(_ExDicPro.tDicH.dwUnknown3));	//  4 76
		printf("wNumRecs = %04x\n", 		SwapWord21(_ExDicPro.tDicH.wNumRecs));		//  2 78 0x4e
		_ExDicPro.reclength = SwapWord21(_ExDicPro.tDicH.wNumRecs) - 1;
	}
	//Index Record End Address
	if( (err = fatfs_fread( fd, (RMuint8*)&dummy, 4, &nread )) > 0 )
	{	
		fprintf(stderr, "Load_Pdb : dummy : fread error \n");
		fatfs_fclose(fd);
		return -1;
	}
	
	if(nread)
		printf("Index End addr = 0x%08x\n", SwapLong4321(dummy));
	// dummy - 6 : Index End position
	_ExDicPro.reclength = ((SwapLong4321(dummy) - 6) - 0x52) / 8;
	printf("Recording Length = %d\n", _ExDicPro.reclength);
	_ExDicPro.tDicI = (struct tDicIndex *)malloc(sizeof(struct tDicIndex) * _ExDicPro.reclength);
	for(i = 0; i < _ExDicPro.reclength; i++)
	{
		if( (err = fatfs_fread( fd, (RMuint8*)&du, 8, &nread )) > 0 )
		{	
			fprintf(stderr, "Load_Pdb : du : fread error \n");
			fatfs_fclose(fd);
			return -1;
		}
		
		_ExDicPro.tDicI[i].dwRecNo = du[0];
		_ExDicPro.tDicI[i].dwRecNo |= du[1] << 8;
		_ExDicPro.tDicI[i].dwRecNo |= du[2] << 16;
		_ExDicPro.tDicI[i].dwRecNo |= du[3] << 24;
		_ExDicPro.tDicI[i].dwIdxpos = du[4];
		_ExDicPro.tDicI[i].dwIdxpos |= du[5] << 8;
		_ExDicPro.tDicI[i].dwIdxpos |= du[6] << 16;
		_ExDicPro.tDicI[i].dwIdxpos |= du[7] << 24;
	}

	for(i = 0; i < _ExDicPro.reclength; i++)
	{
	//	printf("tDicI[%d].dwRecNo  = 0x%08x\n", i, SwapLong4321(_ExDicPro.tDicI[i].dwRecNo) - SIGNATUREPDB);
	//	printf("tDicI[%d].dwIdxpos = 0x%08x\n", i, SwapLong4321(_ExDicPro.tDicI[i].dwIdxpos));
	//	printf("tDicI[%d].size = 0x%04x\n", i, _ExDicPro.tDicI[i].size);
	//	printf("tDicI[%d].startchar = %s\n", i, _ExDicPro.tDicI[i].startchar);
	//	printf("tDicI[%d].endchar = %s\n", i, _ExDicPro.tDicI[i].endchar);

		_ExDicPro.tDicI[i].dwRecNo = SwapLong4321(_ExDicPro.tDicI[i].dwRecNo);
		_ExDicPro.tDicI[i].dwIdxpos = SwapLong4321(_ExDicPro.tDicI[i].dwIdxpos);
	}

	fatfs_fseek(fd, SwapLong4321(dummy) + 16, SEEK_SET);
	// Size 가져오기 : 각 인덱스별 
	for(i = 0; i < _ExDicPro.reclength; i++)
	{
		if( (err = fatfs_fread( fd, (RMuint8*)&du, 2, &nread )) > 0 )
		{	
			fprintf(stderr, "Load_Pdb : du : fread error \n");
			fatfs_fclose(fd);
			return -1;
		}
		
		_ExDicPro.tDicI[i].size = du[0] << 8;
		_ExDicPro.tDicI[i].size |= du[1];
	}
	fatfs_ftell(fd, &currpos);

	key = (u8 *)malloc(_ExDicPro.tDicI[0].dwIdxpos - currpos);
	
	//printf("error3[%d][%d]\n", _ExDicPro.tDicI[0].dwIdxpos, currpos);

	if( (err = fatfs_fread( fd, (RMuint8*)key, _ExDicPro.tDicI[0].dwIdxpos - currpos, &nread )) > 0 )
	{	
		fprintf(stderr, "Load_Pdb : key : fread error \n");
		fatfs_fclose(fd);
		return -1;
	}
	
	//printf("error4\n");
	int charpos = 0;
	for(i = 0; i < _ExDicPro.reclength; i++)
	{
		n = 0;
		memset(_ExDicPro.tDicI[i].startchar, 0, 128);
		memset(_ExDicPro.tDicI[i].endchar, 0, 128);
		while(key[charpos] != 0)
			_ExDicPro.tDicI[i].startchar[n++] = key[charpos++];
		charpos++;
		n = 0;
		while(key[charpos] != 0)
			_ExDicPro.tDicI[i].endchar[n++] = key[charpos++];
		charpos++;
	}	
	free(key);
	fatfs_fclose(fd);

	memset(DicName,0x00,128);
	pT->ConvertEUC8StrToUnicode16Str(DicName, _ExDicPro.tDicH.sName,strlen(_ExDicPro.tDicH.sName));
	
	g_TopPanel->SetTitle((char*)DicName, true);
	g_TopPanel->SetVisible(true);
	g_TopPanel->Update();
	_ExDicPro.m_isLoad_DicData = true;	
	
	return 1;
}

int DataManager::Ok(void)
{
	DMNode * TmpNode = 0;
	int len = 0;
	char tmp[512];
	int ret = 0;

	TmpNode = Search(m_cur_index,(uchar_t*)"");

	if((ret=Load_Pdb(TmpNode->file_name)) == 1)
	{
		fprintf(stderr, "DataManager : OK : pdb loading : [ OK ]\n");
	}
	else if(ret == -1)
	{
		fprintf(stderr, "DataManager : OK : pdb loading : [ Fail ]\n");
		return ret;
	}
	else if(ret == -2)
	{
		fprintf(stderr, "DataManager : OK : Not KDIC\n");
		return ret;
	}
	
	return ret;
}


