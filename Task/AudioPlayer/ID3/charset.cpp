/*
 * Main part of code, written by:
 *
 * Copyright (C) 1999-2001  Håvard Kvålen <havardk@xmms.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef HAVE_ICONV_OPEN
#include <iconv.h>
#endif

#ifdef HAVE_LANGINFO_CODESET
#include <langinfo.h>
#endif

#include "charset.h"


/****************
 * Declarations *
 ****************/

#define CHARSET_TRANS_ARRAY_LEN ( sizeof(charset_trans_array) / sizeof((charset_trans_array)[0]) )
const CharsetInfo charset_trans_array[] = { 
    {N_("Arabic (IBM-864)"),                  "IBM864"        },
    {N_("Arabic (ISO-8859-6)"),               "ISO-8859-6"    },
    {N_("Arabic (Windows-1256)"),             "windows-1256"  },
    {N_("Baltic (ISO-8859-13)"),              "ISO-8859-13"   },
    {N_("Baltic (ISO-8859-4)"),               "ISO-8859-4"    },
    {N_("Baltic (Windows-1257)"),             "windows-1257"  },
    {N_("Celtic (ISO-8859-14)"),              "ISO-8859-14"   },
    {N_("Central European (IBM-852)"),        "IBM852"        },
    {N_("Central European (ISO-8859-2)"),     "ISO-8859-2"    },
    {N_("Central European (Windows-1250)"),   "windows-1250"  },
    {N_("Chinese Simplified (GB18030)"),      "gb18030"       },
    {N_("Chinese Simplified (GB2312)"),       "GB2312"        },
    {N_("Chinese Traditional (Big5)"),        "Big5"          },
    {N_("Chinese Traditional (Big5-HKSCS)"),  "Big5-HKSCS"    },
    {N_("Cyrillic (IBM-855)"),                "IBM855"        },
    {N_("Cyrillic (ISO-8859-5)"),             "ISO-8859-5"    },
    {N_("Cyrillic (ISO-IR-111)"),             "ISO-IR-111"    },
    {N_("Cyrillic (KOI8-R)"),                 "KOI8-R"        },
    {N_("Cyrillic (Windows-1251)"),           "windows-1251"  },
    {N_("Cyrillic/Russian (CP-866)"),         "IBM866"        },
    {N_("Cyrillic/Ukrainian (KOI8-U)"),       "KOI8-U"        },
    {N_("English (US-ASCII)"),                "us-ascii"      },
    {N_("Greek (ISO-8859-7)"),                "ISO-8859-7"    },
    {N_("Greek (Windows-1253)"),              "windows-1253"  },
    {N_("Hebrew (IBM-862)"),                  "IBM862"        },
    {N_("Hebrew (Windows-1255)"),             "windows-1255"  },
    {N_("Japanese (EUC-JP)"),                 "EUC-JP"        },
    {N_("Japanese (ISO-2022-JP)"),            "ISO-2022-JP"   },
    {N_("Japanese (Shift_JIS)"),              "Shift_JIS"     },
    {N_("Korean (EUC-KR)"),                   "EUC-KR"        },
    {N_("Nordic (ISO-8859-10)"),              "ISO-8859-10"   },
    {N_("South European (ISO-8859-3)"),       "ISO-8859-3"    },
    {N_("Thai (TIS-620)"),                    "TIS-620"       },
    {N_("Turkish (IBM-857)"),                 "IBM857"        },
    {N_("Turkish (ISO-8859-9)"),              "ISO-8859-9"    },
    {N_("Turkish (Windows-1254)"),            "windows-1254"  },
    {N_("Unicode (UTF-7)"),                   "UTF-7"         },
    {N_("Unicode (UTF-8)"),                   "UTF-8"         },
    {N_("Unicode (UTF-16BE)"),                "UTF-16BE"      },
    {N_("Unicode (UTF-16LE)"),                "UTF-16LE"      },
    {N_("Unicode (UTF-32BE)"),                "UTF-32BE"      },
    {N_("Unicode (UTF-32LE)"),                "UTF-32LE"      },
    {N_("Vietnamese (VISCII)"),               "VISCII"        },
    {N_("Vietnamese (Windows-1258)"),         "windows-1258"  },
    {N_("Visual Hebrew (ISO-8859-8)"),        "ISO-8859-8"    },
    {N_("Western (IBM-850)"),                 "IBM850"        },
    {N_("Western (ISO-8859-1)"),              "ISO-8859-1"    },
    {N_("Western (ISO-8859-15)"),             "ISO-8859-15"   },
    {N_("Western (Windows-1252)"),            "windows-1252"  }

    /*
     * From this point, character sets aren't supported by iconv
     */
/*    {N_("Arabic (IBM-864-I)"),                "IBM864i"              },
    {N_("Arabic (ISO-8859-6-E)"),             "ISO-8859-6-E"         },
    {N_("Arabic (ISO-8859-6-I)"),             "ISO-8859-6-I"         },
    {N_("Arabic (MacArabic)"),                "x-mac-arabic"         },
    {N_("Armenian (ARMSCII-8)"),              "armscii-8"            },
    {N_("Central European (MacCE)"),          "x-mac-ce"             },
    {N_("Chinese Simplified (GBK)"),          "x-gbk"                },
    {N_("Chinese Simplified (HZ)"),           "HZ-GB-2312"           },
    {N_("Chinese Traditional (EUC-TW)"),      "x-euc-tw"             },
    {N_("Croatian (MacCroatian)"),            "x-mac-croatian"       },
    {N_("Cyrillic (MacCyrillic)"),            "x-mac-cyrillic"       },
    {N_("Cyrillic/Ukrainian (MacUkrainian)"), "x-mac-ukrainian"      },
    {N_("Farsi (MacFarsi)"),                  "x-mac-farsi"},
    {N_("Greek (MacGreek)"),                  "x-mac-greek"          },
    {N_("Gujarati (MacGujarati)"),            "x-mac-gujarati"       },
    {N_("Gurmukhi (MacGurmukhi)"),            "x-mac-gurmukhi"       },
    {N_("Hebrew (ISO-8859-8-E)"),             "ISO-8859-8-E"         },
    {N_("Hebrew (ISO-8859-8-I)"),             "ISO-8859-8-I"         },
    {N_("Hebrew (MacHebrew)"),                "x-mac-hebrew"         },
    {N_("Hindi (MacDevanagari)"),             "x-mac-devanagari"     },
    {N_("Icelandic (MacIcelandic)"),          "x-mac-icelandic"      },
    {N_("Korean (JOHAB)"),                    "x-johab"              },
    {N_("Korean (UHC)"),                      "x-windows-949"        },
    {N_("Romanian (MacRomanian)"),            "x-mac-romanian"       },
    {N_("Turkish (MacTurkish)"),              "x-mac-turkish"        },
    {N_("User Defined"),                      "x-user-defined"       },
    {N_("Vietnamese (TCVN)"),                 "x-viet-tcvn5712"      },
    {N_("Vietnamese (VPS)"),                  "x-viet-vps"           },
    {N_("Western (MacRoman)"),                "x-mac-roman"          },
    // charsets whithout posibly translatable names
    {"T61.8bit",                              "T61.8bit"             },
    {"x-imap4-modified-utf7",                 "x-imap4-modified-utf7"},
    {"x-u-escaped",                           "x-u-escaped"          },
    {"windows-936",                           "windows-936"          }
*/
};




