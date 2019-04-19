//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Korean Language Virtual Keyboard
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/vkbd.cpp,v $
// $Revision: 1.2 $
// $Date: 2006/02/27 02:40:17 $
// $Author: akaiaphanu $
//
//*****************************************************************************
// $Log: vkbd.cpp,v $
// Revision 1.2  2006/02/27 02:40:17  akaiaphanu
// working
//
// Revision 1.2  2006/02/11 07:19:11  akaiaphanu
// work for dictionary
//
// Revision 1.1  2006/01/27 05:16:26  akaiaphanu
// added dictionary
//
// 
// 
//
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>

#include "vkbd.h"

unsigned char * buff;

HCODE		t_cho;
HCODE		t_cho2;
HCODE		t_jung;
HCODE		t_jong;
HCODE		t_jong2;

char		oldstatus = 0;

void VirtualKeyboard::HAN_INPUT_KEY_BUFFER_Clear(HAN_INPUT_KEY_BUFFER *p)
{
		if(p->buf != NULL)
		{
				p->p = p->buf;
				*(p->p) = '\0';
		}

		p->cho = 0;
		p->cho2 = 0;
		p->jung = 0;
		p->jong = 0;
		p->jong2 = 0;
		p->status = 0;
}

bool VirtualKeyboard::HAN_INPUT_KEY_BUFFER_Create(HAN_INPUT_KEY_BUFFER *p, int size)
{
		memset(p, 0, sizeof(HAN_INPUT_KEY_BUFFER));
		p->size = size;
	
		p->buf = (char*)malloc(p->size + 1);

		if(p->buf == NULL)
				return false;
	
		p->p = p->buf;
		*(p->p) = '\0';
		
		return true;
}

void VirtualKeyboard::HAN_INPUT_KEY_BUFFER_Delete(HAN_INPUT_KEY_BUFFER *p)
{
		if(strlen(p->buf) >= 1)
				free(p->buf);

		(p->size) = 0;
		(p->p) = p->buf;
}

void VirtualKeyboard::HAN_INPUT_KEY_BUFFER_COMBI_BufPrint(HAN_INPUT_KEY_BUFFER *p, char *strbuf)
{
		char *q;
		HCODE code; 
		
		q = p->buf;
		
		while(*q != '\0')
		{
				*strbuf = *q;
				printf("Q : %x\n", *q);
				strbuf++;
				q++;
		}
												
		code = COMBI_Han_cho_cho2_jung_jong_jong2_to_Hstr(p->cho, p->cho2, p->jung, p->jong, p->jong2);
		printf("code : %x\n", code);
		if((code&0xffff) != 0x8441)
		{
				*strbuf = HCODE_LEFT_CHAR(code);
				strbuf++;
				*strbuf = HCODE_RIGHT_CHAR(code);
				strbuf++;
		}
		
		*strbuf = '\0';
}

HCODE VirtualKeyboard::COMBI_Key_to_HCODE(char key)
{
		typedef struct _ascii_to_han
		{
				HCODE han1;
				HCODE han2;  // SHIFT
				char type;   // jaum(1), moum(2)
		} KEY_BOARD_ASCII_TO_HAN;

		KEY_BOARD_ASCII_TO_HAN keyboard_A_Z[] = {
				{(HCODE)0xa041, 0, 1},							//��		a
				{(HCODE)0x8741, 0, 2},							//��		b
				{(HCODE)0xc041, 0, 1},							//��		c
				{(HCODE)0xb441, 0, 1},							//��		d
				{(HCODE)0x9441, (HCODE)0x9841, 1},	//��,��	e
				{(HCODE)0x9c41, 0, 1},							//��		f
				{(HCODE)0xd041, 0, 1},							//��		g
				{(HCODE)0x85a1, 0, 2},							//��		h
				{(HCODE)0x84a1, 0, 2},							//��		i
				{(HCODE)0x84e1, 0, 2},							//��		j
				{(HCODE)0x8461, 0, 2},							//��		k
				{(HCODE)0x87a1, 0, 2},							//��		l
				{(HCODE)0x8761, 0, 2},							//��		m
				{(HCODE)0x8681, 0, 2},							//��		n
				{(HCODE)0x8481, (HCODE)0x84c1, 2},	//��,��	o
				{(HCODE)0x8541, (HCODE)0x8581, 2},	//��,��	p
				{(HCODE)0xa441, (HCODE)0xa841, 1},	//��,��	q
				{(HCODE)0x8841, (HCODE)0x8c41, 1},	//��,��	r
				{(HCODE)0x9041, 0, 1},							//��		s
				{(HCODE)0xac41, (HCODE)0xb041, 1},	//��,��	t
				{(HCODE)0x8561, 0, 2},							//��		u
				{(HCODE)0xcc41, 0, 1},							//��		v
				{(HCODE)0xb841, (HCODE)0xbc41, 1},	//��,��	w
				{(HCODE)0xc841, 0, 1},							//��		x
				{(HCODE)0x8661, 0, 2},							//��		y
				{(HCODE)0xc441, 0, 1},							//��		z
				{0,  0, 0}
		};

		if('A' <= key && key <= 'Z')
		{
				if(isSHIFT == true)
				{
						isSHIFT = false; 

						if(keyboard_A_Z[key - 'A'].han2 != 0)
								return keyboard_A_Z[key - 'A'].han2; 
						else
								return keyboard_A_Z[key - 'A'].han1;
				}
				else
				{
						return keyboard_A_Z[key - 'A'].han1;
				}
		}
		else // symbol character
		{
				if(isSHIFT == true)
						isSHIFT = false;

				return (key << 8);
		}
}


