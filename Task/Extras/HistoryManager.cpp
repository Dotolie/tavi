//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : History Manager Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/HistoryManager.cpp,v $
// $Revision: 1.4 $
// $Date: 2006/03/03 05:15:12 $
// $Author: akaiaphanu $
//
//*****************************************************************************
// $Log: HistoryManager.cpp,v $
// Revision 1.4  2006/03/03 05:15:12  akaiaphanu
// working text scroll
//
// Revision 1.3  2006/02/28 04:46:11  akaiaphanu
// working kor,eng filename
//
// Revision 1.2  2006/02/27 02:40:17  akaiaphanu
// working
//
// Revision 1.6  2006/02/22 04:16:28  akaiaphanu
// working selectbar, itemlist
//
// Revision 1.5  2006/02/21 02:04:43  akaiaphanu
// working title, list manager
//
// Revision 1.4  2006/02/20 01:33:56  akaiaphanu
// add ListManager , BitIO
//
// Revision 1.3  2006/02/14 08:37:17  akaiaphanu
// working bottom guide , scrollbar , box
//
// Revision 1.2  2006/02/11 07:19:11  akaiaphanu
// work for dictionary
//
// Revision 1.1  2006/02/06 04:15:29  akaiaphanu
// add to History Manager
//
// 
//
//*****************************************************************************
#include "HistoryManager.h"

HistoryManager::HistoryManager(int x, int y, int width, int height)
{
	m_visibleline = 6;
	
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
	
	m_Head = (HMNode*)malloc(sizeof(HMNode));
	m_Tail = (HMNode*)malloc(sizeof(HMNode));

	m_Head->pNext = m_Tail;
	m_Head->pPrev = m_Tail;
	m_Tail->pNext = NULL;
	m_Tail->pPrev = NULL;
	m_Head->Num = 0x00;
	m_Tail->Num = 0x00;
	
	if(LoadFromFile() == 0) m_history_load = true;
	 
	
}
HistoryManager::~HistoryManager(void)
{
	HMNode * CurNode = m_Head->pNext;
	HMNode * NextNode;
	
	while(CurNode != m_Tail)
	{
		CurNode ->pPrev->pNext = CurNode->pNext;
		CurNode ->pNext->pPrev = CurNode->pPrev;
		NextNode = CurNode->pNext;
		//fprintf(stderr,"HistoryM ->[%d] %s\n", CurNode->Num,CurNode->Data);
		free(CurNode);
		CurNode = NextNode;
	}
	
	free(m_Head);
	free(m_Tail);
	delete(m_pScrollbar3);
}