/*************
 * Functions *
 *************/
// Return the last item of an doubly linked list
static DLL *
dll_last (DLL *list)
{
	DLL *item = list;
	
	while ( item->next )
		item = item->next;
	
	return item;
}

// Append an item to the doubly linked list
static DLL *
dll_append (DLL *list, void *data)
{
	DLL *item = ( DLL* )malloc (sizeof(DLL));
	
	item->data = data;
	item->prev = dll_last(list);
	item->next = NULL;
	
	return item;
}

static char* get_current_charset (void)
{
    char *charset = getenv("CHARSET");

#ifdef HAVE_LANGINFO_CODESET
    if (!charset)
        charset = nl_langinfo(CODESET);
#endif
    if (!charset)
        charset = "ISO-8859-1";

    return charset;
}


#ifdef HAVE_ICONV_OPEN
static char* convert_string (const char *string, char *from, char *to)
{
    size_t outleft, outsize, length;
    iconv_t cd;
    char *out, *outptr;
    const char *input = string;

    if (!string)
        return NULL;

    length = strlen(string);

    /*  g_message("converting %s from %s to %s", string, from, to); */
    if ((cd = iconv_open(to, from)) == (iconv_t)-1)
    {
		fprintf (stderr, "convert_string(): Conversion not supported. Charsets: %s -> %s", from, to);
        return strdup(string);
    }

    /* Due to a GLIBC bug, round outbuf_size up to a multiple of 4 */
    /* + 1 for nul in case len == 1 */
    outsize = ((length + 3) & ~3) + 1;
    out = g_malloc(outsize);
    outleft = outsize - 1;
    outptr = out;

 retry:
    if (iconv(cd, (char **)&input, &length, &outptr, &outleft) == -1)
    {
        int used;
        switch (errno)
        {
            case E2BIG:
                used = outptr - out;
                outsize = (outsize - 1) * 2 + 1;
                out = g_realloc(out, outsize);
                outptr = out + used;
                outleft = outsize - 1 - used;
                goto retry;
            case EINVAL:
                break;
            case EILSEQ:
                /* Invalid sequence, try to get the
                                   rest of the string */
                input++;
                length = strlen(input);
                goto retry;
            default:
                fprintf (stderr, "convert_string(): Conversion failed. Inputstring: %s; Error: %s", string, strerror(errno));
                break;
        }
    }
    *outptr = '\0';

    iconv_close(cd);
    return out;
}
#else
static char* convert_string (const char *string, char *from, char *to)
{
    if (!string)
        return NULL;
    return strdup(string);
}
#endif


