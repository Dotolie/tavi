/* the id3v2.3 library.
 * (c)2002 by Samuel Abels (sam@manicsadness.com)
 * This project's homepage is: http://software.manicsadness.com/cantus
 *
 * This library is designed for easyest possible access to id3 V2 tags.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * Added ID3v2.3 or ID3v1 and Renamed
 * Lee Seok<zstein@newmedialife.com>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include "libid3.h"
#include "charset.h"
#include <rmbasic.h>
#include <fatfs.h>

char *g_szGenreList[NUM_GENRES]={
/*1*/	"Blues",
/*2*/	"Classic Rock",
/*3*/	"Country",
/*4*/	"Dance",
/*5*/	"Disco",
/*6*/	"Funk",
/*7*/	"Grunge",
/*8*/	"Hip-Hop",
/*9*/	"Jazz",
/*10*/	"Metal",
/*11*/	"New Age",
/*12*/	"Oldies",
/*13*/	"Other",
/*14*/	"Pop",
/*15*/	"R&B",
/*16*/	"Rap",
/*17*/	"Reggae",
/*18*/	"Rock",
/*19*/	"Techno",
/*20*/	"Industrial",
/*21*/	"Alternative",
/*22*/	"Ska",
/*23*/	"Death Metal",
/*24*/	"Pranks",
/*25*/	"Soundtrack",
/*26*/	"Euro-Techno",
/*27*/	"Ambient",
/*28*/	"Trip-Hop",
/*29*/	"Vocal",
/*30*/	"Jazz+Funk",
/*31*/	"Fusion",
/*32*/	"Trance",
/*33*/	"Classical",
/*34*/	"Instrumental",
/*35*/	"Acid",
/*36*/	"House",
/*37*/	"Game",
/*38*/	"Sound Clip",
/*39*/	"Gospel",
/*40*/	"Noise",
/*41*/	"AlternRock",
/*42*/	"Bass",
/*43*/	"Soul",
/*44*/	"Punk",
/*45*/	"Space",
/*46*/	"Meditative",
/*47*/	"Instrumental Pop",
/*48*/	"Instrumental Rock",
/*49*/	"Ethnic",
/*50*/	"Gothic",
/*51*/	"Darkwave",
/*52*/	"Techno-Industrial",
/*53*/	"Electronic",
/*54*/	"Pop-Folk",
/*55*/	"Eurodance",
/*56*/	"Dream",
/*57*/	"Southern Rock",
/*58*/	"Comedy",
/*59*/	"Cult",
/*60*/	"Gangsta",
/*61*/	"Top 40",
/*62*/	"Christian Rap",
/*63*/	"Pop/Funk",
/*64*/	"Jungle",
/*65*/	"Native American",
/*66*/	"Cabaret",
/*67*/	"New Wave",
/*68*/	"Psychadelic",
/*69*/	"Rave",
/*70*/	"Showtunes",
/*71*/	"Trailer",
/*72*/	"Lo-Fi",
/*73*/	"Tribal",
/*74*/	"Acid Punk",
/*75*/	"Acid Jazz",
/*76*/	"Polka",
/*77*/	"Retro",
/*78*/	"Musical",
/*79*/	"Rock & Roll",
/*80*/	"Hard Rock",
// WinAmp extensions:
/*81*/	"Folk",
/*82*/	"Folk-Rock",
/*83*/	"National Folk",
/*84*/	"Swing",
/*85*/	"Fast Fusion",
/*86*/	"Bebob",
/*87*/	"Latin",
/*88*/	"Revival",
/*89*/	"Celtic",
/*90*/	"Bluegrass",
/*91*/	"Avantgarde",
/*92*/	"Gothic Rock",
/*93*/	"Progressive Rock",
/*94*/	"Psychedelic Rock",
/*95*/	"Symphonic Rock",
/*96*/	"Slow Rock",
/*97*/	"Big Band",
/*98*/	"Chorus",
/*99*/	"Easy Listening",
/*100*/	"Acoustic",
/*101*/	"Humour",
/*102*/	"Speech",
/*103*/	"Chanson",
/*104*/	"Opera",
/*105*/	"Chamber Music",
/*106*/	"Sonata",
/*107*/	"Symphony",
/*108*/	"Booty Bass",
/*109*/	"Primus",
/*110*/	"Porn Groove",
/*111*/	"Satire",
/*112*/	"Slow Jam",
/*113*/	"Club",
/*114*/	"Tango",
/*115*/	"Samba",
/*116*/	"Folklore",
/*117*/	"Ballad",
/*118*/	"Power Ballad",
/*119*/	"Rhythmic Soul",
/*120*/	"Freestyle",
/*121*/	"Duet",
/*122*/	"Punk Rock",
/*123*/	"Drum Solo",
/*124*/	"A capella",
/*125*/	"Euro-House",
/*126*/	"Dance Hall",
/*127*/	"Goa",
/*128*/	"Drum & Bass",
/*129*/	"Club-House",
/*130*/	"Hardcore",
/*131*/	"Terror",
/*132*/	"Indie",
/*133*/	"BritPop",
/*134*/	"Negerpunk",
/*135*/	"Polsk Punk",
/*136*/	"Beat",
/*137*/	"Christian Gansta Rap",
/*138*/	"Heavy Metal",
/*139*/	"Black Metal",
/*140*/	"Crossover",
/*141*/	"Contemporary Christian",
/*142*/	"Christian Rock",
/*143*/	"Merengue",
/*144*/	"Salsa",
/*145*/	"Thrash Metal",
/*146*/	"Anime",
/*147*/	"Jpop",
/*148*/	"Synthpop"
};


