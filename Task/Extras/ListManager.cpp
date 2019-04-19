//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : List Manager Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/ListManager.cpp,v $
// $Revision: 1.6 $
// $Date: 2006/03/06 11:31:14 $
// $Author: akaiaphanu $
//
//*****************************************************************************
// $Log: ListManager.cpp,v $
// Revision 1.6  2006/03/06 11:31:14  akaiaphanu
// working
//
// Revision 1.5  2006/03/06 11:18:22  akaiaphanu
// working UI
//
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
// Revision 1.5  2006/02/21 05:04:46  akaiaphanu
// exception message : case not k-dic
//
// Revision 1.4  2006/02/21 02:04:43  akaiaphanu
// working title, list manager
//
// Revision 1.3  2006/02/20 08:06:47  akaiaphanu
// change to array of structure from double linked list
//
// Revision 1.2  2006/02/20 03:04:30  akaiaphanu
// add function for modekey
//
// Revision 1.1  2006/02/20 01:32:57  akaiaphanu
// add function for dictionary
//
//
//*****************************************************************************
#include "ListManager.h"

ListManager::ListManager(int x, int y, int width, int height)
{
	m_VisibleLine = 5;
	
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
	m_ScrollBar_Height = m_SelectBar_Height * m_VisibleLine-3;
	
	m_pScrollbar3 = new CScrollBar3( m_ScrollBar_Xpos,  m_ScrollBar_Ypos, m_ScrollBar_Height,  1, 1, 1 );
	memset(m_taWNameList, 0x00, sizeof(struct WNAME)*1024);
	
	m_WNameList_Idx = 0;
	m_WCount = 0;
	m_Page = 0;
	m_UpCount = 0;
	m_DownCount = 0;
	m_FirstDrawPage = true;
	m_SelectBar_Pos = 0;
	m_Start_Pos = 0;
	m_Draw_Pos_Idx = 0;

}
ListManager::~ListManager(void)
{
	delete(m_pScrollbar3);

}
int ListManager::Init_Member(void)
{
	m_WCount = 0;
	m_WNameList_Idx = 0;
	m_Page = 0;
	m_UpCount = 0;
	m_DownCount = 0;
	m_FirstDrawPage = true;
	m_SelectBar_Pos = 0;
	m_Start_Pos = 0;
	m_Draw_Pos_Idx = 0;

	
	return 0;
}
int ListManager::Load_List(char * pData[], int count)
{
	int i =0;
	Init_Member();
	memset(m_taWNameList, 0x00, sizeof(struct WNAME)*1024);
	
	while(i<count)
	{
		Append(pData[i]);
		i++;
	}
#if 0	
	fprintf(stderr,"List Manager : Load_List :  \n");
	i = 0;
	while(i<m_WNameList_Idx)
	{
		fprintf(stderr,"[%d]Name=%s, Num=%d\n",
			i,m_taWNameList[i].Name,m_taWNameList[i].Num);
		i++;
	}
#endif
	return 0;

}
int ListManager::Load_List2(char **pData1, int count1, char **pData2, int count2 )
{
	int i =0;

	Init_Member();
	
	for(i = 0; i < count1; i++)
		printf("[%d][%s]\n", i, *(pData1 + i));
	for(i = 0; i < count2; i++)
		printf("[%d][%s]\n", i, *(pData2 + i));

	memset(m_taWNameList, 0x00, sizeof(struct WNAME)*1024);
	
	for(i = 1; i <= count1; i++)
	{
		Append(*(pData1 + i));
		//fprintf(stderr,"Load_List2 : [%s]\n",pData1[i]);
	}
	
	for(i = 1; i <= count2; i++)
	{
		Append(*(pData2 + i));
		//fprintf(stderr,"Load_List2 : [%s]\n",pData2[i]);
	}
	
	fprintf(stderr,"Load_List2 : Word Name List : \n");
	i = 0;
	while(i<m_WNameList_Idx)
	{
		//fprintf(stderr,"[%d]Name=%s, Num=%d\n",
		//	i,m_taWNameList[i].Name,m_taWNameList[i].Num);
		i++;
	}

	return 0;

}
int ListManager::Append(char * pData)
{
	if(strcmp(pData,"") != 0)
	{	
		sprintf(m_taWNameList[m_WNameList_Idx].Name,"%s",pData);
		m_taWNameList[m_WNameList_Idx++].Num = m_WCount++;
	}
	else
	{
		fprintf(stderr,"LM : Append : The name is NULL\n");
	}
	return 0;
}
int ListManager::Chk_UpDownCount(void)
{
	m_UpCount = m_Cur_NamePos-1;
	m_DownCount = m_WNameList_Idx - m_Cur_NamePos;

	if(m_UpCount < 50 && m_DownCount < 50) return -4;
	else if(m_UpCount < 50 && m_DownCount > 50) return -1;
	else if(m_UpCount > 50 && m_DownCount < 50) return -2;
	else if(m_UpCount > 50 && m_DownCount > 50) return 1;
		
}
int ListManager::Get_NameList(int direction)
{
	
	return 0;
}
int ListManager::Remake_NameList(int move_value)
{
	if(1)
	{
		;
	}
	else
	{
		;
	}
	return 0;
}
int ListManager::Set_CloseByWord(char* wordname)
{	
	int i =0, value = 0, ret = 0;
	
	
	while(i<m_WNameList_Idx)
	{
		if(strcmp(wordname,m_taWNameList[i].Name)<0)
		{
			fprintf(stderr,"ListManager : Set_CloseByWord : [%d][%s]\n",i,m_taWNameList[i].Name);
			m_Start_Pos = i;
			return i;
		}	
		i++;
	}
	
	return 0;
}
int ListManager::ReCounting(int num)
{
	return 0;
}
char* ListManager::Search(char * word)
{
	int i = 0;
	
	while(i<=m_WNameList_Idx)
	{
		if(strcmp(word,m_taWNameList[i].Name)==0)
		{
			fprintf(stderr,"ListManager : Search : Word Name=[%s]\n",m_taWNameList[i].Name);
			return m_taWNameList[i].Name;
		}	
		i++;
	}
	
	return NULL;
}