/*
 * Conversion with UTF-8 for ogg tags
 */
char* convert_to_utf8 (const char *string)
{
    char *charset = get_current_charset();

    return convert_string(string, charset, "UTF-8");
}

char* convert_from_utf8 (const char *string)
{
    char *charset = get_current_charset();

    return convert_string(string, "UTF-8", charset);
}


DLL *Charset_Create_List (void)
{
    DLL *list = NULL;
    int i;

    for (i=0; i<CHARSET_TRANS_ARRAY_LEN; i++)
        list = dll_append (list,_(charset_trans_array[i].charset_title));
    return list;
}


/*
 * Return charset_name from charset_title
 */
char *Charset_Get_Name_From_Title (char *charset_title)
{
    int i;

    if (charset_title)
        for (i=0; i<CHARSET_TRANS_ARRAY_LEN; i++)
            if ( strcasecmp(_(charset_title),_(charset_trans_array[i].charset_title)) == 0 )
                return charset_trans_array[i].charset_name;
    return "";
}


/*
 * Return charset_title from charset_name
 */
char *Charset_Get_Title_From_Name (char *charset_name)
{
    int i;

    if (charset_name)
        for (i=0; i<CHARSET_TRANS_ARRAY_LEN; i++)
            if ( strcasecmp(charset_name,charset_trans_array[i].charset_name) == 0 )
                return _(charset_trans_array[i].charset_title);
    return "";
}



/*
 * Test if the conversion is supported between two character sets ('from' and 'to)
 */
#ifdef HAVE_ICONV_OPEN
short int test_conversion_charset (char *from, char *to)
{
    iconv_t cd;

    if ((cd=iconv_open(to,from)) == (iconv_t)-1)
    {
        /* Conversion not supported */
        return FALSE;
    }
    iconv_close(cd);
    return TRUE;
}
#else
short int test_conversion_charset (char *from, char *to)
{
    return TRUE;
}
#endif