void VirtualKeyboard::COMBI_Han_DelPreChar(HAN_INPUT_KEY_BUFFER *p)
{
		if(p->p > p->buf + 1)
		{
				if(((*(p->p - 2) & 0x80) != 0) && ((*(p->p - 1) & 0x80) != 0))
				{
						p->p -= 2;
						*(p->p) = '\0';
				}
				else if(((*(p->p - 2) & 0x80) != 0) && ((*(p->p - 1) >= 0x40)))
				{
						p->p -= 2;
						*(p->p) = '\0';
				}
				else
				{
						p->p--;
						*(p->p) = '\0';
				}
		}
		else
		{
				if(p->p == p->buf)
				{
						;
				}
				else
				{
						p->p--;
						*(p->p) = '\0';
				}
		}
}

bool VirtualKeyboard::COMBI_Eng_putch(HAN_INPUT_KEY_BUFFER *p, HCODE ascii)
{
		if(p->size > (int)(strlen(p->buf) + 3))  // memory ok
		{
				*(p->p)=HCODE_LEFT_CHAR(ascii);	p->p++;
				*(p->p)='\0';

				return true;
		}
		else
		{
				return false;
		}
}


HCODE VirtualKeyboard::COMBI_MixJaumCho(HCODE jaum1, HCODE jaum2)
{
		HCODE Table_COMBI_Han_jaumCho[][2] = {
				{(HCODE)0x8841, (HCODE)0x8c41},	// ��+��=��
				{(HCODE)0x9441, (HCODE)0x9841},	// ��+��=��
				{(HCODE)0xa441, (HCODE)0xa841},	// ��+��=��
				{(HCODE)0xac41, (HCODE)0xb041},	// ��+��=��
				{(HCODE)0xb841, (HCODE)0xbc41},	// ��+��=��
				{0,0}
		};
		
		register int i = 0;

		while(Table_COMBI_Han_jaumCho[i][0] != 0)
		{
				if(Table_COMBI_Han_jaumCho[i][0] == jaum1)
				{
						if(Table_COMBI_Han_jaumCho[i][0] == jaum2)
						{
								return Table_COMBI_Han_jaumCho[i][1];
						}
				}
				i++;
		}
		
		return 0;
}


HCODE VirtualKeyboard::COMBI_MixJaumJong(HCODE cho1, HCODE cho2)
{
		HCODE Table_COMBI_Han_jaumJong[][3] = {
				{(HCODE)0x8841, (HCODE)0x8841, (HCODE)0x8443},	// ��+��=��
				{(HCODE)0x8841, (HCODE)0xac41, (HCODE)0x8444},	// ��+��=��
				{(HCODE)0x9041, (HCODE)0xb841, (HCODE)0x8446},	// ��+��=��
				{(HCODE)0x9041, (HCODE)0xd041, (HCODE)0x8447},	// ��+��=��
				{(HCODE)0x9c41, (HCODE)0x8841, (HCODE)0x844a},	// ��+��=��
				{(HCODE)0x9c41, (HCODE)0xa041, (HCODE)0x844b},	// ��+��=��
				{(HCODE)0x9c41, (HCODE)0xa441, (HCODE)0x844c},	// ��+��=��
				{(HCODE)0x9c41, (HCODE)0xac41, (HCODE)0x844d},	// ��+��=��
				{(HCODE)0x9c41, (HCODE)0xc841, (HCODE)0x844e},	// ��+��=��
				{(HCODE)0x9c41, (HCODE)0xcc41, (HCODE)0x844f},	// ��+��=��
				{(HCODE)0x9c41, (HCODE)0xd041, (HCODE)0x8450},	// ��+��=��
				{(HCODE)0xa441, (HCODE)0xac41, (HCODE)0x8454},	// ��+��=��
				{(HCODE)0xac41, (HCODE)0xac41, (HCODE)0x8456},	// ��+��=��
				{0,0,0}
		};
		
		register int i = 0; 
		
		while(Table_COMBI_Han_jaumJong[i][0] != 0)
		{
				if(Table_COMBI_Han_jaumJong[i][0] == cho1)
				{
						if(Table_COMBI_Han_jaumJong[i][1] == cho2)
						{
								return Table_COMBI_Han_jaumJong[i][2];
						}
				}
				i++;
		}
		
		return 0;
}