int ListManager::GetWord(int index)
{	
	return 0;
}
void ListManager::DrawSelectbar(void)
{
	int y;

	if(m_SelectBar_Pos == 0)
		y = 0;
	else
		y = m_SelectBar_Pos*m_SelectBar_Height;
	
	if(!m_WNameList_Idx)
	{
		char NoDataExist[20];
	}	
	else
	{
		DrawBoxWithoutRefresh( LBOX_SELECT_BAR_COLOR, 
								m_SelectBar_Xpos, 
								m_SelectBar_Ypos + y, 
								m_SelectBar_Width,
								m_SelectBar_Height-1 );

		DrawBoxWithoutRefresh( LBOX_SELECT_BAR_SHADOW_COLOR, 
								m_SelectBar_Xpos+1, 
								m_SelectBar_Ypos + y + m_SelectBar_Height-1, 
								m_SelectBar_Width-2,
								1 );
		/* bar top left point */
		DrawBoxWithoutRefresh( LBOX_BASE_COLOR, 
								m_SelectBar_Xpos, 
								m_SelectBar_Ypos+y, 
								1,
								1 );
		/* bar top right point */
		DrawBoxWithoutRefresh( LBOX_BASE_COLOR, 
								m_SelectBar_Xpos+m_SelectBar_Width-1,
								m_SelectBar_Ypos+y,
								1,
								1 );
		/* bar bottom left point */
		DrawBoxWithoutRefresh( LBOX_BASE_COLOR, 
								m_SelectBar_Xpos, 
								m_SelectBar_Ypos+y+m_SelectBar_Height-2,
								1,
								1);
		/* bar bottom right point */
		DrawBoxWithoutRefresh( LBOX_BASE_COLOR, 
								m_SelectBar_Xpos+m_SelectBar_Width-1,
								m_SelectBar_Ypos+y+m_SelectBar_Height-2,
								1,
								1);
#if 0
		DrawItemWithoutRefresh(	&TAVI_IMAGES[SELECTBAR_EDGE],
									0,
									m_SelectBar_Xpos,
									m_SelectBar_Ypos + y,
									4,
									22 );
		
		DrawItemWithoutRefresh(	&TAVI_IMAGES[SELECTBAR_EDGE],
									1, 
									m_SelectBar_Xpos+m_SelectBar_Width,
									m_SelectBar_Ypos + y,
									4,
									22 );
#endif
	}

	return ;
}

