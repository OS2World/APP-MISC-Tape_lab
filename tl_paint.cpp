/* 


	tl_paint.cpp (emx+gcc) 
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
#include "pmdlg.h"
#include "pmgpi.h"
#include "pmstdres.h"



BOOL TapeLab::paint(PMPresSpace& ps)
{
	PMRect rcl;
	rcl=this;

	// inizializzazione
	ps.fillRect(&rcl,CLR_PALEGRAY);
	SIZEL size;
	size.cx=1500;
	size.cy=fixpoints[labelformat.size][0].y+100;
	ps.set(&size,PU_LOMETRIC);

	PMRect q;
	ps.queryPageViewport(&q);

	if ( sizey==-1 ) {
		sizey=rcl.yTop;
	} 
	if ( posy==-1 ) {
		posy=q.yTop-sizey;
	} 
	PMRect r(0,-posy,q.xRight,q.yTop-posy);
	ps.setPageViewport(&r);
	sb->setScrollBar(q.yTop-sizey-posy,0,q.yTop-sizey);
	sb->setThumbSize(sizey,q.yTop);
		
	// scatola bianca | white box
	ps.setColor(CLR_WHITE);
	ps.move(&fixpoints[labelformat.size][0]);
	ps.box(DRO_FILL,&fixpoints[labelformat.size][labelformat.flaps*2 +1]);
	return paintlabel(ps);
}


// parte in comune fra stampante e schermo

BOOL TapeLab::paintlabel(PMPresSpace& ps) 
{
	
	ps.setColor(CLR_BLACK);

	// righe orizzontali
	int i;
	for (i=0; i<=labelformat.flaps; i++) {
		if (i>0 && i<labelformat.flaps) ps.setLineType(LINETYPE_DOT); 
			else ps.setLineType(LINETYPE_DEFAULT);
		ps.move(&fixpoints[labelformat.size][i*2]);
		ps.line(&fixpoints[labelformat.size][i*2+1]);
	}
	// righe verticali
	ps.move(&fixpoints[labelformat.size][0]);
	ps.line(&fixpoints[labelformat.size][ labelformat.flaps * 2 ]);
	ps.move(&fixpoints[labelformat.size][1]);
	ps.line(&fixpoints[labelformat.size][ ( labelformat.flaps * 2 ) + 1 ]);

	// esegue clip dentro a rettangolo dell'etichetta!
	PMRect clip(fixpoints[labelformat.size][labelformat.flaps*2],fixpoints[labelformat.size][1]);
	ps.intersectClipRectangle(&clip);

	// titolo
	PMPoint a;
	if (labelformat.title2[0] != '\0') { // ci sono 2 titoli
		a.y=(fixpoints[labelformat.size][2].y+fixpoints[labelformat.size][4].y)/2;
		a.x=fixpoints[labelformat.size][2].x+labelformat.form.titlelm;
		a.y=a.y+ (((fixpoints[labelformat.size][2].y-fixpoints[labelformat.size][4].y)/2)-pts2dmm(labelformat.title1font.height))/2 ;
		paintString(labelformat.title1,&labelformat.title1font,1000L,a,ps);

		a.y=fixpoints[labelformat.size][4].y; 
		a.x=fixpoints[labelformat.size][2].x+labelformat.form.titlelm;
		a.y=a.y+ (((fixpoints[labelformat.size][2].y-fixpoints[labelformat.size][4].y)/2)-pts2dmm(labelformat.title2font.height))/2 ;
		paintString(labelformat.title2,&labelformat.title2font,1000L,a,ps);
	} else { // c'e' un solo titolo
		a.x=fixpoints[labelformat.size][2].x+labelformat.form.titlelm;
		a.y=(fixpoints[labelformat.size][2].y+fixpoints[labelformat.size][4].y-pts2dmm(labelformat.title1font.height)) / 2 ;
		paintString(labelformat.title1,&labelformat.title1font,1000L,a,ps);
	}
	// testo
	a.y=fixpoints[labelformat.size][4].y-pts2dmm(labelformat.text1font.height)-labelformat.form.texttm; 
	a.x=fixpoints[labelformat.size][4].x+labelformat.form.textlm;
	paintString(labelformat.text1,&labelformat.text1font,labelformat.form.textvert1,a,ps);

	a.y=fixpoints[labelformat.size][4].y-pts2dmm(labelformat.text1font.height)-labelformat.form.texttm; 
	a.x=(fixpoints[labelformat.size][4].x+fixpoints[labelformat.size][5].x)/2+labelformat.form.textlm;
	paintString(labelformat.text2,&labelformat.text2font,labelformat.form.textvert2,a,ps);

	
	if (labelformat.size != 2 && labelformat.flaps > 2) {
		a.y=fixpoints[labelformat.size][0].y-pts2dmm(labelformat.datefont.height)-15; 
		// data A
		if (labelformat.whatinfo[0]) {
			a.x=fixpoints[labelformat.size][0].x+20;
			paintString(labelformat.date1,&labelformat.datefont,1000L,a,ps);
		}
		// data B
		if (labelformat.whatinfo[1]) {
			a.x=fixpoints[labelformat.size][1].x-queryStrWidth(labelformat.date2,&labelformat.datefont,ps)-20;
			paintString(labelformat.date2,&labelformat.datefont,1000L,a,ps);
		}
		// "DATE"
		if (labelformat.whatinfo[1] || labelformat.whatinfo[0]) {
			a.x=(fixpoints[labelformat.size][0].x+fixpoints[labelformat.size][1].x-queryStrWidth("DATE",&labelformat.datefont,ps))/2;
			paintString("DATE",&labelformat.datefont,1000L,a,ps);
			a.y-=pts2dmm(labelformat.sourcefont.height); // se c'era uno dei due abbassati...
		}

		// source A
		if (labelformat.whatinfo[4]) {
			a.x=fixpoints[labelformat.size][0].x+20;
			paintString(apchSources[labelformat.source1],&labelformat.sourcefont,1000L,a,ps);
		}
		// source B			
		if (labelformat.whatinfo[5]) {
			a.x=fixpoints[labelformat.size][1].x-queryStrWidth(apchSources[labelformat.source2],&labelformat.sourcefont,ps)-20;
			paintString(apchSources[labelformat.source2],&labelformat.sourcefont,1000L,a,ps);
		}
		// "SOURCE"
		if (labelformat.whatinfo[4] || labelformat.whatinfo[5]) {
			a.x=(fixpoints[labelformat.size][0].x+fixpoints[labelformat.size][1].x-queryStrWidth("SOURCE",&labelformat.sourcefont,ps))/2;
			paintString("SOURCE",&labelformat.sourcefont,1000L,a,ps);
			a.y-=pts2dmm(labelformat.nrfont.height); 
		}

		// NR A
		if (labelformat.whatinfo[2]) {
			a.x=fixpoints[labelformat.size][0].x+20;
			paintString(apchNR[labelformat.nr1],&labelformat.nrfont,1000L,a,ps);
		}
		// NR B
		if (labelformat.whatinfo[3]) {
			a.x=fixpoints[labelformat.size][1].x-queryStrWidth(apchNR[labelformat.nr2],&labelformat.nrfont,ps)-20;
			paintString(apchNR[labelformat.nr2],&labelformat.nrfont,1000L,a,ps);
		}
		// "NR"
		if (labelformat.whatinfo[2] || labelformat.whatinfo[3]) {
			a.x=(fixpoints[labelformat.size][0].x+fixpoints[labelformat.size][1].x-queryStrWidth("NR",&labelformat.nrfont,ps))/2;
			paintString("NR",&labelformat.sourcefont,1000L,a,ps);
		}
	}
	return TRUE;
}

BOOL TapeLab::paintString(char* str,_fontinfo *font,int spc,PMPoint pos,PMPresSpace &ps)
{
	FONTMETRICS fm;
	PMLogFont f(ps,&font->fattrs);
	ps.setCharSet(f);

	SIZEF csize;
	csize.cy=MAKEFIXED(font->height*254L/72L,0);   // [punti] * [decimillimetri]/[pollice] * 1/[punti]/[pollice]
	csize.cx=(font->height << 16)*254L/72L;
	ps.setCharBox(&csize);
	ps.queryFontMetrics(&fm);	  // chiedi i fontmetrics

	pos.y += fm.lMaxDescender;    // e alza il font a causa del descender
	
	char* p=str,*start;
	int line=0,x=0,pageadj=0;
	while (*p) {		// finche' la str non finisce
		int len=0;
		start=p;
		while (*p!=0xd && *p!=0x9 && *p) { len++; p++; }  // cerca newline, tab, o fine str
		PMPoint off(x,-pageadj-( line*( ( font->height * spc / 1000L ) + font->extleading ) *254L /72L)  );	   // calcola posizione
		off=off+pos;

		if (off.y-fm.lMaxDescender<fixpoints[labelformat.size][6].y && pageadj==0) { // siamo passati sulla nuova pagina, aggiusta di conseguenza...
			pageadj=off.y-(pos.y-fixpoints[labelformat.size][4].y+fixpoints[labelformat.size][6].y);
			off.y-=pageadj;
		}		
		ps.charStringAt(&off,len,start);		   // stampa

		if (*p==0x9) { // tabba
			POINTL aptlPoints[TXTBOX_COUNT];
			p++;
			ps.queryTextBox (len,start, TXTBOX_COUNT, aptlPoints);
			x+=(aptlPoints[TXTBOX_TOPRIGHT].x/labelformat.form.tabsize + 1 )* labelformat.form.tabsize; 

		} else {
			line++;
			x=0;		// ritorno a capo significa eliminare offset x
			if (*p==0xd) p++;					   // skippa 0xd
			if (*p==0xa) p++;					   // skippa 0xa
		}
	}
	return TRUE;
}

LONG TapeLab::queryStrWidth(PSZ pszString,_fontinfo *font,PMPresSpace &ps)
{
	PMLogFont f(ps,&font->fattrs);
	ps.setCharSet(f);
	return ps.queryTextBoxWidth(pszString);
}

BOOL TapeLab::setFont(_fontinfo *font,char* title)
{
	FONTMETRICS fm;
	PMWindowPresSpace ps(this);	  // crea pres space della finestra
	SIZEL size;
	size.cx=1500;
	size.cy=1800;
	ps.set(&size,PU_LOMETRIC);
	PMLogFont f(ps,&font->fattrs);		  // crea font
	ps.setCharSet(f);			  // imposta font

	SIZEF csize;
	csize.cy=(font->height << 16)*254L/72L;
	csize.cx=(font->height << 16)*254L/72L;
	ps.setCharBox(&csize);

	ps.queryFontMetrics(&fm);	  // chiedi i fontmetrics
	fm.sNominalPointSize=font->point*10;
	PMFontDialog fd(HWND_DESKTOP,hwnd, &fm,font->type,title,"Preview");
	int ret=fd.createWin();		  // fai scegliere all'utente
	FONTDLG fi=fd;
	if ( ret == DID_OK ) {
		font->fattrs=fi.fAttrs; 
		font->height=fi.lEmHeight;
		font->extleading=fi.lExternalLeading;
		font->type=fi.flType;
		font->point=FIXEDINT(fi.fxPointSize);
		font->fattrs.lMaxBaselineExt = 0L;
		font->fattrs.lAveCharWidth = 0L; 
		setModified(TRUE);
		invalidate(TRUE);
		return TRUE;
	}
	return FALSE;
}


