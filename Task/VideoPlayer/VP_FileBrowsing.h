/*
 * TAVI-PMP Version 1.0
 * new media LIFE CO., LTD.
 * VP_FileBrowsing.h
 * Sang-jin Ryu (ew@newmedialife.com)
 *
 */

#ifndef __VIDEO_PLAYER_FILE_BROWSING_H__
#define __VIDEO_PLAYER_FILE_BROWSING_H__

#define CURRENT_DIR							"."
#define PARENT_DIR							".."
#define FILE_TYPE_DIRECTORY					1
#define FILE_TYPE_REGULAR					2


RMuint32 VPFB_getrootdir(RMint8 *dir, RMuint32 length, void *context);
RMuint32 VPFB_opendir(RMint8 *dir, void *context);
RMuint32 VPFB_readdir(RMuint32 handle, DIR_ENTRY_INFO *pDirInfo, void *context);
RMuint32 VPFB_closedir(RMuint32 handle, void *context);
RMuint32 VPFB_filenameoffset();

RMuint32 VPFB_rendererase(void * context);
//RMuint32 VPFB_rendericon(FILEBROWSER_ICON_TYPE type , RMint32 x,RMint32 y,RMint32 w,RMint32 h,RMuint32 sel,void * context);
RMuint32 VPFB_rendericon( U16 usPictureID, U16 usX, U16 usY, U16 usItemWidth, U16 usItemHeight );
RMuint32 VPFB_rendertext(RMint8 * text,RMint32 x,RMint32 y,RMint32 w,RMint32 h,
                                                                    RMuint32 sel,void * context);
RMuint32 VPFB_renderfile(RMint8 * filename , void * context);
RMuint32 VPFB_isrenderdone(void * context);
RMuint32 VPFB_renderstop(void * context);
RMuint32 VPFB_info(RMint32 msg , void * info, void * context);
RMuint32 VPFB_showsettings(void * context);
RMuint32 VPFB_rendertextwithoutrefresh(RMint8 *text, RMint32 x, RMint32 y, RMint32 w, RMint32 h, RMuint32 colour, void* context);
RMuint32 VPFB_osdrefresh();
RMuint32 VPFB_clearosdwithoutrefresh();



int VP_FileBrowsing(int ssTaskId , char * file_name, unsigned int name_len, int opt);

#endif