/***************************************************************************************
 * BELOW FOLLOW THE STATICS
 ***************************************************************************************/
// Return the last item of an doubly linked list
static DLL *
dll_last (DLL *list)
{
	if( list == NULL )
		return (NULL);
	
	while ( list->next != NULL )
		list = (DLL*)list->next;
	
	return (list);
}

// Append an item to the doubly linked list
static DLL *
dll_append (DLL *list, void *data)
{
	DLL *item = ( DLL* )malloc (sizeof(DLL));
	DLL *lastitem = dll_last(list);
	
	item->data = (DLL*)data;
	item->next = NULL;
	
	if ( lastitem == NULL )
	{
		item->prev = NULL;
		return (item);
	}
	else
	{
		item->prev = lastitem;
		lastitem->next = item;
	}

	return list;
}
  
static DLL *
dll_remove (DLL *list, void *data)
{
	DLL *item = list;
	
	while ( item )
	{
		if ( item->data == data )
		{
			if ( item->prev == NULL 
				&& item->next == NULL )
			{
// No other items there? Then return a zero pointer.
				free (item);
				return (NULL);
			}
			if ( item->prev == NULL )
			{
// remove the first item of the list here...
				list = item->next;
				list->prev = NULL;
				free (item);
				
				break;
			}
			if ( item->next == NULL )
			{
// ...remove the last item of the list here...
				((DLL*)(item->prev))->next = NULL;
				free (item);
				
				break;
			}
// ...or other items here
			((DLL*)(item->prev))->next = item->next;
			((DLL*)(item->next))->prev = item->prev;
			free (item);
			
			break;
		}
		item = item->next;
	}
	
	return list;
}

// Free a doubly linked list
static DLL *
dll_free (DLL *list)
{
	DLL *item = list;
	DLL *current = NULL;
	
	while (item)
	{
		current = item;
		item = item->next;
		
		free (current);
	}
	
	return NULL;
}

/*
 * Converts all occurences of a CR/LF to LF
 */
static void
crlf2cr (char *source)
{
	char *psource = source;
	char destination[2048];

	if(source != NULL)
	{
		memset (destination, 0, 2048);
		for (psource = source; *psource != '\0'; psource++)
		{
			if(*psource == 13
				&& *(psource+1) == 10 )
			{
				psource++;
			}
			destination[strlen(destination)] = *psource;
		}
	}
	
	strncpy (source, destination, strlen(destination)+1);
}


/*
 * Converts all occurences of a LF to CR/LF
 */
