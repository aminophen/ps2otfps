/* ps2otfps.c

   Copyright 2013-2014 Akira Kakuto.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.


   Change a PS file with CJK OpenType fonts into another PS
   file which can be printed by Ghostscript.
   Original PS can contain binary data.
   Usage1: ps2otfps oldfile newfile
   Usage2: ps2otfps -f
   Here ps2otfps works as a filter in the Usage2.
*/

#include <kpathsea/kpathsea.h>
#define  PSNAMESFILE "psnames-for-otf"
/*
  If a file $TEXMF/dvips/ps2otfps/psnames-for-otf exists,
  PostScript names of OpenType fonts are read from the file.
  If the file does not exists, or it contains no available data,
  PostScript names of OpenType fonts are obtained from the following
  fixed array char *fonts[].
  The formats for the file $TEXMF/dvips/ps2otfps/psnames-for-otf are:
  (1) empty lines and lines starting by % or # are discarded
  (2) one font name in a line
  example:

% psnames-for-otf
%
% PostSctipt names for OpenType fonts
%
% This file is used by a program ps2otfps
% in order to add needed information to a ps file
% created by the dvips
%
AdobeFanHeitiStd-Bold
AdobeGothicStd-Bold
...
...
*/

/*
  PostScript name of OpenType fonts.
  Any font can be added here.
  PostScripr name of an OpenType font is shown by
  otfinfo -p opentypefontfile
*/
char *fonts[] = {
"AdobeFanHeitiStd-Bold",
"AdobeGothicStd-Bold",
"AdobeGothicStd-Light",
"AdobeHeitiStd-Regular",
"AdobeMingStd-Light",
"AdobeMyungjoStd-Medium",
"AdobeSongStd-Light",
"FutoGoB101-Bold",
"FutoGoB101Pr6N-Bold",
"FutoGoB101Pro-Bold",
"FutoMinA101-Bold",
"FutoMinA101Pr6N-Bold",
"FutoMinA101Pro-Bold",
"GothicBBB-Medium",
"GothicBBBPr6N-Medium",
"GothicBBBPro-Medium",
"HYGoThic-Medium",
"HYSMyeongJo-Medium",
"HiraKakuPro-W3",
"HiraKakuPro-W6",
"HiraKakuProN-W3",
"HiraKakuProN-W6",
"HiraKakuStd-W8",
"HiraKakuStdN-W8",
"HiraMaruPro-W4",
"HiraMaruProN-W4",
"HiraMinPro-W3",
"HiraMinPro-W6",
"HiraMinProN-W3",
"HiraMinProN-W6",
"HiraMinStdN-W2",
"Jun101-Light",
"Jun101Pr6N-Light",
"Jun101Pro-Light",
"KozGoPr6N-Bold",
"KozGoPr6N-ExtraLight",
"KozGoPr6N-Heavy",
"KozGoPr6N-Light",
"KozGoPr6N-Medium",
"KozGoPr6N-Regular",
"KozGoPro-Bold",
"KozGoPro-ExtraLight",
"KozGoPro-Heavy",
"KozGoPro-Light",
"KozGoPro-Medium",
"KozGoPro-Regular",
"KozMinPr6N-Bold",
"KozMinPr6N-ExtraLight",
"KozMinPr6N-Heavy",
"KozMinPr6N-Light",
"KozMinPr6N-Medium",
"KozMinPr6N-Regular",
"KozMinPro-Bold",
"KozMinPro-ExtraLight",
"KozMinPro-Heavy",
"KozMinPro-Light",
"KozMinPro-Medium",
"KozMinPro-Regular",
"MHei-Medium",
"MSung-Light",
"MidashiGo-MB31",
"MidashiGoPr6N-MB31",
"MidashiGoPro-MB31",
"MidashiMin-MA31",
"MidashiMinPr6N-MA31",
"MidashiMinPro-MA31",
"Ryumin-Light",
"RyuminPr6N-Light",
"RyuminPro-Light",
"STHeiti-Regular",
"STSong-Light",
"ShinGo-Bold",
"ShinGo-Heavy",
"ShinGo-Light",
"ShinGo-Medium",
"ShinGo-Regular",
"ShinGo-Ultra",
"ShinGoPr6N-Bold",
"ShinGoPr6N-Heavy",
"ShinGoPr6N-Light",
"ShinGoPr6N-Medium",
"ShinGoPr6N-Regular",
"ShinGoPr6N-Ultra",
"ShinGoPro-Bold",
"ShinGoPro-Heavy",
"ShinGoPro-Light",
"ShinGoPro-Medium",
"ShinGoPro-Regular",
"ShinGoPro-Ultra",
NULL};

void usage(void)
{
  fprintf(stderr, "Change a PS file with CJK OpenType fonts into another PS\n");
  fprintf(stderr, "file which can be printed by Ghostscript.\n");
  fprintf(stderr, "Original PS can contain binary data.\n");
  fprintf(stderr, "usage1: ps2otfps oldfile newfile\n");
  fprintf(stderr, "usage2: ps2otfps -f\n");
  fprintf(stderr, "Here ps2otfps works as a filter in the usage2\n");
}