void ListManager::DrawScrollbar(int total, int start, int maxvisible)
{

	m_pScrollbar3->ReDraw(total, start, maxvisible);
	return ;
}
void ListManager::DrawGuide( void)
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
void ListManager::DrawBox(void)
{
	int csx, csy;
		
	DrawBoxWithoutRefresh( LBOX_TITLE_COLOR, m_Xpos+4, m_Ypos, m_Width-8, 1); 
	DrawBoxWithoutRefresh( LBOX_TITLE_COLOR, m_Xpos+4, m_Ypos+1, m_Width-8, 3);
	DrawBoxWithoutRefresh( LBOX_TITLE_COLOR, m_Xpos+1, m_Ypos+2, m_Width-2, m_Title_Height);
	DrawBoxWithoutRefresh( LBOX_TITLE_COLOR, m_Xpos, m_Ypos+4, 1, m_Height-8);
	DrawBoxWithoutRefresh( LBOX_TITLE_COLOR, m_Xpos+m_Width-1, m_Ypos+4, 1, m_Height-8);

	csx = m_Xpos+1;
	csy = m_Ypos+1+m_Title_Height;
		
	DrawBoxWithoutRefresh( LBOX_BASE_COLOR, csx, csy, m_Width-2, m_Height-1-m_Title_Height-4 );
	DrawBoxWithoutRefresh( LBOX_BASE_COLOR, m_Xpos+4, csy+m_Height-1-m_Title_Height-4, m_Width-8, 3 );
	DrawBoxWithoutRefresh( LBOX_BASE_COLOR, m_Xpos+4, csy+m_Height-2-m_Title_Height, m_Width-8, 1 );

	DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 0, m_Xpos, m_Ypos, 4,4 );
	DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 1, m_Xpos+m_Width-4, m_Ypos, 4,4 );
	DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 2, m_Xpos, m_Ypos+m_Height-4, 4,4 );
	DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 3, m_Xpos+m_Width-4, m_Ypos+m_Height-4, 4,4 );

	DrawGuide();
	
	return ;
}