static void
cr2crlf (char *source)
{
	char *psource = source;
	char destination[2048];
	
	if( source != NULL )
	{
		memset (destination, 0, 2048);
		for (psource = source; *psource != '\0'; psource++)
		{
			if (*psource == 10)
				destination[strlen(destination)] = 13;
			destination[strlen(destination)] = *psource;
		}
	}
	
	strncpy (source, destination, strlen(destination)+1);
}




/*
 * Reads the first ten bytes of an file and checks, if it's a valid ID3 V2.3 file
 * If it is, the header flags are stored in the tag struct.
 * Returns TRUE on a valid header, otherwise FALSE.
 */
static short int
//check_header (FILE *mp3file, id3v2Tag *v2)
check_header (int mp3file, id3v2Tag *v2)
{
	unsigned char buf[10];
	FATFS_ERROR sdFatfsErr;
	RMuint32 nBytes;
	
// get header (=first ten bytes of the file)
	//fseek (mp3file, 0, SEEK_SET);
	//if ( fread (buf, 1, 10, mp3file) < 10 )
	//	return (FALSE);
	fatfs_fseek( (RMuint32)mp3file, 0, SEEK_SET );
	sdFatfsErr = fatfs_fread( (RMuint32)mp3file, (RMuint8*)buf, sizeof(buf), &nBytes );
	if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) {
		return FALSE;
	}

// a valid tag must begin with "ID3" followed by the version (checked below)
// followed by a flag byte, where the last fife bytes are unused and must be FALSE
	if ( memcmp(buf, "ID3", 3) != 0
		|| (buf[5] & 31) != 0 )
		return (FALSE);

// check if version is supported
  // Support ID3 v2.2
	if ( (buf[3] != 2 && buf[3] != 3 && buf[3] != 4) || buf[4] != 0 )
		return (FALSE);
	v2->ver_major = buf[3];
	v2->ver_minor = buf[4];

// The next thing to come is the tag size. This are 4 bytes, the MSB should always be set to zero. check!
	if ( (buf[6] & 128) != 0
		|| (buf[7] & 128) != 0
		|| (buf[8] & 128) != 0
		|| (buf[9] & 128) != 0 )
		return (FALSE);
	
// The tag size is encoded to be syncsave, so I got to decode it.
// The tag size is the size of the complete tag EXCLUDING the 10-byte header.
 	v2->tag_size = buf[9] + (buf[8] << 7) + (buf[7] << 14) + (buf[6] << 21);
	v2->tag_size += 10;

// ok, so were save. put the flags in the nicer struct.
	v2->unsync = (buf[5] & 128) >> 7;
	v2->has_extheader = (buf[5] & 64) >> 6;
	v2->is_experimental = (buf[5] & 32) >> 5;
	
	return (TRUE);
}





/*
 * Reads the extheader of a valid ID3V2.3 file and checks, if it's a valid.
 * If it is, the extheader flags are stored in the tag struct.
 * Returns TRUE on a valid extheader, otherwise FALSE.
 */
static short int
//check_extheader (FILE *mp3file, id3v2Tag *v2)
check_extheader (int mp3file, id3v2Tag *v2)
{
	unsigned char buf[10];
	FATFS_ERROR sdFatfsErr;
	RMuint32 nBytes;

// Read id3 extheader intro (5 bytes)
	//fseek (mp3file, 10, SEEK_SET);
	//if ( fread(buf, 1, 5, mp3file) < 5 )
	//	return (FALSE);
	fatfs_fseek( (RMuint32)mp3file, 0, SEEK_SET );
	sdFatfsErr = fatfs_fread( (RMuint32)mp3file, (RMuint8*)buf, sizeof(buf), &nBytes );
	if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) {
		return FALSE;
	}
	
// First comes the extheader size. This are 4 bytes, the MSB should always be set to zero. check!
	if( (buf[0] & 128) != 0
		|| (buf[1] & 128) != 0
		|| (buf[2] & 128) != 0
		|| (buf[3] & 128) != 0 )
		return (FALSE);
	
// OK. In ID3V2.3 only six byte or ten byte extheaders are allowed.
	if( v2->extheader_size != 6 
		&& v2->extheader_size != 10 )
		return (FALSE);
	
