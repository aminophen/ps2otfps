# ps2otfps

ps2otfps is a small utility prepared by Akira Kakuto for W32TeX distribution.
This program can be used to change a PS file with CJK fonts into another
PS file which can be printed by Ghostscript without preparing snippets.

## Import from dvipsk-w32.tar.xz 2016-11-09

* w32tex/share/texmf-dist/doc/support/ps2otfps
    - ps2otfps.pdf
    - ps2otfps.tex

* w32tex/share/texmf-dist/dvips/ps2otfps
    - psnames-for-otf

## Import from w32tex-src.tar.xz 2016-05-13 (as in TeX Live r41087)

* w32tex-src/ktx/texk/ps2otfps
    - Makefile (renamed to Makefile.vc)
    - ps2otfps.c

ps2otfps.c is also available in w32tex/share/texmf-dist/doc/dvips/base
The file Makefile.gcc is created by Hironobu Yamashita.

--------------------
Hironobu Yamashita