HCODE VirtualKeyboard::COMBI_Han_Cho_to_Jong(HCODE cho)
{
		HCODE Table_COMBI_Han_ChoJong[][2] = {
				{(HCODE)0x8841, (HCODE)0x8442},	// ��
				{(HCODE)0x8c41, (HCODE)0x8443},	// ��
				{(HCODE)0x9041, (HCODE)0x8445},	// ��
				{(HCODE)0x9441, (HCODE)0x8448},	// ��
				{(HCODE)0x9c41, (HCODE)0x8449},	// ��
				{(HCODE)0xa041, (HCODE)0x8451},	// ��
				{(HCODE)0xa441, (HCODE)0x8453},	// �� 
				{(HCODE)0xac41, (HCODE)0x8455},	// ��
				{(HCODE)0xb041, (HCODE)0x8456},	// ��
				{(HCODE)0xb441, (HCODE)0x8457},	// ��
				{(HCODE)0xb841, (HCODE)0x8458},	// ��
				{(HCODE)0xc041, (HCODE)0x8459},	// ��
				{(HCODE)0xc441, (HCODE)0x845a},	// ��
				{(HCODE)0xc841, (HCODE)0x845b},	// ��
				{(HCODE)0xcc41, (HCODE)0x845c},	// ��
				{(HCODE)0xd041, (HCODE)0x845d},	// ��
				{0, 0}
		};
		
		register int i=0;

		while(Table_COMBI_Han_ChoJong[i][0] != 0)
		{
				if(Table_COMBI_Han_ChoJong[i][0] == cho)
				{
						return Table_COMBI_Han_ChoJong[i][1];
				}
				i++;
		}
		return 0;
}


HCODE VirtualKeyboard::COMBI_Han_cho_cho2_jung_jong_jong2_to_Hstr(HCODE cho, HCODE cho2, HCODE jung, HCODE jong, HCODE jong2)
{
		HCODE code; 
		
		if((cho != 0) && (cho2 != 0)) // merge chosung
		{
				cho = COMBI_MixJaumCho(cho, cho2);
		}
	
		if((jong != 0) && (jong2 != 0)) // merge jongsung
		{
				jong = COMBI_MixJaumJong(jong, jong2);
		}
		else
		{
				jong = COMBI_Han_Cho_to_Jong(jong);
		}

		// 1 xxxxx yyyyy zzzzz  => 1xxx xxyy yyyz zzzz
		// 0x8441=1 00001 00010 00001  => 1000 0100 0100 0001
	
		code = (HCODE)0x8000;
		if(cho != 0) code |= (cho & 0x7c00); else code |= 0x0400;

		if(jung != 0)	code |= (jung & 0x03e0); else code |= 0x0040;

		if(jong != 0)	code |= (jong & 0x001f); else	code |= 0x0001;

		return code;
}


bool VirtualKeyboard::COMBI_Han_putch(HAN_INPUT_KEY_BUFFER *p, HCODE cho, HCODE cho2, HCODE jung, HCODE jong, HCODE jong2)
{
		HCODE code; 
		
		if(p->size > (int)(strlen(p->buf) + 3))  // memory ok
		{
				code = COMBI_Han_cho_cho2_jung_jong_jong2_to_Hstr(cho, cho2, jung, jong, jong2);  // merge cho, jung, jong

				*(p->p) = HCODE_LEFT_CHAR(code);	p->p++;
				*(p->p) = HCODE_RIGHT_CHAR(code);	 p->p++;
				*(p->p) = '\0'; 
				
				return true;
		}
		else
		{
				printf("out of memory!\n");
				return false;
		}
}


HCODE VirtualKeyboard::COMBI_MixDoubleVowel(HCODE jung1, HCODE jung2)
{
		HCODE Table_COMBI_Han_jung[][3] = {
				{(HCODE)0x85a1, (HCODE)0x8461, (HCODE)0x85c1},	//��+��=��
				{(HCODE)0x85a1, (HCODE)0x8481, (HCODE)0x85e1},	//��+��=��
				{(HCODE)0x85a1, (HCODE)0x87a1, (HCODE)0x8641},	//��+��=��
				{(HCODE)0x8681, (HCODE)0x84e1, (HCODE)0x86a1},	//��+��=��
				{(HCODE)0x8681, (HCODE)0x8541, (HCODE)0x86c1},	//��+��=��
				{(HCODE)0x8681, (HCODE)0x87a1, (HCODE)0x86e1},	//��+��=��
				{(HCODE)0x8761, (HCODE)0x87a1, (HCODE)0x8781},	//��+��=��
				{0,0}
		};
		
		register int i = 0;

		while(Table_COMBI_Han_jung[i][0] != 0)
		{
				if(Table_COMBI_Han_jung[i][0] == jung1)
				{
						if(Table_COMBI_Han_jung[i][1] == jung2)
						{
								return Table_COMBI_Han_jung[i][2];
						}
				}
				i++;
		}
		return 0;
}