// The first four bytes specify the extheader size.
	v2->extheader_size = buf[3] + (buf[2] << 7) + (buf[1] << 14) + (buf[0] << 21);
	
// The fifth byte specifies extendened flags. (in fact, only one flag is used for ID3V2.3
// The MSB of the byte 5 specifies, if there is CRC data to come, appended to the extheader.
	if( (buf[4] & 127) != 0
		|| buf[5] != 0 )
		return (FALSE);
	
	v2->crc_data_present = (buf[4] & 128) >> 7;
	
// if crc data is present, the extheader size must be ten bytes, otherwise 6.
	if ( (v2->extheader_size == 6 && v2->crc_data_present == TRUE)
		|| (v2->extheader_size == 10 && v2->crc_data_present == FALSE) )
		return (FALSE);
	
// now come four bytes specifying the padding size
	if ( (buf[6] & 128) != 0
		|| (buf[7] & 128) != 0
		|| (buf[8] & 128) != 0
		|| (buf[9] & 128) != 0 )
		return (FALSE);
	
	v2->padding_size = buf[9] + (buf[8] << 7) + (buf[7] << 14) + (buf[6] << 21);
	
// Now to the optional crc data.
	if( v2->crc_data_present )
	{
		//if( fread (buf, 1, 4, mp3file) < 4 )
		//	return (FALSE);
		sdFatfsErr = fatfs_fread( (RMuint32)mp3file, (RMuint8*)buf, 4, &nBytes );
		if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) {
			return FALSE;
		}
		
		memcpy (v2->crc_data, buf, 4);
	}
	
	return (TRUE);
}

/* ++

  read_frames_v2_2

Description:
  Parase ID3 tag V2.2
  
-- */
static short int
read_frames_v2_2 (int mp3file, id3v2Tag *v2)
{
	unsigned char buf[10];
	int numframes = 0;
	unsigned int totalframesize = 0;
	id3v2Frame *frame = NULL;
	RMuint32 nCurPos, nBytes;
	
// set the position to the first frame header (header = 10 bytes + extheadersize + 4 bytes "extheaderheader")
	if (v2->has_extheader)
		fatfs_fseek( (RMuint32)mp3file, 10 + v2->extheader_size + 4, SEEK_SET );
	else
		fatfs_fseek( (RMuint32)mp3file, 10, SEEK_SET );

// If the tag size is too small for frames, return with an error.
	fatfs_ftell( (RMuint32)mp3file, &nCurPos );
	if ( ((v2->tag_size + 10) - v2->padding_size) <= nCurPos ) {
		return FALSE;
	}
// now read all the frames
	numframes = 0;
	v2->frames = NULL;
	while( 1 )
	{
		fatfs_ftell( (RMuint32)mp3file, &nCurPos );
		if( nCurPos >= ((v2->tag_size + 6) - v2->padding_size) ) break;
		frame = ( id3v2Frame* )calloc (1, sizeof(id3v2Frame));

// the frame header is ten bytes long
		fatfs_fread( (RMuint32)mp3file, buf, 6, &nBytes );
		if( nBytes < 6 ) {
		  goto error;
		}

// if we are already in the padding, we must no longer look for frames...
		if ( buf[0] == 0 && buf[1] == 0 && buf[2] ==  0 )
		{
			if (numframes == 0) {
				goto error;
			}
			
			free (frame);
			break;
		}
		
// first come four characters identifying the frame. It must be alphanumeric.
		if ( !isalnum(buf[0]) || !isalnum(buf[1]) || !isalnum(buf[2])) {
			goto error;
		}
		
		*(frame->id) = buf[0];
		*(frame->id + 1) = buf[1];
		*(frame->id + 2) = buf[2];
		
		frame->datasize = buf[3]<<16 | buf[4]<<8 | buf[5];
		
// A minimum size must be present!
		if ( frame->datasize <= 0 ) {
			goto error;
		}
		
// ok, we are done with the frame header, so now we read the frame data.
		frame->data = ( char* )calloc (1, frame->datasize + 1);
		if( fatfs_fread(mp3file, (RMuint8*)frame->data, frame->datasize, (RMuint32*)&nBytes) != FATFS_ERROR_NO_ERROR ) {
			goto error;
		}
		
		numframes++;
		totalframesize += 6 + frame->datasize;
		
// we append it to a glist, which is appended to the v2 struct.
		v2->frames = dll_append (v2->frames, frame);

		if( totalframesize >= (v2->tag_size-10) ) {
		  break;
		}
	}

// if we have no extheader, that means, we don't know how much padding we have!
// thus, i calculate it here.
	if ( !v2->has_extheader )
		v2->padding_size = v2->tag_size - totalframesize;
	
// A minimum of one frame is mandatory.
	if( numframes > 0 )
		return (TRUE);
	
error:
// cleanups in case of an error.
	if( frame && frame->data )
		free (frame->data);
	if( frame )
		free (frame);
	return (FALSE);
}



