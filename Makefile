KPSELIB = ../kpathsea/kpathsea622.lib
CFLAGS = -nologo -MD -O2 -DWIN32=1 -I..
CC = cl
ps2otfps.exe: ps2otfps.obj
	$(CC) -Fe$@ $< setargv.obj $(KPSELIB)

ps2otfps.obj: ps2otfps.c
	$(CC) $(CFLAGS) -c $<

clean::
	rm -f *~ *.obj *.exe

SDST=c:/usr/work/edrive/wk/dist/DVIPSK/share/texmf-dist/doc/dvips/base/
install: ps2otfps.exe ps2otfps.c
	cp -p ps2otfps.exe c:/usr/local/bin/
	cp -p ps2otfps.exe c:/usr/work/edrive/wk/dist/DVIPSK/bin/
	cp -p ps2otfps.c $(SDST)