void VirtualKeyboard::COMBI_Han_Input(HAN_INPUT_KEY_BUFFER *p, HCODE code)
{
	HCODE temp_hcode;

	switch(p->status)
	{
		case 0:	//s0 : ó�� ����------------------------------------------------
			printf("s0 : ó�� ����-----[%x]---------------------------------------\n", COMBI_Han_Get_Type(code));
			switch(COMBI_Han_Get_Type(code))
			{
			case HCODE_JAUM:	//����:
				//������ �Էµ�
				p->cho=code;
				//���� ���·� �̵�
				p->status=1;
				break;
			case HCODE_MOUM:	//����:
				//ó�� ���� ������ �Էµ�
				//�ʼ��� fill cdoe�� ä��� s2 : �ʼ��� �߼��� �ԷµȻ��·� �̵�
				//fill code=00001  1 00001 00010 00001 = 0x8441
				COMBI_Han_putch(p, 0, 0, code, 0, 0);
				break;
			case HCODE_BACKSPACE:	//BackSpace:
				//���� ���� ����
				COMBI_Han_DelPreChar(p);
				///p->cho=0;
				break;
			case HCODE_NEXT:
				//COMBI_Han_putch(p, code, 0, 0, 0, 0);
				p->cho = code;
				//p->status = 1;
				break;
			default:	//��Ÿ����
				//���� �Էµ��ڵ� ���
				COMBI_Eng_putch(p, code);
				///p->cho=0;
				break;
			}
			oldstatus = 0;
			break;
		case 1:	//s1 : �ʼ��� �ԷµȻ���--------------------------------------------------
			printf("s1 : �ʼ��� �ԷµȻ���-[%x]-----------------------------------------------\n", COMBI_Han_Get_Type(code));
			switch(COMBI_Han_Get_Type(code))
			{
			case HCODE_JAUM:	//����:
				//������ �Էµ�
				//�Էµ� �ʼ��� ������,�������̶��
				if(p->cho2!=0)
				{
					//���� ������, �������� �ԷµǾ� ����
					//���� ������, �������� ����ϰ� �Էµ� code�� �ʼ����� ����
					COMBI_Han_putch(p, p->cho, p->cho2, 0, 0, 0);
					p->cho=code;
					p->cho2=0;
					///status=1;
				}
				else
				{
					//�ʼ� 1��(�⺻����)�� �Էµ�
					//�������� �Ǵ°�?(��,��,����,��)
					temp_hcode=COMBI_MixJaumCho(p->cho, code);
					if(temp_hcode!=0)
					{
						//�� ������
						p->cho2=code;
						///status=1;
					}
					else
					{
						//������(����)�� �Ǵ°�?(��,��,��,��,��,��,��,��,��,��,��)
						temp_hcode=COMBI_MixJaumJong(p->cho, code);
						if(temp_hcode!=0)
						{
							//��������
							//���� �ʼ��� �Է°��� �������� ����
							p->jong=p->cho;
							p->jong2=code;
							p->cho=0;
							///p->cho2=0;
							//�������·� �̵�: s3 : �ʼ�, �߼�, ������ �ԷµȻ���(���ڿϼ��ȵ�)
							p->status=3;
						}
						else
						{
							//�� ���� �ȵ�
							//���� �Էµ� ������ ����ϰ� ���� �Է°��� �������� ����
							COMBI_Han_putch(p, p->cho, p->cho2, 0, 0, 0);
							p->cho=code;
							p->cho2=0;
							///status=1;
						}
					}
				}
				break;
			case HCODE_MOUM:	//����:
				//������ �Էµ�
				//���� ������, �������� �ԷµǾ� ����
				p->jung=code;
				//���� ���·� �̵�
				p->status=2;
				break;
			case HCODE_BACKSPACE:	//BackSpace:
				//���� ���� ����
				if(oldstatus != 3) //��强���?�Էµ� ���°� �ƴ϶�� 
				{
					if(p->cho2!=0)
					{
						p->cho2=0;
						p->status=1;
					}
					else
					{
						printf("p->jong = [%x]\n", (HCODE)p->jong);
						p->cho=0;
						p->status=0;
					}
				}
				else
				{
					COMBI_Han_DelPreChar(p);
					p->cho = t_cho ;
					p->cho2 = t_cho2;
					p->jung = t_jung;
					p->jong = t_jong;
					p->jong2 = t_jong2;
					p->status=3;
				}
				break;
			case HCODE_NEXT:
//				COMBI_Han_putch(p, 0, 0, code, 0, 0);

				COMBI_Han_putch(p, p->cho, 0, 0, 0, 0);
				p->cho=0;
				p->cho2=0;

//				COMBI_Han_putch(p, p->cho, 0, 0, 0, 0);
//				p->cho=0;
//				p->cho2 = 0;
				p->status=0;
				break;
			default:	//��Ÿ����
				//�ʼ����
				COMBI_Han_putch(p, p->cho, p->cho2, 0, 0, 0);
				//���� �Էµ��ڵ� ���
				COMBI_Eng_putch(p, code);
				//ó�� ���·� �̵�
				p->cho=0;
				p->status=0;	
				break;
			}
			oldstatus = 1;
			break;
		case 2:	//s2 : �ʼ��� �߼��� �ԷµȻ���--------------------------------------
			printf("s2 : �ʼ��� �߼��� �ԷµȻ���-[%x]-------------------------------------\n", COMBI_Han_Get_Type(code));
			switch(COMBI_Han_Get_Type(code))
			{
				case HCODE_JAUM:	//����:
					//������ �Էµ�(����)
					p->jong=code;
					//���� ���·� �̵�
					p->status=3;
					break;
				case HCODE_MOUM:	//����:
					//������ �Էµ�
					//�θ����� �Ǵ°�?(��, ��, ��, ��, ��, ��, ��)
					temp_hcode=COMBI_MixDoubleVowel(p->jung, code);
					if(temp_hcode==false)
					{
						//�θ����� �ȵȴ�.
						//���� ���� ����ϰ� s2 : �ʼ��� �߼��� �ԷµȻ��·� �̵�
						COMBI_Han_putch(p, p->cho, p->cho2, p->jung, 0, 0);
						p->cho=0;
						p->jung=code;
						///status=2;
					}
					else
					{
						//�θ����� �ȴ�.
						//�θ������� ��ģ��.
						p->jung=temp_hcode;
						///status=2;
					}
					break;
				case HCODE_BACKSPACE:	//BackSpace:
					//���� ����(�߼�) ����
					if(p->cho!=0)
					{
						if(p->cho2 !=0)
						{
							p->jung = 0;
							p->status=1;
						}
						else
						{
							p->cho2 = 0;
							p->jung = 0;
							p->status=1;
						}
					}
					else
					{
						p->cho = 0;
						p->cho2 = 0;
						p->jung=0;
						p->status=0;
					}
					//���� ����(�ʼ��� �Էµ� ����)�� �̵�
					
					break;
				case HCODE_NEXT:
//					COMBI_Han_putch(p, 0, 0, code, 0, 0);
					COMBI_Han_putch(p, p->cho, p->cho2, p->jung, 0, 0);
					p->cho = 0;
					p->cho2 = 0;
					p->jung=0;
					p->status=0;
					break;
				default:	//��Ÿ����
					//�ʼ�,�߼��� ����ϰ� ó�����·� �̵�
					COMBI_Han_putch(p, p->cho, p->cho2, p->jung, 0, 0);
					//���� �Էµ��ڵ� ���
					COMBI_Eng_putch(p, code);
					//ó�� ���·� �̵�
					p->cho=0;
					p->cho2=0;
					p->jung=0;
					p->status=0;	
					break;
			}
			oldstatus = 2;
			break;
		case 3:	//s3 : �ʼ�, �߼�, ������ �ԷµȻ���(���ڿϼ��ȵ�)----------------------
			printf("s3 : �ʼ�, �߼�, ������ �ԷµȻ���(���ڿϼ��ȵ�)-[%x]---------------------\n", COMBI_Han_Get_Type(code));
			switch(COMBI_Han_Get_Type(code))
			{
			case HCODE_JAUM:	//����:
				//������ �Էµ�
				//�������� �ԷµǾ��ִٸ�
				if(p->jong2!=0)
				{
					//������ ���, ���� �Է��� �ʼ�����
					COMBI_Han_putch(p, p->cho, p->cho2, p->jung, p->jong, p->jong2);
					p->cho=code;
					p->cho2=0;
					p->jung=0;
					p->jong=0;
					p->jong2=0;
					//�������·� �̵� : s1 : �ʼ��� �ԷµȻ���
					p->status=1;
				}
				else
				{
					//������ �Ѱ��� �ԷµǾ� �ִ�.
					//��ħ����(������,������)�� �Ǵ°�?(������������������������������������������������������)
					temp_hcode=COMBI_MixJaumJong(p->jong, code);
					if(temp_hcode!=0)
					{
						//��ħ����(������,������)�� ��
						//���������� ����
						p->jong2=code;
						///status=3;
					}
					else
					{
						//��ħ����(������,������) �ȵ� 
						//���� ���� ����� �Էµ� ������ �ʼ��� �Է�
						COMBI_Han_putch(p, p->cho, p->cho2, p->jung, p->jong, 0);
						t_cho = p->cho;
						t_cho2 = p->cho2;
						t_jung = p->jung;
						t_jong = p->jong;
						t_jong2 = p->jong2;
						
						p->cho=code;
						p->cho2=0;
						p->jung=0;
						p->jong=0;
						p->jong2=0;
						//������ �Էµ� ���·� �̵�
						p->status=1;			
					}
				}
				break;
			case HCODE_MOUM:	//����:
				//������ �Էµ�
				//������ 1�� �ԷµǾ� �ִٸ�
				if(p->jong2==0)
				{
					//�ʼ�+�߼�(���), �ʼ�+�߼����� �����.
					COMBI_Han_putch(p, p->cho, p->cho2, p->jung, 0, 0);	
					//�ʼ�, �߼��� �Էµ� ���·�
					p->cho=p->jong;
					p->cho2=0;
					p->jung=code;
					p->jong=0;
					p->jong2=0;
					p->status=2;
				}
				else
				{
					//������,��������
					//�ʼ�+�߼�+���� ���, ����2=>�ʼ�+�߼�
					COMBI_Han_putch(p, p->cho, p->cho2, p->jung, p->jong, 0);
					//�ʼ�, �߼��� �Էµ� ���·�
					p->cho=p->jong2;
					p->cho2=0;
					p->jung=code;
					p->jong=0;
					p->jong2=0;
					p->status=2;
				}
				break;
			case HCODE_BACKSPACE:	//BackSpace:
				//���� ���� ����
				//����2�� ������
				if(p->jong2!=0)
				{
					//����2 ����
					p->jong2=0;
					//�ʼ�, �߼�, ������ �Էµ� ����
					///status=3;
				}
				else
				{
					//���� ����
					p->jong=0;
					//�ʼ�, �߼��� �Էµ� ����
					p->status=2;
				}
				break;
			case HCODE_NEXT:
//				COMBI_Han_putch(p, 0, 0, code, 0, 0);
				COMBI_Han_putch(p, p->cho, p->cho2, p->jung, p->jong, p->jong2);
				p->cho=0;
				p->cho2=0;
				p->jung=0;
				p->jong=0;
				p->jong2=0;
				p->status=0;
				break;
			default:	//��Ÿ����
				//�ʼ�,�߼�, ������ ����ϰ� ó�����·� �̵�
				COMBI_Han_putch(p, p->cho, p->cho2, p->jung, p->jong, p->jong2);
				//���� �Էµ��ڵ� ���
				COMBI_Eng_putch(p, code);
				//ó�� ���·� �̵�
				p->cho=0;
				p->cho2=0;
				p->jung=0;
				p->jong=0;
				p->jong2=0;
				p->status=0;	
				break;
			}
			oldstatus = 3;
			break;
	}
}

