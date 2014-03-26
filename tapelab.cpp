/* 


	tapelab.cpp (emx+gcc) 
	Tape Label Editor Version 0.09

	1995 1996 Giovanni Iachello
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


////////////////////////////////////////////////////////////////////////////


TapeLab::TapeLab(HAB ab,char* file) : PMMainWin("tapelab",ab,helpwin) 
{		
	createArgs->flCreateFlags|=FCF_MENU|FCF_ICON|FCF_ACCELTABLE|FCF_VERTSCROLL;
	createArgs->idResources=ID_TAPELAB;
	createArgs->pszTitle="Tape Lab - (untitled)";
	caption="Tape Lab";
	fnFilter="*.TL";
	flCaption="Open Tape Label File";
	fsCaption="Save Tape Label File";
	fileFlags=PMMWFF_UNTITLED;
	tempfile=file;
	sb=NULL;
//	sizey = vertical size in pixels of the client window
//	posy = vertical position in pixels of the client window viewport
//         in the whole vitual client window
	posy=-1;				  // must be set for the first time
	sizey=-1;
}

TapeLab::~TapeLab()
{
	delete sb;	
}

BOOL TapeLab::createWin() 
{
	PMMainWin::createWin();
	sb=new PMVertScrollBar(this,NULL);
	if (tempfile)
		if (fileOpen(tempfile)) {
			strcpy(filename,tempfile);
			fileFlags=PMMWFF_FILE;
			setTitleCaption();
			tempfile=NULL;
		}
	return TRUE;
}	

BOOL TapeLab::size(SHORT cx,SHORT cy)
{
	if (posy!=-1) {		 // pos is better set the first time by paint().
		posy=posy+sizey-cy;
	}
	sizey=cy;	   // update window dimentions          
	return TRUE;
}

BOOL TapeLab::other(PMEvent &event)
{
	if (event.msg==WM_VSCROLL) {	  // gestione scroll bar
		PMWindowPresSpace ps(this);	  // crea pres space della finestra
		SIZEL size;
		size.cx=1500;
		size.cy=fixpoints[labelformat.size][0].y+100;
		ps.set(&size,PU_LOMETRIC);
		PMRect q;
		ps.queryPageViewport(&q);
		switch (SHORT2FROMMP(event.mp2) ) {
			case SB_SLIDERPOSITION: {
				posy=q.yTop-sizey-SHORT1FROMMP(event.mp2);
				invalidate(TRUE);		
				return TRUE;
			}
			case SB_LINEUP: {
				posy=min(q.yTop-sizey,posy+10);
				invalidate(TRUE);		
				return TRUE;
			}
			case SB_LINEDOWN: {
				posy=max(0,posy-10);
				invalidate(TRUE);		
				return TRUE;
			}
			case SB_PAGEUP: {
				SWP swp;
				queryPos(&swp);
				posy=min(q.yTop-sizey,posy+swp.cy);
				invalidate(TRUE);		
				return TRUE;
			}
			case SB_PAGEDOWN: {
				SWP swp;
				queryPos(&swp);
				posy=max(0,posy-swp.cy);
				invalidate(TRUE);		
				return TRUE;
			}
		}
	}
	return FALSE;
}	

BOOL TapeLab::command(USHORT id,USHORT cmddev)
{
	switch (id) {
/////////////////////////////////////////////////////////////////////////////
// Menu selection IDM_TITLE : user wants to change label title
//
   	    case IDM_TITLE:	{
			struct _temp {
				char t1[200];
				char t2[200];
			};
			_temp *temp=new _temp;
			strcpy(temp->t1,labelformat.title1);
			strcpy(temp->t2,labelformat.title2);
			static PMControlMap titlectrlmap[]={
				cmEntryField(DTITLE_EF_T1,_temp,t1)
				cmEntryField(DTITLE_EF_T2,_temp,t2)
				cmEnd(DTITLE_EF_T1)
			};
			PMModalDialog titledlg(HWND_DESKTOP,hwnd,DLG_TITLE,titlectrlmap,temp);
			int ret=titledlg.createWin();
			if (ret) {
				delete labelformat.title1;
				delete labelformat.title2;
				labelformat.title1=new char[strlen(temp->t1)+1];
				labelformat.title2=new char[strlen(temp->t2)+1];
				strcpy(labelformat.title1,temp->t1);
				strcpy(labelformat.title2,temp->t2);
				setModified(TRUE);
				invalidate(TRUE);
			}
			delete temp;
			return TRUE;
		}
/////////////////////////////////////////////////////////////////////////////
// Menu selection IDM_TEXT : user wants to change label text
//
   	    case IDM_TEXT:	{
			struct _temp1 {
				char t1[3000];
				char t2[3000];
			};
			_temp1 *temp=new _temp1;
			strcpy(temp->t1,labelformat.text1);
			strcpy(temp->t2,labelformat.text2);

			static PMControlMap textctrlmap[]={
				cmMultiLineEdit(DTEXT_MLE_T1,_temp1,t1)
				cmMultiLineEdit(DTEXT_MLE_T2,_temp1,t2)
				cmEnd(DTEXT_MLE_T1)
			};
			PMModalDialog textdlg(HWND_DESKTOP,hwnd,DLG_TEXT,textctrlmap,temp);
			int ret=textdlg.createWin();
			if (ret) {
				delete labelformat.text1;
				delete labelformat.text2;
				labelformat.text1=new char[strlen(temp->t1)+1];
				labelformat.text2=new char[strlen(temp->t2)+1];
				strcpy(labelformat.text1,temp->t1);
				strcpy(labelformat.text2,temp->t2);
				setModified(TRUE);
				invalidate(TRUE);
			}
			delete temp;
			return TRUE;
		}
/////////////////////////////////////////////////////////////////////////////
// Menu selection IDM_SIZE : user wants to change label size
//
   	    case IDM_SIZE:	{
			struct _temp2 {
				int size;
				int flaps;
				char width[32];
				char flap1[32];
				char flap2[32];
				char flap3[32];
				char flap4[32];
			} temp;
			temp.size=labelformat.size;
			temp.flaps=labelformat.flaps;
			sprintf(temp.width,"%d",labelformat.customsize.width);
			sprintf(temp.flap1,"%d",labelformat.customsize.flap[0]);
			sprintf(temp.flap2,"%d",labelformat.customsize.flap[1]);
			sprintf(temp.flap3,"%d",labelformat.customsize.flap[2]);
			sprintf(temp.flap4,"%d",labelformat.customsize.flap[3]);
			static PMControlMap sizectrlmap[]={
				cmRadioButtonGroup(DSIZE_RB_NORMAL,_temp2,size)
				cmSpinButton(DSIZE_SB_FLAPS,_temp2,flaps)
				cmEntryField(DSIZE_EF_WIDTH,_temp2,width)
				cmEntryField(DSIZE_EF_FLAP1,_temp2,flap1)
				cmEntryField(DSIZE_EF_FLAP2,_temp2,flap2)
				cmEntryField(DSIZE_EF_FLAP3,_temp2,flap3)
				cmEntryField(DSIZE_EF_FLAP4,_temp2,flap4)
				cmEnd(DSIZE_RB_NORMAL)
			};
			SizeDlg sizedlg(hwnd, sizectrlmap, &temp);
			int ret=sizedlg.createWin();
			if (ret) {
				labelformat.size=temp.size;
				labelformat.flaps=temp.flaps;
				labelformat.customsize.width=strtol(temp.width,NULL,0);
				labelformat.customsize.flap[0]=strtol(temp.flap1,NULL,0);
				labelformat.customsize.flap[1]=strtol(temp.flap2,NULL,0);
				labelformat.customsize.flap[2]=strtol(temp.flap3,NULL,0);
				labelformat.customsize.flap[3]=strtol(temp.flap4,NULL,0);
				labelformat.writecustomfixpoints(); // write new custom points in global data structure..
				setModified(TRUE);
				posy=-1;				// recalculate the scroll bar
				sizey=-1;
				invalidate(TRUE);
			}
			return TRUE;
		}
/////////////////////////////////////////////////////////////////////////////
// Menu selection IDM_INFO : user wants to change label information
//
   	    case IDM_INFO:	{
			struct _temp3 {
				char date1[40];
				int nr1;
				int source1;
				char date2[40];
				int nr2;
				int source2;
				int whatinfo[6];
			} temp;
			strcpy(temp.date1,labelformat.date1);
			strcpy(temp.date2,labelformat.date2);
			temp.nr1=labelformat.nr1;
			temp.nr2=labelformat.nr2;
			temp.source1=labelformat.source1;
			temp.source2=labelformat.source2;
			for (int i=0; i<6; i++) temp.whatinfo[i]=labelformat.whatinfo[i];

			static PMControlMap infoctrlmap[]={
				cmEntryField(DINFO_EF_DATE1,_temp3,date1)
				cmSpinButton(DINFO_SB_SOURCE1,_temp3,source1)
				cmSpinButton(DINFO_SB_NR1,_temp3,nr1)
				cmEntryField(DINFO_EF_DATE2,_temp3,date2)
				cmSpinButton(DINFO_SB_SOURCE2,_temp3,source2)
				cmSpinButton(DINFO_SB_NR2,_temp3,nr2)
				cmCheckBox(DINFO_CB_BASE0,_temp3,whatinfo[0])
				cmCheckBox(DINFO_CB_BASE1,_temp3,whatinfo[1])
				cmCheckBox(DINFO_CB_BASE2,_temp3,whatinfo[2])
				cmCheckBox(DINFO_CB_BASE3,_temp3,whatinfo[3])
				cmCheckBox(DINFO_CB_BASE4,_temp3,whatinfo[4])
				cmCheckBox(DINFO_CB_BASE5,_temp3,whatinfo[5])
				cmEnd(DINFO_EF_DATE1)
			};
			InfoDlg infodlg(hwnd, infoctrlmap, &temp);
			int ret=infodlg.createWin();
			if (ret) {
				delete labelformat.date1;
				delete labelformat.date2;
				labelformat.date1=new char[strlen(temp.date1)+1];
				labelformat.date2=new char[strlen(temp.date2)+1];
				strcpy(labelformat.date1,temp.date1);
				strcpy(labelformat.date2,temp.date2);
				labelformat.nr1=temp.nr1;
				labelformat.nr2=temp.nr2;
				labelformat.source1=temp.source1;
				labelformat.source2=temp.source2;
				for (int i=0; i<6; i++) labelformat.whatinfo[i]=temp.whatinfo[i];
				setModified(TRUE);
				invalidate(TRUE);
			}
			return TRUE;
		}
/////////////////////////////////////////////////////////////////////////////
// Menu selection IDM_FORM : user wants to change label format
//
   	    case IDM_FORM:	{
			struct _temp4 {
				char titlelm[32]; // margine sin. titolo
				char textlm[32],texttm[32]; // margine sinistro e superiore testo
				char tabsize[32]; // dimensione tabs
				char textvert1[32],textvert2[32];	// permillesimi di spaziatura orizzontale e verticale
				char texthoriz1[32],texthoriz2[32];
			};
			_temp4 *temp=new _temp4;
			sprintf(temp->tabsize,"%d",labelformat.form.tabsize);
			sprintf(temp->titlelm,"%d",labelformat.form.titlelm);
			sprintf(temp->textlm,"%d",labelformat.form.textlm);
			sprintf(temp->texttm,"%d",labelformat.form.texttm);
			sprintf(temp->textvert1,"%d",labelformat.form.textvert1);
			sprintf(temp->texthoriz1,"%d",labelformat.form.texthoriz1);
			sprintf(temp->textvert2,"%d",labelformat.form.textvert2);
			sprintf(temp->texthoriz2,"%d",labelformat.form.texthoriz2);
			static PMControlMap formctrlmap[]={
				cmEntryField(DFORM_EF_TITLM,_temp4,titlelm)
				cmEntryField(DFORM_EF_TABSIZE,_temp4,tabsize)
				cmEntryField(DFORM_EF_TLM,_temp4,textlm)
				cmEntryField(DFORM_EF_TTM,_temp4,texttm)
				cmEntryField(DFORM_EF_THSA,_temp4,texthoriz1)
				cmEntryField(DFORM_EF_THSB,_temp4,texthoriz2)
				cmEntryField(DFORM_EF_TVSA,_temp4,textvert1)
				cmEntryField(DFORM_EF_TVSB,_temp4,textvert2)
				cmEnd(DFORM_EF_TITLM)
			};
			PMModalDialog titledlg(HWND_DESKTOP,hwnd,DLG_FORMAT,formctrlmap,temp);
			int ret=titledlg.createWin();
			if (ret) {
				labelformat.form.tabsize=strtol(temp->tabsize,NULL,0);
				labelformat.form.titlelm=strtol(temp->titlelm,NULL,0);
				labelformat.form.textlm=strtol(temp->textlm,NULL,0);
				labelformat.form.texttm=strtol(temp->texttm,NULL,0);
				labelformat.form.textvert1=strtol(temp->textvert1,NULL,0);
				labelformat.form.textvert2=strtol(temp->textvert2,NULL,0);
				labelformat.form.texthoriz1=strtol(temp->texthoriz1,NULL,0);
				labelformat.form.texthoriz2=strtol(temp->texthoriz2,NULL,0);
				setModified(TRUE);
				invalidate(TRUE);
			}
			delete temp;
			return TRUE;
		}
/////////////////////////////////////////////////////////////////////////////
// Menu selection IDM_SAVE : user wants to save settings
//
   	    case IDM_SAVE:	{
   	    	PMUserProfile myprofile;
   	    	myprofile.writeData(SA_APPLNAME,SA_SETTINGSKEY,&labelformat,sizeof(labelformat));
			return TRUE;
		}
/////////////////////////////////////////////////////////////////////////////
// Menu selection IDM_FON_* : user wants to change various fonts
//
   	    case IDM_FON_TIA:	
			setFont(&labelformat.title1font,"Title Side A Font"); 
			return TRUE;
   	    case IDM_FON_TIB:	
			setFont(&labelformat.title2font,"Title Side B Font"); 
			return TRUE;
   	    case IDM_FON_TXA:	
			setFont(&labelformat.text1font,"Text Side A Font");
			return TRUE;
   	    case IDM_FON_TXB:	
			setFont(&labelformat.text2font,"Text Side B Font");
			return TRUE;
   	    case IDM_FON_DATE:	
			setFont(&labelformat.datefont,"Date Font"); 
			return TRUE;
   	    case IDM_FON_SOURCE: 
			setFont(&labelformat.sourcefont,"Source Font");
			return TRUE;
   	    case IDM_FON_NR: 
			setFont(&labelformat.nrfont,"NR Font");
			return TRUE;
	}
	return PMMainWin::command(id,cmddev);
}

BOOL TapeLab::helpmsg(PMEvent &event)
{
	if (event.msg==HM_QUERY_KEYS_HELP) {
		event.ret=MRFROMSHORT(HLP_KEYS_TAPELAB); 
		return TRUE;
	}
	if (event.msg==HM_HELPSUBITEM_NOT_FOUND) {
		// Don't intercept general help 
		if( SHORT1FROMMP( event.mp1 ) != 1 && SHORT2FROMMP( event.mp2 ) != 2 ) 
			ErrBox("IPF Help error.  Topic %lx (subtopic %lx) not found for %s.",
                   (ULONG) SHORT1FROMMP( event.mp2 ),
                   (ULONG) SHORT2FROMMP( event.mp2 ),
                   LONGFROMMP( event.mp1 ) == (LONG)HLPM_WINDOW ? "application window" :
                   (LONGFROMMP( event.mp1 ) == (LONG)HLPM_FRAME ? "frame window" : "menu window" ) );

		// For HLPM_WINDOW or HLPM_FRAME return FALSE
		// for no action or TRUE for extended help 
		event.ret=(MRESULT) TRUE;
		return TRUE;
	}
	return FALSE;
}

BOOL TapeLab::fileNew() 
{
	labelformat.~_labelformat();  // azzera la struttura dati e ricreala!
	labelformat._labelformat();	  // kill the old label, and make a new one.	
	restoreInitialFormat();
	posy=-1;				// recalculate la scroll bar
	sizey=-1;
	invalidate(TRUE);
	return TRUE;
}


BOOL TapeLab::filePrint() 
{
	char buf[300];

	if (fileFlags==PMMWFF_FILE) sprintf(buf,"Tape Lab Document: %s",filename);
	else sprintf(buf,"Tape Lab Document: %s",fileUntitled);

	new PMPrintLabelThread(buf,this);
	
	return TRUE;
}

BOOL TapeLab::fileOpen(PCSZ filename) 
{
	if (labelformat.load(filename)) {
		posy=-1;				// recalculate la scroll bar
		sizey=-1;
		invalidate(TRUE);
		return TRUE;
	}
	ErrBox("Error Loading file %s",filename);

	return FALSE;
}

BOOL TapeLab::fileSave(PCSZ filename) 
{
	labelformat.save(filename);
	invalidate(TRUE);
	return TRUE;
}

// if application is quitting, store the window position
void TapeLab::saveApplication()
{
	storeWindowPos(SA_APPLNAME,SA_SWPKEY);
}

BOOL TapeLab::create(PMEvent &)
{
	// if possible restore window position. If not possible, set initial position.
	BOOL bSuccess;
	bSuccess=restoreWindowPos(SA_APPLNAME,SA_SWPKEY);
	if (bSuccess) setFramePos(HWND_TOP,0,0,0,0,SWP_ACTIVATE|SWP_SHOW);
	else setFramePos(HWND_TOP,10,10,400,450,SWP_ACTIVATE|SWP_SHOW|SWP_MOVE|SWP_SIZE);
	// if present, load initial format from ini file..
	restoreInitialFormat();
	return TRUE;	
}

void TapeLab::restoreInitialFormat()
{
	_labelformat locallabelformat;
   	BOOL bSuccess;
   	PMUserProfile myprofile;                       // open user profile
	ULONG buffersize = sizeof(labelformat);        // load profile data
	bSuccess = myprofile.queryData(SA_APPLNAME,SA_SETTINGSKEY,&locallabelformat,&buffersize);
	if (bSuccess) {
		// tell labelformat to update selected data fields.
		labelformat.newformat(&locallabelformat);
	}
}