/*
 * Reads the complete frames of a valid ID3V2.3 file and checks, if they are valid.
 * If they are, the flags are stored in a DLL and appended to the tag struct.
 * Returns TRUE on success, otherwise FALSE.
 */
static short int
read_frames_v2_3 (int mp3file, id3v2Tag *v2)
{
	unsigned char buf[10];
	int numframes = 0;
	unsigned int totalframesize = 0;
	id3v2Frame *frame = NULL;
	RMuint32 nCurPos, nBytes;
	
// set the position to the first frame header (header = 10 bytes + extheadersize + 4 bytes "extheaderheader")
	if (v2->has_extheader)
		fatfs_fseek( (RMuint32)mp3file, 10 + v2->extheader_size + 4, SEEK_SET );
	else
		fatfs_fseek( (RMuint32)mp3file, 10, SEEK_SET );

// If the tag size is too small for frames, return with an error.
	fatfs_ftell( (RMuint32)mp3file, &nCurPos );
	if ( ((v2->tag_size + 10) - v2->padding_size) <= nCurPos )
		return FALSE;
// now read all the frames
	numframes = 0;
	v2->frames = NULL;
	while( 1 )
	{
		fatfs_ftell( (RMuint32)mp3file, &nCurPos );
		if( nCurPos >= ((v2->tag_size + 10) - v2->padding_size) ) break;
		frame = ( id3v2Frame* )calloc (1, sizeof(id3v2Frame));

// the frame header is ten bytes long
		fatfs_fread( (RMuint32)mp3file, buf, 10, &nBytes );
		if( nBytes < 10 ) {
		  goto error;
		}

// if we are already in the padding, we must no longer look for frames...
		if ( buf[0] == 0 && buf[1] == 0 && buf[2] ==  0 && buf[3] == 0 )
		{
			if (numframes == 0) {
				goto error;
			}
			
			free (frame);
			break;
		}
		
// first come four characters identifying the frame. It must be alphanumeric.
		if ( !isalnum(buf[0]) || !isalnum(buf[1]) || 
		     !isalnum(buf[2]) || !isalnum(buf[3]) ) {
			goto error;
		}
		
		*(frame->id) = buf[0];
		*(frame->id + 1) = buf[1];
		*(frame->id + 2) = buf[2];
		*(frame->id + 3) = buf[3];
		
// then, the frame size is to come. Again, the four MSBs must be zero.
		if ( (buf[4] & 128) != 0
			|| (buf[5] & 128) != 0
			|| (buf[6] & 128) != 0
			|| (buf[7] & 128) != 0 ) {
			goto error;
		}
		
		frame->datasize = buf[7] + (buf[6] << 7) + (buf[5] << 14) + (buf[4] << 21);
		
// A minimum size must be present!
		if ( frame->datasize <= 0 ) {
			goto error;
		}
		
// The two following frame header flags have the 5 LSBs not set.
		if ( (buf[8] & 31) != 0
			|| (buf[9] & 31) != 0 ) {
			goto error;
		}

// now, put the flags in the struct.
		frame->tagalter = (buf[8] & 128) >> 7;
		frame->filealter = (buf[8] & 64) >> 6;
		frame->readonly = (buf[8] & 32) >> 5;
		
		frame->compression = (buf[9] & 128) >> 7;
		frame->encryption = (buf[8] & 64) >> 6;
		frame->grouping = (buf[8] & 32) >> 5;
		
// ok, we are done with the frame header, so now we read the frame data.
		frame->data = ( char* )calloc (1, frame->datasize + 1);
		if( fatfs_fread(mp3file, (RMuint8*)frame->data, frame->datasize, (RMuint32*)&nBytes) != FATFS_ERROR_NO_ERROR ) {
			goto error;
		}
		
		numframes++;
		totalframesize += 10 + frame->datasize;
		
// we append it to a glist, which is appended to the v2 struct.
		v2->frames = dll_append (v2->frames, frame);
	}

// if we have no extheader, that means, we don't know how much padding we have!
// thus, i calculate it here.
	if ( !v2->has_extheader )
		v2->padding_size = v2->tag_size - totalframesize;
	
// A minimum of one frame is mandatory.
	if( numframes > 0 )
		return (TRUE);
	
error:
// cleanups in case of an error.
	if( frame && frame->data )
		free (frame->data);
	if( frame )
		free (frame);
	return (FALSE);
}

