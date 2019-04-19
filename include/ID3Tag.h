/*
 * TAVI-PMP Version 1.0
 * new media LIFE CO., LTD.
 * ID3Tag.h
 * Sang-jin Ryu (ew@newmedialife.com)
 *
 */

#ifndef __ID3_TAG_H__
#define __ID3_TAG_H__


typedef struct _tag_ID3v1
{
	char TAG[3];
	char Title[30];
	char Artist[30];
	char Album[30];
	char Year[4];
	char Comment[29];
	char Track;
	char Genre;
	
} ID3v1;

const char stGenre[][24] = {

// The following genres is defined in ID3v1
// 0. Blues
"Blues",
"Classic Rock",
"Country",
"Dance",
"Disco",
"Funk",
"Grunge",
"Hip-Hop",
"Jazz",
"Metal",
"New Age",
"Oldies",
"Other",
"Pop",
"R&B",
"Rap",
"Reggae",
"Rock",
"Techno",
"Industrial",
"Alternative",
"Ska",
"Death Metal",
"Pranks",
"Soundtrack",
"Euro-Techno",
"Ambient",
"Trip-Hop",
"Vocal",
"Jazz+Funk",
"Fusion",
"Trance",
"Classical",
"Instrumental",
"Acid",
"House",
"Game",
"Sound Clip",
"Gospel",
"Noise",
"AlternRock",
"Bass",
"Soul",
"Punk",
"Space",
"Meditative",
"Instrumental Pop",
"Instrumental Rock",
"Ethnic",
"Gothic",
"Darkwave",
"Techno-Industrial",
"Electronic",
"Pop-Folk",
"Eurodance",
"Dream",
"Southern Rock",
"Comedy",
"Cult",
"Gangsta",
"Top 40",
"Christian Rap",
"Pop/Funk",
"Jungle",
"Native American",
"Cabaret",
"New Wave",
"Psychadelic",
"Rave",
"Showtunes",
"Trailer",
"Lo-Fi",
"Tribal",
"Acid Punk",
"Acid Jazz",
"Polka",
"Retro",
"Musical",
"Rock & Roll",
"Hard Rock",

// The following genres are Winamp extensions (http://www.id3.org/id3v2.3.0.html)
// 80. Folk
"Folk",
"Folk-Rock",
"National Folk",
"Swing",
"Fast Fusion",
"Bebob",
"Latin",
"Revival",
"Celtic",
"Bluegrass",
"Avantgarde",
"Gothic Rock",
"Progressive Rock",
"Psychedelic Rock",
"Symphonic Rock",
"Slow Rock",
"Big Band",
"Chorus",
"Easy Listening",
"Acoustic",
"Humour",
"Speech",
"Chanson",
"Opera",
"Chamber Music",
"Sonata",
"Symphony",
"Booty Bass",
"Primus",
"Porn Groove",
"Satire",
"Slow Jam",
"Club",
"Tango",
"Samba",
"Folklore",
"Ballad",
"Power Ballad",
"Rhythmic Soul",
"Freestyle",
"Duet",
"Punk Rock",
"Drum Solo",
"A capella",
"Euro-House",
"Dance Hall"
};


#endif