int VirtualKeyboard::COMBI_Han_Get_Type(HCODE code)
{
				//��������������������������������������������������������������������������������������������	
				HCODE Table_COMBI_Han_cho[] = {
						(HCODE)0x8442, (HCODE)0x8443, (HCODE)0x8444, (HCODE)0x8445, (HCODE)0x8446, 
						(HCODE)0x8447, (HCODE)0x8448, (HCODE)0x8449, (HCODE)0x844a, (HCODE)0x844b,
						(HCODE)0x844c, (HCODE)0x844d, (HCODE)0x844e, (HCODE)0x844f, (HCODE)0x8450, 
						(HCODE)0x8451, (HCODE)0x8453, (HCODE)0x8454, (HCODE)0x8455, (HCODE)0x8456, 
						(HCODE)0x8457, (HCODE)0x8458, (HCODE)0x8459, (HCODE)0x845a, (HCODE)0x845b, 
						(HCODE)0x845c, (HCODE)0x845d, (HCODE)0x8841, (HCODE)0x8c41, (HCODE)0x9041, 
						(HCODE)0x9441, (HCODE)0x9841, (HCODE)0x9c41, (HCODE)0xa041, (HCODE)0xa441, 
						(HCODE)0xa841, (HCODE)0xac41, (HCODE)0xb041, (HCODE)0xb441, (HCODE)0xb841, 
						(HCODE)0xbc41, (HCODE)0xc041, (HCODE)0xc441, (HCODE)0xc841, (HCODE)0xcc41, 
						(HCODE)0xd041, 0
				};
				
				//�������¤äĤŤƤǤȤɤʤˤ̤ͤΤϤФѤҤ�
				HCODE Table_COMBI_Han_jung[] = {
						(HCODE)0x8461, (HCODE)0x8481, (HCODE)0x84a1, (HCODE)0x84c1, (HCODE)0x84e1, 
						(HCODE)0x8541, (HCODE)0x8561, (HCODE)0x8581, (HCODE)0x85a1, (HCODE)0x85c1, 
						(HCODE)0x85e1, (HCODE)0x8641, (HCODE)0x8661, (HCODE)0x8681, (HCODE)0x86a1, 
						(HCODE)0x86c1, (HCODE)0x86e1, (HCODE)0x8741, (HCODE)0x8761, (HCODE)0x8781, 
						(HCODE)0x87a1, 0
				};
							
						register int i;

						if(code == 0x2400)  // BackSpace
								return HCODE_BACKSPACE;

						if(code == 0x2424) //Next Arrow
								return HCODE_NEXT;
						
						i = 0;
						while(Table_COMBI_Han_cho[i] != 0)
						{
								if(code == Table_COMBI_Han_cho[i])
										return HCODE_JAUM;

								i++;
						}

						i = 0;
						while(Table_COMBI_Han_jung[i] != 0)
						{
								if(code == Table_COMBI_Han_jung[i])
										return HCODE_MOUM;
								
								i++;
						}

						return 0;
}