/*
 * Reads all tag information of a valid ID3V2.3 file.
 * When successful, the info is stored in the tag struct.
 * Returns:
 * 0 = success
 * 1 = Cannot open file.
 * 2 = No header or wrong version.
 * 3 = broken extheader.
 * 4 = broken frames.
 */
static int
get_id3v2tag_raw (id3v2Tag *v2, const uchar_t *filename, int unicode )
{
	int mp3file;
	FATFS_ERROR sdFatfsErr;
	int error = 0;

// open file
	error = 1;
	sdFatfsErr = fatfs_wfopen( (RMuint16*)filename, _O_RDONLY, (RMuint32*)&mp3file );
	if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) {
		printf( "get_id3v2tag_raw: 0x%x\n", sdFatfsErr );
		goto done;
	}
	
// check/get header
	error = 2;
	if( !check_header (mp3file, v2) )
		goto done;
// check/get extheader
	error = 3;
	if( v2->has_extheader
		&& !check_extheader (mp3file, v2) )
		goto done;

// get the content frames
	error = 4;
	if( v2->ver_major == 2 ) {
	  if( !read_frames_v2_2(mp3file, v2) )
  		goto done;
	}
	else if( v2->ver_major == 3 || v2->ver_major == 4 ) {
  	if( !read_frames_v2_3(mp3file, v2) )
  		goto done;
  }
  else {
    goto done;
  }

	error = 0;
done:
	//close (mp3file);
	sdFatfsErr =fatfs_fclose( (RMuint32)mp3file );
	if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) {
		printf( "get_id3v2tag_raw flie close: 0x%x \n", sdFatfsErr );
	}
	return (error);
}
	
/*
 * Find one frames data and give bag its data in the correct format.
 * Returns TRUE on success, otherwise FALSE;
 */
