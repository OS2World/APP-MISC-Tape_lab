#
#
# tapelab.exe makefile
# version 0.09 2-8-1995
# to be used with dmake & gcc/emx 0.9a patchlevel 3
#
# 1995 1996 by G.Iachello giac@dei.unipd.it
#
CFLAGS=-Zmtd -Zcrtdll -g -v -Wall -Id:\emx\os2class -m486
CC=gcc $(CFLAGS)
B=/emx/bin/
I=/emx/include/
IP=/emx/include.cpp/
L=/emx/lib/
OS2CLASS=/emx/os2class/
LIBS=$(L)crt0.o $(L)mt/c.a $(L)gcc.a
LIBP=$(L)iostream.a
LIBOS2=$(L)os2.a
LIBBSD=$(L)bsd.a

.SUFFIXES: .o .c .cc .cpp .s .exe .obj

.c.o: 
			$(CC) -c $<

.cc.o:
			$(CC) -c $< 

.cpp.o:
			$(CC) -c $< 

.c.s:
			$(CC) -S $< 

.o.exe:
			$(CC) -o $*.exe $< 


default:        tapelab.exe tapelab.hlp

all:            tapelab.exe tapelab.hlp

clean:
			-del *.o $(DELOPT)
			-del *.exe $(DELOPT)
			-del *.dll $(DELOPT)
			-del *.map $(DELOPT)
			-del *.lib $(DELOPT)

#############################################################################        


tapelab.o:       tapelab.cpp tapelab.h $(OS2CLASS)pmwin.h $(OS2CLASS)pmdlg.h $(OS2CLASS)pmgpi.h $(OS2CLASS)pmsys.h $(I)stdio.h $(I)os2emx.h
		$(CC) -c tapelab.cpp

tl_core.o:       tl_core.cpp tapelab.h $(OS2CLASS)pmwin.h $(OS2CLASS)pmdlg.h $(OS2CLASS)pmgpi.h $(OS2CLASS)pmsys.h $(I)stdio.h $(I)os2emx.h
		$(CC) -c tl_core.cpp

tl_main.o:       tl_main.cpp tapelab.h $(OS2CLASS)pmwin.h $(OS2CLASS)pmdlg.h $(OS2CLASS)pmgpi.h $(OS2CLASS)pmsys.h $(I)stdio.h $(I)os2emx.h
		$(CC) -c tl_main.cpp

tl_paint.o:       tl_paint.cpp tapelab.h $(OS2CLASS)pmwin.h $(OS2CLASS)pmdlg.h $(OS2CLASS)pmgpi.h $(OS2CLASS)pmsys.h $(I)stdio.h $(I)os2emx.h
		$(CC) -c tl_paint.cpp

tapelab.res:     tapelab.rc tapelab.h
		rc -r -i \emx\include -i \emx\os2class tapelab.rc 

tapelab:         tapelab.o tl_core.o tl_paint.o tl_main.o tapelab.def $(OS2CLASS)pmpp.a $(LIBS) $(LIBOS2)
		$(CC) -o tapelab tapelab.o tl_core.o tl_paint.o tl_main.o $(OS2CLASS)pmpp.a tapelab.def
        									 
tapelab.exe:     tapelab tapelab.res
		emxbind -bpq -rtapelab.res $(B)emxl tapelab

#
#  You'll need the IPF file pre-processor by Larry Salomon to do this.
#  It can be found on the hobbes archive (ftp-os2.cdrom.com)
#
tapelab.ipf:     tapelab.ipp $(OS2CLASS)pmstdres.h tapelab.h
		ipfcpp -i tapelab.ipp -o tapelab.ipf
#
#  You'll need the IPF compiler by IBM. It can be found on the DEVCON CD's
#  or in the Dev. Toolkits.
#
tapelab.hlp:     tapelab.ipf
		ipfc tapelab.ipf