int HistoryManager::LoadFromFile(void)
{
	char buf[8192];
	char tmp_buf[512];
	RMuint32 fd = 0;
	RMuint32 nread = 0;
	FATFS_ERROR err;
	int i =0, j=0;
	m_total_index = 0;
	m_count = 0;
	
	fprintf(stderr, "\nHistory Manager : LoadFromFile \n");
	
	memset(buf,0,8192);
	memset(tmp_buf,0,512);
	
	fatfs_fopen ((RMuint8*)DIC_HISTORY_DATA_PATH, _O_RDONLY, &fd);
	
	if(fd == 0)
	{
		fprintf(stderr, "LoadFromFile : fopen error\n");
		return -1;
	}
	if( (err = fatfs_fread( fd, (RMuint8*)buf, 1024*8, &nread )) > 0 )
	{	
		if(fd)
		{	
			fprintf(stderr, "LoadFromFile : fread error : nread=[%d]\n", nread);
			fatfs_fclose( fd );
		}
		return -1;
	}
	while(1)
	{
		if((buf[i] == 0x5C))
		{
			strncpy(tmp_buf, buf+j, i-j);
			tmp_buf[i-j]='\0';
			Append(tmp_buf);
			j = i+1;
			//fprintf(stderr, "History loading...[%d], %s\n", m_total_index, tmp_buf);
		}
		if((buf[i] == 0x5C) && (buf[i+1] == 0x5C))
		{	
			break;
		}
		i++;
	}

	fatfs_fclose(fd);
	
	return 0;
}
int HistoryManager::SaveToFile(void)
{
	HMNode * 		TmpNode;
	RMuint32 		fd = 0, nwrite = 0;
	FATFS_ERROR 	err;
	char 			buf[256];
	int 				i =0, j=0;
	int 				size = 0;
	
	fprintf(stderr, "\nHistory Manager : SaveToFile \n");
	fatfs_fopen ((RMuint8*)DIC_HISTORY_DATA_PATH, _O_WRONLY|_O_CREAT, &fd);
	
	if(fd == 0)
	{
		fprintf(stderr, "SaveToFile : fopen error\n");
		return -1;
	}
	TmpNode = m_Head->pNext;
	if(TmpNode != m_Tail)
	{
		while(TmpNode != m_Tail)
		{
			memset(buf,0,256);
			 size = strlen(TmpNode->Data);
			strcpy(buf,TmpNode->Data);
			buf[size] = 0x5C; /* \ */
			//fprintf(stderr,"TmpNode->Data =[%s] : Save buff =[%s]\n", TmpNode->Data, buf);
			
			if( (err=fatfs_fwrite( fd, (RMuint8*)buf, size+1, &nwrite )) != 0 )
			{
				fatfs_fclose(fd);
				return -1;
			}
			
			TmpNode = TmpNode->pNext;
		}
		buf[0] = 0x5C;
		buf[1] = 0x5C;
		if((err=fatfs_fwrite( fd, (RMuint8*)buf, 2, &nwrite )) != 0)
		{
			fatfs_fclose(fd);
			return -1;
		}
	}
	else
	{
		fprintf(stderr,"Not exist to Save Data\n");
	}
	
	fatfs_fclose(fd);
	
	return 0;
}
int HistoryManager::Chk_History(int flags)
{
	switch(flags)
	{
		case 0 : /* load state */
			if(m_history_load == true) return 1;
			else 				     break;

		case 1 : /* load count */
			return m_count;
			
		case 2 :
			break;

		default :
			break;		
	}

	return 0;
}
int HistoryManager::Append(char * pData)
{
	HMNode * TmpNode = NULL;
	if(!(TmpNode = (HMNode*)malloc(sizeof(HMNode))))
	{	
		fprintf(stderr, "Append : malloc failed\n");
		return -1;
	}
	
	m_redundance_delete = Redundance_Delete(pData);
	
	if(m_total_index>DIC_HISTORY_MAX_COUNT)
	{
		Delete();
		ReCounting();
	}
	
	if(m_Tail->pPrev== NULL)
	{
		m_Head->pNext = TmpNode;
		TmpNode->pNext = m_Head->pPrev;
		m_Tail->pNext = m_Head;
		m_Tail->pPrev = TmpNode;
		TmpNode ->pPrev = m_Head;
		TmpNode->Num = ++m_count;
		strcpy(TmpNode->Data, pData);
	}
	else
	{
		TmpNode->pNext = m_Tail;
		TmpNode->pPrev = m_Tail->pPrev;
		m_Tail->pPrev->pNext = TmpNode;
		m_Tail->pPrev = TmpNode;
		TmpNode->Num = ++m_count;
		strcpy(TmpNode->Data, pData);
	}
	m_total_index++;

	if(m_redundance_delete == 1)
	{
		ReCounting();
		m_redundance_delete = 0;
	}
	
	return 0;
}
int HistoryManager::ReCounting(void)
{
	m_count = 0;
	HMNode * TmpNode;
	TmpNode = m_Head->pNext;
	while(TmpNode != m_Tail)
	{
		TmpNode->Num = ++m_count;
		TmpNode = TmpNode->pNext;
	}
	return 0;
}
HMNode * HistoryManager::Search(int pos, char * word)
{
	HMNode * TmpNode;
	TmpNode = m_Head->pNext;
	if(strcmp(word,"")==0)
	{
		while(TmpNode != m_Tail)
		{
			if(TmpNode->Num == pos)
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
			if(strcmp(TmpNode->Data, word)==0)
			{
				return TmpNode;
			}

			TmpNode = TmpNode->pNext;
		}
	}
	return NULL;
}
int HistoryManager::Redundance_Delete(char* word)
{
	HMNode * TmpNode = NULL;
	
	TmpNode = Search(0,word);
	
	if(TmpNode != NULL)
	{
		fprintf(stderr,"Redundance Delete word = %s\n",TmpNode->Data);
		TmpNode->pNext->pPrev = TmpNode->pPrev;
		TmpNode->pPrev->pNext = TmpNode->pNext;
		--m_total_index;
		
		m_cur_index = m_total_index+1;
		m_cur_page = m_total_index/m_visibleline+1;
		
		fprintf(stderr,"total idx=[%d], cur idx=[%d]\n", m_total_index, m_cur_index);
		free(TmpNode);
		return 1;
	}
	else
		return -1;
}
int HistoryManager::Delete(void)
{
	HMNode * TmpNode = NULL;
	TmpNode = m_Head->pNext;
	TmpNode->pNext->pPrev = m_Head;
	m_Head->pNext = TmpNode->pNext;
	fprintf(stderr,"HistoryManager : Delete : %s\n", TmpNode->Data);
	free(TmpNode);
	--m_total_index;
	return 0;
}
int HistoryManager::SetMember(void)
{
	return 0;
}
char* HistoryManager::GetWord(int index)
{	HMNode * TmpNode;
	TmpNode = Search(index,"");
	return TmpNode->Data;
}
void HistoryManager::DrawSelectbar(void)
{
	int y;

	if((m_cur_index%m_visibleline) != 0)
		y = ((m_cur_index%m_visibleline)*m_SelectBar_Height)-m_SelectBar_Height;
	else
		y = (m_visibleline*m_SelectBar_Height)-m_SelectBar_Height;
	
	if(!m_total_index)
	{
		char NoDataExist[20];
		sprintf( NoDataExist, "%s", "No History Data" );
		pT->DrawEUC8Text( NoDataExist, 
			0, 120,
			(U16)_WHITE, 
			(U16)16, 
			(U32)DT_TRANSPARENCY|DT_ALIGN_CENTER);
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

void HistoryManager::DrawScrollbar(int total, int start, int maxvisible)
{

	m_pScrollbar3->ReDraw(total, start-1, maxvisible);
	return ;
}
void HistoryManager::DrawGuide( void)
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
void HistoryManager::DrawBox(void)
{
	int csx, csy;
		
	DrawBoxWithoutRefresh( HBOX_TITLE_COLOR, m_Xpos+4, m_Ypos, m_Width-8, 1); 
	DrawBoxWithoutRefresh( HBOX_TITLE_COLOR, m_Xpos+4, m_Ypos+1, m_Width-8, 3);
	DrawBoxWithoutRefresh( HBOX_TITLE_COLOR, m_Xpos+1, m_Ypos+2, m_Width-2, m_Title_Height);
	DrawBoxWithoutRefresh( HBOX_TITLE_COLOR, m_Xpos, m_Ypos+4, 1, m_Height-8);
	DrawBoxWithoutRefresh( HBOX_TITLE_COLOR, m_Xpos+m_Width-1, m_Ypos+4, 1, m_Height-8);

	csx = m_Xpos+1;
	csy = m_Ypos+1+m_Title_Height;
		
	DrawBoxWithoutRefresh( HBOX_BASE_COLOR, csx, csy, m_Width-2, m_Height-1-m_Title_Height-4 );
	DrawBoxWithoutRefresh( HBOX_BASE_COLOR, m_Xpos+4, csy+m_Height-1-m_Title_Height-4, m_Width-8, 3 );
	DrawBoxWithoutRefresh( HBOX_BASE_COLOR, m_Xpos+4, csy+m_Height-2-m_Title_Height, m_Width-8, 1 );

	DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 0, m_Xpos, m_Ypos, 4,4 );
	DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 1, m_Xpos+m_Width-4, m_Ypos, 4,4 );
	DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 2, m_Xpos, m_Ypos+m_Height-4, 4,4 );
	DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 3, m_Xpos+m_Width-4, m_Ypos+m_Height-4, 4,4 );

	DrawGuide();
	
	return ;
}
void HistoryManager::DrawPage(int cur_page, int visibleline)
{
	HMNode * TmpNode;
	int ypos = m_Text_Ypos;
	int cur_selectbar_ypos = 0;
	int start_pos =0;
	
	if(cur_page == 1)
		TmpNode = m_Head->pNext;
	else
	{
		start_pos= ((cur_page-1)*visibleline)+1;
		TmpNode = Search(start_pos, "");
	}
	
	while(TmpNode != m_Tail)
	{
	//	fprintf(stderr,"Num = %d Data = %s\n", TmpNode->Num, TmpNode->Data);
		
		if(TmpNode->Num == m_cur_index)
		{
			//pT->DrawScrollTxt(0,DIC_SELECTBAR_TEXT_SCROLL_W);
			pT->DrawEUC8Text( TmpNode->Data, 
								m_Text_Xpos,
								ypos, 
								_BLACK,
								HBOX_SELECT_BAR_COLOR,
								12, 
								DT_TRANSPARENCY|DT_SCROLL_SELECTBAR);
			cur_selectbar_ypos = ypos;
			ypos += 22;
			
			TmpNode = TmpNode->pNext;
		}
		else
		{
			pT->DrawEUC8Text( TmpNode->Data, 
								m_Text_Xpos,
								ypos, 
								_WHITE,
								HBOX_BASE_COLOR,
								12, 
								DT_TRANSPARENCY|DT_SCROLL_SELECTBAR );
			ypos += 22;
			
			TmpNode = TmpNode->pNext;

		}
		
		if(((TmpNode->pPrev->Num)%m_visibleline) == 0)
			break;
		
		
	}

	TmpNode = Search(m_cur_index, "");
	pT->DrawEUC8Text( TmpNode->Data, 
								m_Text_Xpos,
								cur_selectbar_ypos, 
								_BLACK,
								HBOX_SELECT_BAR_COLOR,
								12, 
								DT_TRANSPARENCY|DT_SCROLL_SELECTBAR);
	TmpNode = NULL;
	
	return ;
}
int HistoryManager::DrawTitle(int xpos, int ypos)
{
	char tmp[128];
	memset(tmp, 0, 128);
	
	pT->DrawResText( ML_HISTORY, xpos, ypos, _WHITE, 12, DT_TRANSPARENCY );
	return 0;
}
int HistoryManager::Show(void)
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