static short int
frame_find (id3v2Tag *v2, char *name, char *value)
{
	DLL *curframe = NULL;
	id3v2Frame *frame = NULL;
	
// we parse through the whole list of frames, giving back the correct frame value.
	curframe = v2->frames;
	while ( curframe )
	{
		frame = (id3v2Frame *)curframe->data;

// Just to be sure...
		if( frame->datasize <= 0 )
			goto nextframe;
		
// Matches the users request? Otherwise try the next frame.
		if( memcmp (frame->id, name, 4) != 0 )
			goto nextframe;
		
// This types don't need much change, just give the whole data back to the user according to the encoding.
// The first byte is the encoding.
// TPE1: Artist
// TIT2: Song Title
// TALB: Album Title
// TYER: Year
// TRCK: Track
// TCON: Genre
// COMM: Comment
  if( v2->ver_major == 3 || v2->ver_major == 4 ) { // ver 2.3 or 2.4
		if ( memcmp (frame->id, "TPE1", 4) == 0
			|| memcmp (frame->id, "TIT2", 4) == 0
			|| memcmp (frame->id, "TALB", 4) == 0
			|| memcmp (frame->id, "TYER", 4) == 0
			|| memcmp (frame->id, "TRCK", 4) == 0
			|| memcmp (frame->id, "TCON", 4) == 0
			|| memcmp (frame->id, "TALB", 4) == 0 )
		{
			if ( *frame->data == 0 ) {
				memcpy(value, frame->data + 1, frame->datasize - 1);
			}
			if ( *frame->data == 1 ) {
				char nulltermvalue[frame->datasize];
				char *isovalue = NULL;
			
// the tag is not null terminated, so i have to create a null terminated string first.
				memset( value, 0, frame->datasize );
				memcpy (value, frame->data + 1, frame->datasize - 1);
			}
			
// change linefeeds to a single "return" key.
			crlf2cr (value);
			return (TRUE);
		}
	}
	else if( v2->ver_major == 2 ) {
	  if ( memcmp (frame->id, "TP1", 3) == 0
			|| memcmp (frame->id, "TAL", 3) == 0
			|| memcmp (frame->id, "TCO", 3) == 0
			|| memcmp (frame->id, "TT2", 3) == 0 )
		{
			if ( *frame->data == 0 ) {
				memcpy(value, frame->data + 1, frame->datasize - 1);
			}
			if ( *frame->data == 1 ) {
				char nulltermvalue[frame->datasize];
				char *isovalue = NULL;
			
// the tag is not null terminated, so i have to create a null terminated string first.
				memset( value, 0, frame->datasize );
				memcpy (value, frame->data + 1, frame->datasize - 1);
			}
			
// change linefeeds to a single "return" key.
			crlf2cr (value);
			return (TRUE);
		}
	}
		
// The comment requires special handling.
// Its data has: One byte "encoding" (0 = ISO-8859-1, 1 = UNICODE)
// followed by the language (three bytes, e.g. "eng"),
// followed by a short description,
// then a NULL,
// and the full description
// By now, i simply drop the short description
		if( memcmp(frame->id, "COMM", 4) == 0 )
		{
// check for the right format. (minsize 5, must contain a "\0" after the language)
			if ( frame->datasize < 5 )
				goto nextframe;
			if ( !memchr (frame->data + 4, '\0', frame->datasize - 4) )
				goto nextframe;

// now, give the data back to the user, according to the encoding.
			if ( *frame->data == 0 )
				memcpy (value, frame->data + 5, frame->datasize - 5);
			if ( *frame->data == 1 )
			{
				char nulltermvalue[frame->datasize];
				char *isovalue = NULL;
			
// the tag is not null terminated, so i have to create a null terminated string first.
				memset (nulltermvalue, 0, frame->datasize);
				memcpy (nulltermvalue, frame->data + 5, frame->datasize - 5);
				
// Convert from UTF to ISO and copy to the users variable.
				isovalue = convert_from_utf8 (nulltermvalue);
				strncpy (value, isovalue, sizeof(value) - 1);
				free (isovalue);
			}
				
// change linefeeds to a single "return" key.
			crlf2cr (value);
			return TRUE;
		}
		
nextframe:
		curframe = curframe->next;
	}
	
	return FALSE;
}

	
	
	
/***************************************************************************************
 * END OF STATICS
 ***************************************************************************************/
/*
 * Purpose: Reads the ID3 tag from a file.
 * Parameters: tag - The structure to store the tag in, filename - The name of the file to operate on.
 * Returns:
 * 0 if successful,
 * 1 if an error occured when opening the file
 * 2 if error while reading tag.
 * 3 if no TAG found.
 */