int main(int argc, char *argv[])
{
  FILE *psin, *psout;
  char *buf, enc[64], rdbuf[4096];
  char *p, *q, *r;
  char **pp;
  int n;
  int indoc = 0;
  int filter = 0;
  char **fontsfromfile = NULL;
  int  numfonts = 0;
  char *fullname;
  FILE *psname;

  kpse_set_program_name(argv[0], "dvips");
  
#ifdef WIN32
  _setmode(fileno(stdin), _O_BINARY);
  _setmode(fileno(stdout), _O_BINARY);
#endif

  if(argc == 2 && (!strncmp(argv[1], "-h", 2) || !strncmp(argv[1], "--h", 3))) {
    usage();
    return 0;
  }

  if(argc >= 2 && !strncmp(argv[1], "-f", 2)) {
    filter = 1;
    psin = stdin;
    psout = stdout;
  } else if(argc == 3) {
    psin = fopen(argv[1], "rb");
    if(!psin) {
      fprintf(stderr, "Input file %s open error.\n", argv[1]);
      return 1;
    }
    psout = fopen(argv[2], "wb");
      if(!psout) {
      fprintf(stderr, "Output file %s open error.\n", argv[2]);
      return 1;
    }
  } else {
    usage();
    return 1;
  }

  fullname = kpse_find_file(PSNAMESFILE, kpse_program_text_format, 0);
  if(fullname) {
    psname = fopen(fullname, "r");
    if(psname) {
      while(fgets(rdbuf, 256, psname)) {
        if(rdbuf[0] == '\n' || rdbuf[0] == '%' || rdbuf[0] == '#')
          continue;
        numfonts++;
      }
      fclose(psname);
      if(numfonts) {
        int len;
        fontsfromfile = xmalloc((numfonts + 1) * sizeof(char *));
        pp = fontsfromfile;
        psname = fopen(fullname, "r");
        while(fgets(rdbuf, 256, psname)) {
          if(rdbuf[0] == '\n' || rdbuf[0] == '%' || rdbuf[0] == '#')
            continue;
          len = strlen(rdbuf);
          p = rdbuf + len - 1;
          while(*p == '\r' || *p == '\n'|| *p == '\t'|| *p == ' ')
            *p-- = '\0';
          *pp = xmalloc(strlen(rdbuf) + 1);
          strcpy(*pp, rdbuf);
          pp++;
	}
        *pp = NULL;
        fclose(psname);
      }
    }
    free(fullname);
  }

/*
  allocate very large memopry (6 Mbytes) for a working buffer
*/
  buf = malloc(6291456);

  while((n = getc(psin)) != EOF) {
    putc(n, psout);
    if(n == '%') {
/*
  a candidate of ASCII line, which may by a character in binary data,
  thus I allocate large memory to the buf
*/
      r = buf;
      *r++ = '%';
      while((n = getc(psin)) != '\n' && n != EOF) {
        putc(n, psout);
        *r++ = n;
      }
      if (n == EOF)
	goto EndOfWork;
      putc(n, psout);
      *r++ = ' ';
      *r = '\0';
      p = buf;
      if(strncmp(p, "%%DocumentFonts:", 16) == 0) {
        for( ; ; ) {
          fgets(rdbuf, 4094, psin);
          if(strncmp(rdbuf, "%%+", 3) == 0) {
            fputs(rdbuf, psout);
            strcat(p, " ");
            strcat(p, rdbuf);
            *(p + strlen(p) - 1) = ' ';
          } else {
            if(numfonts)
              pp = fontsfromfile;
            else
              pp = fonts;
            while(*pp) {
              q = strstr(p, *pp);
              if(q) {
                q += strlen(*pp) + 1;
                r = enc;
                while(*q != ' ' && *q != '\t' && *q != '\n' && *q != '\r')
                  *r++ = *q++;
                *r = '\0';
                if(strncmp(enc, "Identity", 8)) {
                  fprintf(psout, "%%%%BeginFont: %s-%s\n", *pp, enc);
                  fprintf(psout, "/%s-%s\n", *pp, enc);
                  fprintf(psout, "/%s /CMap findresource\n", enc);
                  fprintf(psout, "[/%s /CIDFont findresource]\n", *pp);
                  fprintf(psout, "composefont pop\n");
                  fprintf(psout, "%%%%EndFont\n");
                }
              }
              pp++;
            }
            fputs(rdbuf, psout);
            break;
          }
        }
      }
    }
  }

 EndOfWork:
  free(buf);
  if(filter == 0) {
    fclose(psin);
    fclose(psout);
  }
  if(fontsfromfile) {
    pp = fontsfromfile;
    while(*pp) {
      free(*pp);
      pp++;
    }
    free(fontsfromfile);
  }
  return 0;
}