void HistoryManager::Counting()
{
return ;

}
void HistoryManager::Up(void)
{
	//fprintf(stderr,"Up()...idx = [%d] page = [%d]\n", m_cur_index, m_cur_page);
	if(m_cur_index == 1)
	{	
		m_cur_index = m_total_index;
		if((m_total_index/m_visibleline) == 0)
			m_cur_page = 1;
		else
		{
			if((m_total_index%m_visibleline) == 0)
				m_cur_page = (m_total_index/m_visibleline);

			if((m_total_index%m_visibleline) != 0)
				m_cur_page = (m_total_index/m_visibleline)+1;
		}
	}
	else
	{
		if(m_cur_page > 1)
		{
			if((m_cur_index%m_visibleline)==1 && (m_cur_index != 1))
			{	m_cur_page--; }
		}	
		m_cur_index--;
	}
	Show();
	return ;
}
void HistoryManager::Down()
{
	//fprintf(stderr,"Down()...idx = [%d] page = [%d]\n", m_cur_index, m_cur_page);
	if(m_cur_index == m_total_index)
	{	
		m_cur_index = 1;
		m_cur_page = 1; 
	}
	else
	{
		if((m_visibleline*m_cur_page) <= m_total_index)
		{
			if((m_cur_index%m_visibleline)==0 && (m_cur_index != 1))
			{	m_cur_page++; }
		}	
		m_cur_index++;
	}
	Show();
	return ;
}
void HistoryManager::Left()
{
return ;
}
void HistoryManager::Right()
{
return ;
}
char* HistoryManager::Ok(void)
{
	HMNode * TmpNode;
	TmpNode = Search(m_cur_index,"");
	return TmpNode->Data;
}


