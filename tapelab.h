/* 


	tapelab.h (emx+gcc) 
	Tape Label Editor Version 0.09

	1995 1996 Giovanni Iachello
	This is freeware software. You can use or modify it as you wish,
	provided that the part of code that I wrote remains freeware.
	Freeware means that the source code must be available on request 
	to anyone.
	You must also include this notice in all files derived from this
	file.


*/
#ifndef _TAPELAB_H_
#define _TAPELAB_H_

#define ID_TAPELAB 1

#define DID_HELP 9

#define IDM_COMP 10
#define IDM_SIZE 11
#define IDM_TITLE 12
#define IDM_TEXT 13
#define IDM_SAVE 16
#define IDM_INFO 14
#define IDM_FORM 15

#define IDM_FONTS 20
#define IDM_FON_TIA 21
#define IDM_FON_TIB 22
#define IDM_FON_TXA 23
#define IDM_FON_TXB 24
#define IDM_FON_DATE 25
#define IDM_FON_SOURCE 26
#define IDM_FON_NR 27


#define DLG_TITLE 1100
#define DTITLE_EF_T1 1101
#define DTITLE_EF_T2 1102

#define DLG_TEXT 1200
#define DTEXT_MLE_T1 1201
#define DTEXT_MLE_T2 1202

#define DLG_SIZE 1300
#define DSIZE_SB_FLAPS 1301
#define DSIZE_RB_NORMAL 1302
#define DSIZE_RB_TDK 1303
#define DSIZE_RB_SONY 1304
#define DSIZE_RB_CUSTOM 1305
#define DSIZE_EF_X 1306
#define DSIZE_EF_Y 1307
#define DSIZE_EF_WIDTH 1308
#define DSIZE_EF_FLAP1 1309
#define DSIZE_EF_FLAP2 1310
#define DSIZE_EF_FLAP3 1311
#define DSIZE_EF_FLAP4 1312

#define DLG_INFO 1400
#define DINFO_EF_DATE1 1401
#define DINFO_PB_CURRDATE1 1407
#define DINFO_SB_NR1 1402
#define DINFO_SB_SOURCE1 1403
#define DINFO_EF_DATE2 1404
#define DINFO_PB_CURRDATE2 1408
#define DINFO_SB_NR2 1405
#define DINFO_SB_SOURCE2 1406
#define DINFO_CB_BASE0 1410   
#define DINFO_CB_BASE1 1411
#define DINFO_CB_BASE2 1412
#define DINFO_CB_BASE3 1413
#define DINFO_CB_BASE4 1414
#define DINFO_CB_BASE5 1415

#define DLG_FORMAT 1600
#define DFORM_EF_TABSIZE 1601
#define DFORM_EF_TITLM 1602
#define DFORM_EF_TTM 1603
#define DFORM_EF_TLM 1604
#define DFORM_EF_THSB 1605
#define DFORM_EF_THSA 1606
#define DFORM_EF_TVSA 1607
#define DFORM_EF_TVSB 1608

#define SA_APPLNAME "TAPELAB"
#define SA_SWPKEY "WINDOWPOS"
#define SA_SETTINGSKEY "SETTINGS_V1"
					  
#include "tl_help.h"

#ifndef RC_INVOKED

#define INCL_WIN
#include "pmwin.h"
#include "pmdlg.h"
#include "pmgpi.h"

struct _fontinfo 		// full information for font management
{
	FATTRS fattrs;		// OS2 font structure
	LONG height;		// .. and useful info about the font (sizes etc)
	LONG extleading;
	ULONG type;
	SHORT point;
};

struct _formatting {
	int titlelm; // margine sin. titolo left title margin
	int textlm,texttm; // margine sinistro e superiore testo, text left and top margins
	int tabsize; // dimensione tabs, tab size;
	int textvert1,textvert2;	// permillesimi di spaziatura orizzontale e verticale, thousands of vertical spaceing 
	int texthoriz1,texthoriz2;	
};

struct _customsize {    // custom label size 
	int width;
	int flap[4];
};

// this structure contains all the information about the label. It is used
// partly also by the save settings function.
struct _labelformat 
{
// data members
	int flaps; // numero di flaps 2, 3 o 4 (flaps number 2,3, or 4)
	int size;  // size parameters 

	char* title1;
	char* title2;
	_fontinfo title1font;
	_fontinfo title2font;

	char* text1;
	char* text2;
	_fontinfo text1font;
	_fontinfo text2font;

	_formatting form;
	
	int whatinfo[6]; // sceglie quali info stampare
	char* date1;
	int source1;
	int nr1;
	char* date2;
	int source2;
	int nr2;
	_fontinfo datefont;
	_fontinfo sourcefont;
	_fontinfo nrfont;

	_customsize customsize;
// member functions	
	_labelformat();
	~_labelformat();
	BOOL save(const char* file); // save to file
	BOOL load(const char* file); // load from file
	void newformat(_labelformat* nf);  // read new format form init file
	void writecustomfixpoints(); // write fixpoints structure with customsize data
};


extern POINTL fixpoints[4][10];
extern char *apchSources[];
extern char *apchNR[];

// application global information

extern PMApp* App;

extern PMHelpWin *helpwin;

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#define pts2dmm(x) ((x)*254L/72L)

class TapeLab : public PMMainWin {
	char* tempfile;
	_labelformat labelformat;
	PMVertScrollBar* sb;
	PMRect viewport;
	LONG sizey,posy;
public:
	TapeLab(HAB ab,char* file);
	~TapeLab();
	BOOL paint(PMPresSpace& ps);       // redefine processing of these messages.
	BOOL paintlabel(PMPresSpace& ps);
	BOOL size(SHORT cx,SHORT cy);
	BOOL other(PMEvent &event);	
	void saveApplication();		 
	BOOL create(PMEvent &event);

	BOOL command(USHORT id,USHORT cmddev);
	BOOL setFont(_fontinfo *font,char* title);
	BOOL paintString(char* str,_fontinfo *font,int spc,PMPoint pos,PMPresSpace &ps);
	BOOL createWin(); 
	
	BOOL fileNew(); 
	BOOL filePrint();
	BOOL fileOpen(PCSZ filename); 
	BOOL fileSave(PCSZ filename); 
	LONG queryStrWidth(PSZ pszString,_fontinfo *font,PMPresSpace& ps);
	BOOL helpmsg(PMEvent &event);
	void restoreInitialFormat();
};

BOOL helpHook ( HAB habAnchor, SHORT sMode, USHORT usTopic, USHORT usSubTopic, PRECTL prclPos );

////////////////////////////////////////////////////////////////////////////
// Information and Size dialogs
//
class InfoDlg : public PMModalDialog {
public:
	InfoDlg(HWND hwnd,PMControlMap* ctrlmap,void* data) : PMModalDialog(HWND_DESKTOP,hwnd,DLG_INFO,ctrlmap,data)
		{ }
	BOOL command(USHORT id,USHORT cmddev);
	BOOL initdlg();
};

class SizeDlg : public PMModalDialog {
public:
	SizeDlg(HWND hwnd,PMControlMap* ctrlmap,void* data) : PMModalDialog(HWND_DESKTOP,hwnd,DLG_SIZE,ctrlmap,data)
		{ }
	BOOL initdlg();
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
	void main(void* arg=NULL) ;
};

#endif	 /* RC_INVOKED */

#endif	 /* _TAPELAB_H_ */