/*---------------------------------------------------------------------------------
binary search : return index that search to johap and wansung code from hangul table
---------------------------------------------------------------------------------*/
int VirtualKeyboard::Binary_Search(const UINT16 lpTbl[], UINT16 code, INT32 n)
{
		int lower = 0, mid, upper = n - 1; 
		
		while(lower <= upper)
		{
				mid = (upper + lower) / 2; 
				
				if(lpTbl[mid] < code) lower = mid + 1;
				else if(lpTbl[mid] > code) upper = mid - 1;
				else return mid;
		}
	
		return -1;	// no index
}											   

/*------------------------------------------------------------
		johap Code -> wansung Code
-------------------------------------------------------------*/
int VirtualKeyboard::COMBI_to_KS(char *dest, const char *src)
{
		HCODE hcode;
		int   index;

		if((src[0] & 0x80) == 0) // if (NOT (Hangle Code))
		{
				dest[0] = src[0];
				return 1;
		}
	
		// johap Code -> wansung Code
		hcode = STR2HCODE(src[0], src[1]);
		index = Binary_Search(COMBI_Code_Table, hcode, HCODE_N);  // table search

		if (index == -1) hcode = HCODE_JASO_cmb2ks(hcode);
		else			 hcode = KS_Code_Table[index];

		HCODE2STR(dest[0], dest[1], hcode);
		return 2;
}