void ListManager::DrawPage(void)
{
	
	int ypos = m_Text_Ypos;
	int LoopCount =0;
	int idx = 0;
	int cur_selectbar_ypos = 0, cur_selectbar_idx=0;
	
	if(m_FirstDrawPage)
	{
		idx = m_Draw_Pos_Idx;
		m_FirstDrawPage = false;
	}
	else if(m_SelectBar_Pos == 0)
	{
		idx = m_Draw_Pos_Idx = m_Start_Pos;
	}
	else if(m_SelectBar_Pos == 4)
	{
		idx = m_Draw_Pos_Idx = m_Start_Pos-4;
	}
	else
	{
		idx = m_Draw_Pos_Idx;
	}
	

	for(LoopCount=0;LoopCount<m_VisibleLine;LoopCount++,idx++ )
	{

		if(m_SelectBar_Pos == LoopCount)
		{
			//fprintf(stderr,"B : SelectBar_Pos[%d]:LoopCount[%d]\n",m_SelectBar_Pos,LoopCount);
			//pT->DrawScrollTxt(0,DIC_SELECTBAR_TEXT_SCROLL_W);
			pT->DrawEUC8Text( m_taWNameList[idx].Name, 
								m_Text_Xpos, ypos, 
								_BLACK,
								LBOX_SELECT_BAR_COLOR,
								12, 
								DT_TRANSPARENCY|DT_SCROLL_SELECTBAR);
			cur_selectbar_ypos = ypos;
			cur_selectbar_idx = idx;
			ypos += 22;			
		}
		else
		{
			//fprintf(stderr,"W : SelectBar_Pos[%d]:LoopCount[%d]\n",m_SelectBar_Pos,LoopCount);
			
			pT->DrawEUC8Text( m_taWNameList[idx].Name, 
								m_Text_Xpos, ypos, 
								_WHITE, 
								LBOX_BASE_COLOR,
								12, 
								DT_TRANSPARENCY|DT_SCROLL_SELECTBAR);
			ypos += 22;
		}
		
	}

	pT->DrawEUC8Text( m_taWNameList[cur_selectbar_idx].Name, 
								m_Text_Xpos,
								cur_selectbar_ypos, 
								_BLACK,
								LBOX_SELECT_BAR_COLOR,
								12, 
								DT_TRANSPARENCY|DT_SCROLL_SELECTBAR);

	return ;
}
int ListManager::DrawTitle(int xpos, int ypos)
{
	char tmp[128];
	memset(tmp, 0, 128);
	
	pT->DrawResText( ML_SUGGESTION, 
					xpos, 
					ypos,
					_WHITE,
					12,
					DT_TRANSPARENCY );
	return 0;
}
int ListManager::Show(void)
{
	op_ClearAreaOSD( 0, 25, 320, 215 );
	if(m_FirstDrawPage)
	{
		if(m_Start_Pos<2)
		{
			m_SelectBar_Pos = 0;
			m_Draw_Pos_Idx = 0;
		}
		else	
		{
			m_Draw_Pos_Idx = m_Start_Pos -2;
			m_SelectBar_Pos = 2;
		}
		
	}
	DrawBox();
	DrawTitle(m_Title_Xpos,m_Title_Ypos);
	if(!(m_WNameList_Idx <= m_VisibleLine))
	{	
		//DrawScrollbar(m_WNameList_Idx, m_Cur_NamePos, m_visibleline);
	}
	DrawSelectbar();
	DrawPage();
	pMpegDecoder->RefreshOsd();
	return 0;
}

int ListManager::ReShow(void)
{
	DrawBox();
	DrawTitle(m_Title_Xpos,m_Title_Ypos);
	DrawSelectbar();
	DrawPage();
	pMpegDecoder->RefreshOsd();
	return 0;
}

void ListManager::Counting()
{
	return ;
}

void ListManager::Up(void)
{
	//fprintf(stderr,"Up()...m_Start_Pos=[%d], m_SelectBar_Pos=[%d]\n",m_Start_Pos, m_SelectBar_Pos);

	if(m_Start_Pos != 0)
	{
		m_Start_Pos--;

		if(m_SelectBar_Pos != 0)
			m_SelectBar_Pos--;
	}
	else
	{	
		;
	}
	ReShow();
	return ;
}

void ListManager::Down(void)
{
	//fprintf(stderr,"Down()...m_Start_Pos=[%d], m_SelectBar_Pos=[%d]\n",m_Start_Pos, m_SelectBar_Pos);

	if(m_Start_Pos != (m_WNameList_Idx-1))
	{
		m_Start_Pos++;

		if(m_SelectBar_Pos != 4)
			m_SelectBar_Pos++;
	}
	else
	{	
		;
	}
	ReShow();
	return ;
}
void ListManager::Left()
{
return ;
}
void ListManager::Right()
{
return ;
}
char* ListManager::Ok(void)
{
	return m_taWNameList[m_Start_Pos].Name;
}