int GetID3v2Tag(ID3Tag* tag, const uchar_t *filename, int unicode )
{
	id3v2Tag *v2 = ( id3v2Tag* )calloc (1, sizeof(id3v2Tag));
	DLL *curframe = NULL;
	int error = 0;
	
// Read the tag.
	error = get_id3v2tag_raw (v2, filename, unicode );
// Init the users tag
	memset (tag, 0, sizeof (ID3Tag));

	if( error == 0 )
	{
// if we have a valid tag we copy the raw data to the users struct
		tag->size = v2->tag_size;
		if( v2->ver_major == 3 || v2->ver_major == 4 ) { // ID3 v2.3 or v2.4
  		frame_find (v2, "TPE1", tag->artist);
  		frame_find (v2, "TIT2", tag->title);
  		frame_find (v2, "TALB", tag->album);
  		frame_find (v2, "TYER", tag->year);
  		frame_find (v2, "COMM", tag->comment);
  		frame_find (v2, "TRCK", tag->track);
  		frame_find (v2, "TCON", tag->genre);
		}
		else if( v2->ver_major == 2 ) { // ID3 v2.2
		  frame_find( v2, "TP1", tag->artist );
		  frame_find( v2, "TAL", tag->album );
		  frame_find( v2, "TCO", tag->genre );
		  frame_find( v2, "TT2", tag->title );
		}
	}
	
// Free all the stuff
	if (v2->frames)
	{
		id3v2Frame *frame = NULL;
		
		curframe = v2->frames;
		while ( curframe )
		{
			frame = (id3v2Frame *)curframe->data;
			
			free (frame->data);
			free (frame);
			
			curframe = curframe->next;
		}
		v2->frames = dll_free (v2->frames);
	}

	tag->ver_major = v2->ver_major;
	tag->ver_minor = v2->ver_minor;
	
	free (v2);
	return (error);
}

static int clear_whitespace( char* p, int size )
{
  int i;

  for( i=size-1; i>-1; i-- ) {
    if( p[i] == ' ' || p[i] == '\t' ) {
      p[i] = 0x00;
    }
    else {
      break;
    }
  }

  return (size-i);
}

/*
 * Purpose: Reads the ID3v1 tag from a file.
 * Parameters: tag - The structure to store the tag in, filename - The name of the file to operate on.
 * Returns:
 * 0 if successful,
 * 1 if an error occured when opening the file
 * 2 if error while reading tag.
 * 3 if no TAG found.
 */
int GetID3v1Tag( ID3Tag* tag, const uchar_t* filename, int unicode )
{
	//FILE* fp;
	//struct stat buf;
	char id3buf[128], *pChar;
	RMuint32 sdFd;
	RMuint32 nFilesize;
	RMuint32 nBytes;
	FATFS_ERROR sdFatfsErr;

	if( !tag ) {
		return 2;
	}

	memset (tag, 0, sizeof (ID3Tag));

	//if( lstat(filename, &buf ) < 0 ) return 1;

	//fp = fopen( filename, "rb" );
	//if( !fp ) {
	//	return 1;
	//}
	sdFatfsErr = fatfs_wfopen( (RMuint16*)filename, _O_RDONLY, &sdFd );
	if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) {
		printf( "GetID3v1Tag: error 0x%x\n", sdFatfsErr );
		return 1;
	}

	fatfs_fsize( (RMuint32)sdFd, &nFilesize );

	//fseek( fp, buf.st_size-128, SEEK_SET );
	//fread( id3buf, sizeof(id3buf), 1, fp );
	//fclose( fp );
	fatfs_fseek( sdFd, nFilesize-128, SEEK_SET );
	fatfs_fread( sdFd, (RMuint8*)id3buf, sizeof(id3buf), &nBytes );
	sdFatfsErr = fatfs_fclose( sdFd );
	if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) {
		printf( "GetID3v1Tag: file close error 0x%x\n", sdFatfsErr );
		return 1;
	}

	pChar = id3buf;
	// check ID3v1 Header
	if( pChar[0] != 'T' || pChar[1] != 'A' || pChar[2] != 'G' ) return 3;
	pChar += 3;
	strncpy( tag->title, pChar, 30 );
	clear_whitespace( tag->title, 30 );
	pChar += 30;
	strncpy( tag->artist, pChar, 30 );
	clear_whitespace( tag->artist, 30 );
	pChar += 30;
	strncpy( tag->album, pChar, 30 );
	clear_whitespace( tag->album, 30 );
	pChar += 30;
	strncpy( tag->year, pChar, 4 );
	pChar += 4;
	strncpy( tag->comment, pChar, 30 );
	pChar += 30;
	//strncpy( tag->genre, pChar, 1 );
	tag->genre[0] = *pChar;

	tag->ver_major = 1;
	tag->ver_minor = 0;

	return 0;
}