/*---------------------------------------------------------------------
	change johap type jaso(��, ��, ..., ��, ��, ...) to wansung type jaso
---------------------------------------------------------------------*/
UINT16 VirtualKeyboard::HCODE_JASO_cmb2ks(UINT16  hCode)
{
		if(hCode <= 0x8581) // ��
		{
				if(hCode <= 0x8454)  // ��
				{
						if(hCode <= 0x844a)  // ��
						{
								switch(hCode) 
								{
										case 0x8441 : return 0x2020;  // NULL
										case 0x8442 : return 0xa4a1;  // ��  1 00001 00010 00010
										case 0x8443 : return 0xa4a2;  // ��  1 00001 00010 00011
										case 0x8444 : return 0xa4a3;  // ��  1 00001 00010 00100
										case 0x8445 : return 0xa4a4;  // ��  1 00001 00010 00101
										case 0x8446 : return 0xa4a5;  // ��  1 00001 00010 00110
										case 0x8447 : return 0xa4a6;  // ��  1 00001 00010 00111
										case 0x8448 : return 0xa4a7;  // ��  1 00001 00010 01000
										case 0x8449 : return 0xa4a9;  // ��  1 00001 00010 01001
										case 0x844a : return 0xa4aa;  // ��  1 00001 00010 01010
										default	    : return 0x2020;
								}
						}
						else
						{
								switch(hCode) 
								{
										case 0x844b : return 0xa4ab;  // ��  1 00001 00010 01011
										case 0x844c : return 0xa4ac;  // ��  1 00001 00010 01100
										case 0x844d : return 0xa4ad;  // ��  1 00001 00010 01101
										case 0x844e : return 0xa4ae;  // ��  1 00001 00010 01110
										case 0x844f : return 0xa4af;  // ��  1 00001 00010 01111
										case 0x8450 : return 0xa4b0;  // ��  1 00001 00010 10000
										case 0x8451 : return 0xa4b1;  // ��  1 00001 00010 10001
										case 0x8453 : return 0xa4b2;  // ��  1 00001 00010 10011
										case 0x8454 : return 0xa4b4;  // ��  1 00001 00010 10100
										default	    : return 0x2020;
								}
						}
				}
				else
				{
						if(hCode <= 0x845d) // ��
						{
								switch(hCode) 
								{
										case 0x8455 : return 0xa4b5;  // ��  1 00001 00010 10101
										case 0x8456 : return 0xa4b6;  // ��  1 00001 00010 10110
										case 0x8457 : return 0xa4b7;  // ��  1 00001 00010 10111
										case 0x8458 : return 0xa4b8;  // ��  1 00001 00010 11000
										case 0x8459 : return 0xa4ba;  // ��  1 00001 00010 11001
										case 0x845a : return 0xa4bb;  // ��  1 00001 00010 11010
										case 0x845b : return 0xa4bc;  // ��  1 00001 00010 11011
										case 0x845c : return 0xa4bd;  // ��  1 00001 00010 11100
										case 0x845d : return 0xa4be;  // ��  1 00001 00010 11101
										default	    : return 0x2020;
								}
						}
						else
						{
								switch(hCode) 
								{
										case 0x8461 : return 0xa4bf;  // ��  1 00001 00011 00001
										case 0x8481 : return 0xa4c0;  // ��  1 00001 00100 00001
										case 0x84a1 : return 0xa4c1;  // ��  1 00001 00101 00001
										case 0x84c1 : return 0xa4c2;  // ��  1 00001 00110 00001
										case 0x84e1 : return 0xa4c3;  // ��  1 00001 00111 00001
										case 0x8541 : return 0xa4c4;  // ��  1 00001 01010 00001
										case 0x8561 : return 0xa4c5;  // ��  1 00001 01011 00001
										case 0x8581 : return 0xa4c6;  // ��  1 00001 01100 00001
										default	    : return 0x2020;
								}
						}
				}
		}
		else
		{
				if(hCode <= 0x9041) // ��
				{
						if(hCode <= 0x86c1) // ��
						{
								switch(hCode) 
								{
										case 0x85a1 : return 0xa4c7;  // ��  1 00001 01101 00001
										case 0x85c1 : return 0xa4c8;  // ��  1 00001 01110 00001
										case 0x85e1 : return 0xa4c9;  // ��  1 00001 01111 00001
										case 0x8641 : return 0xa4ca;  // ��  1 00001 10010 00001
										case 0x8661 : return 0xa4cb;  // ��  1 00001 10011 00001
										case 0x8681 : return 0xa4cc;  // ��  1 00001 10100 00001
										case 0x86a1 : return 0xa4cd;  // ��  1 00001 10101 00001
										case 0x86c1 : return 0xa4ce;  // ��  1 00001 10110 00001
										default	    : return 0x2020;
								}
						}
						else
						{
								switch(hCode) 
								{
										case 0x86e1 : return 0xa4cf;  // ��  1 00001 10111 00001
										case 0x8741 : return 0xa4d0;  // ��  1 00001 11010 00001
										case 0x8761 : return 0xa4d1;  // ��  1 00001 11011 00001
										case 0x8781 : return 0xa4d2;  // ��  1 00001 11100 00001
										case 0x87a1 : return 0xa4d3;  // ��  1 00001 11101 00001
										case 0x8841 : return 0xa4a1;  // ��  1 00010 00010 00001
										case 0x8c41 : return 0xa4a2;  // ��  1 00011 00010 00001
										case 0x9041 : return 0xa4a4;  // ��  1 00100 00010 00001
										default	    : return 0x2020;
								}
						}
				}
				else
				{
						if(hCode <= 0xb041) // ��
						{
								switch(hCode) 
								{
										case 0x9441 : return 0xa4a7;  // ��  1 00101 00010 00001
										case 0x9841 : return 0xa4a8;  // ��  1 00110 00010 00001
										case 0x9c41 : return 0xa4a9;  // ��  1 00111 00010 00001
										case 0xa041 : return 0xa4b1;  // ��  1 01000 00010 00001
										case 0xa441 : return 0xa4b2;  // ��  1 01001 00010 00001
										case 0xa841 : return 0xa4b3;  // ��  1 01010 00010 00001
										case 0xac41 : return 0xa4b5;  // ��  1 01011 00010 00001
										case 0xb041 : return 0xa4b6;  // ��  1 01100 00010 00001
										default	    : return 0x2020;
								}
						}
						else
						{
								switch(hCode) 
								{
										case 0xb441 : return 0xa4b7;  // ��  1 01101 00010 00001
										case 0xb841 : return 0xa4b8;  // ��  1 01110 00010 00001
										case 0xbc41 : return 0xa4b9;  // ��  1 01111 00010 00001
										case 0xc041 : return 0xa4ba;  // ��  1 10000 00010 00001
										case 0xc441 : return 0xa4bb;  // ��  1 10001 00010 00001
										case 0xc841 : return 0xa4bc;  // ��  1 10010 00010 00001
										case 0xcc41 : return 0xa4bd;  // ��  1 10011 00010 00001
										case 0xd041 : return 0xa4be;  // ��  1 10100 00010 00001
										default	    : return 0x2020;
								}
						}
				}
		}
}

/*------------------------------------------------------------
	johap code string -> wansung code string
-------------------------------------------------------------*/
void VirtualKeyboard::COMBI_Str_to_KS_Str(char *dest, const char *src)
{
		register int nStep;

		while(*src != '\0')
		{
				nStep = COMBI_to_KS(dest, src);
				dest += nStep;
				src += nStep;
		}
		*dest=0;
}

