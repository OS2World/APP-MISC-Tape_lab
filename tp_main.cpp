/* 


	tapelab.cpp (emx+gcc) 
	Tape Label Editor Version 0.08

	1995 Giovanni Iachello
	This is freeware software. You can use or modify it as you wish,
	provided that the part of code that I wrote remains freeware.
	Freeware means that the source code must be available on request 
	to anyone.
	You must also include this notice in all files derived from this
	file.


*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "tapelab.h"
#include "pmstdres.h"
#include "pmhelp.h"

PMApp* App;

PMHelpWin *helpwin;

/////////////////////////////////////////////////////////////////////////////
//
// this help hook services requests of help from message boxes.
//
//

BOOL helpHook ( HAB habAnchor, SHORT sMode, USHORT usTopic, USHORT usSubTopic, PRECTL prclPos )
{
	if ( (sMode == HLPM_WINDOW ) && 
	     helpwin && 
		 ( usTopic>=PMHLP_DLG_ERRMSG && usTopic<=PMHLP_DLG_ASSERTFAIL)
	   ) {
		helpwin->sendMsg(HM_DISPLAY_HELP, 
			MPFROMLONG (MAKELONG (usTopic, 0)), MPFROMSHORT( HM_RESOURCEID) );
		return TRUE;
	} 
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////

class InfoDlg : public PMModalDialog {
public:
	InfoDlg(HWND hwnd,PMControlMap* ctrlmap,void* data) : PMModalDialog(HWND_DESKTOP,hwnd,DLG_INFO,ctrlmap,data)
		{ }
	BOOL command(USHORT id,USHORT cmddev) {
		switch (id) {
			case DINFO_PB_CURRDATE1:
			case DINFO_PB_CURRDATE2:
				char buf[64];
				time_t t;
				t=time(NULL);
				strftime(buf,64,"%D",localtime(&t));
				if (id == DINFO_PB_CURRDATE1) controlFromID(DINFO_EF_DATE1)->setText(buf);
				else controlFromID(DINFO_EF_DATE2)->setText(buf);
  	        	return TRUE;
		} 
		return PMModalDialog::command(id,cmddev);
	}
	BOOL initdlg()
	{
		PMSpinButton* sp;
		sp=(PMSpinButton*)controlFromID(DINFO_SB_SOURCE1);
		sp->setArray(apchSources,5); sp->set();

		sp=(PMSpinButton*)controlFromID(DINFO_SB_SOURCE2);
		sp->setArray(apchSources,5); sp->set();

		sp=(PMSpinButton*)controlFromID(DINFO_SB_NR1);
		sp->setArray(apchNR,6); sp->set();

		sp=(PMSpinButton*)controlFromID(DINFO_SB_NR2);
		sp->setArray(apchNR,6); sp->set();

		return TRUE;
	}
};

class SizeDlg : public PMModalDialog {
public:
	SizeDlg(HWND hwnd,PMControlMap* ctrlmap,void* data) : PMModalDialog(HWND_DESKTOP,hwnd,DLG_SIZE,ctrlmap,data)
		{ }
	BOOL initdlg()
	{
		PMSpinButton* sp=(PMSpinButton*)controlFromID(DSIZE_SB_FLAPS);
		sp->setLimits(4,2);
		sp->set();
		return TRUE;
	}
};

//////////////////////////////////////////////////////////////////////////////

//
// usa un thread separato per stampare l'output, in modo da non bloccare
// il programma!
// use a separate thread for printing, to avoid blocking the rest of the
// program.
//


class PMPrintLabelThread : public PMWindowThread 
{
	TapeLab* tl;
	char* buf;
public:
	PMPrintLabelThread(char* ibuf,TapeLab *itl) : PMWindowThread() 
	{
		buf=ibuf; tl=itl;
	}
	void main(void* arg=NULL) 
	{
		PMPrinterDC printer(hab);
		printer.open();
		printer.startDoc(buf);

		PMPresSpace *prnps=new PMPresSpace(&printer,0,0,PU_LOMETRIC|GPIF_DEFAULT|GPIT_NORMAL|GPIA_ASSOC,hab);

		tl->paintlabel(*prnps);

		delete prnps;
		printer.endDoc(buf);
	}
};

//////////////////////////////////////////////////////////////////////////////


int main (int argc,char* argv[])
{
	char* file=NULL;
	if (argc>1) file=argv[1];
	
	PMAnchorBlock ab;
	PMMessageQueue mq;
	ab.init();
	mq.create(ab);
	
	App=new PMApp(ab,mq,argc,argv);

	helpwin=new PMHelpWin("Tape Label Editor Help","tapelab.hlp",ID_TAPELAB,ab);
	helpwin->createWin();

	TapeLab * tl=new TapeLab(ab,file); // load also a file specified on the command line
	tl->createWin();
	
	ab.setHook(mq,HK_HELP,(PFN)helpHook,NULLHANDLE);

	App->run();

	ab.releaseHook(mq,HK_HELP,(PFN)helpHook,NULLHANDLE);

	tl->destroyWin();

	mq.destroy();
	ab.uninit();

	return (0);
}


