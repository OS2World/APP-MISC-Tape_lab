/* 


	tl_core.cpp (emx+gcc) 
	Tape Label Editor Version 0.09

	(C) 1996 Giovanni Iachello
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

#include "tapelab.h"

_labelformat::_labelformat()
{
	flaps=4;
	size=0; 
	title1=new char; *title1='\0';
	title2=new char; *title2='\0';
	text1=new char; *text1='\0';
	text2=new char; *text2='\0';
	date1=new char; *date1='\0';
	date2=new char; *date2='\0';
	source1=0;
	source2=0;
	nr1=0;
	nr2=0;
	for (int i=0; i<6; i++) whatinfo[i]=1;
	
	// init title fonts FATTRS and info
	memset(&title1font,0,sizeof(title1font));
	title1font.fattrs.usRecordLength = sizeof(FATTRS); /* sets size of structure   */
	title1font.fattrs.usCodePage = 850;        /* code-page 850                    */
	title1font.fattrs.fsFontUse = FATTR_FONTUSE_NOMIX;/* doesn't mix with graphics */
	strcpy(title1font.fattrs.szFacename ,"Helvetica");
	title1font.point=12;
	title1font.height=16;

	title2font=title1font;

	text1font=title1font;
	text1font.point=10;
	text1font.height=12;
	text2font=text1font;
	datefont=text1font;
	sourcefont=text1font;
	nrfont=text1font;

	form.textvert1=form.textvert2=1100;
	form.texthoriz1=form.texthoriz2=1000;
	form.textlm=20;
	form.texttm=20;
	form.tabsize=100;
	form.titlelm=30;
	
	customsize.width=1010;
	customsize.flap[0]=160;
	customsize.flap[1]=130;
	customsize.flap[2]=645;
	customsize.flap[3]=640;
	writecustomfixpoints();
}

_labelformat::~_labelformat()
{
	delete title1;
	delete title2;
	delete text1;
	delete text2;
	delete date1;
	delete date2;
}

char* filebanner="Tape Lab data file\x1a";

char version=0;

#define fwritestr(a,f) { \
	int len = strlen(a) + 1; \
	fwrite(&len,sizeof(len),1,f); \
	fwrite(a,len,1,f); \
}						

#define freadstr(a,f) { \
	int len; \
	fread(&len,sizeof(len),1,f); \
	a=new char[len]; \
	fread(a,len,1,f); \
}						

BOOL _labelformat::save(const char* file)
{
	FILE *f=fopen(file,"wb");
	if ( !f ) return FALSE;
	fwrite(filebanner,strlen(filebanner)+1,1,f);
	fwrite(&version,sizeof(version),1,f);
	fwrite(&size,sizeof(size),1,f);
	fwrite(&flaps,sizeof(flaps),1,f);
	fwrite(&title1font,sizeof(title1font),1,f);
	fwrite(&title2font,sizeof(title2font),1,f);
	fwrite(&text1font,sizeof(text1font),1,f);
	fwrite(&text2font,sizeof(text2font),1,f);
	fwrite(&datefont,sizeof(datefont),1,f);
	fwrite(&sourcefont,sizeof(sourcefont),1,f);
	fwrite(&nrfont,sizeof(nrfont),1,f);
	fwrite(&form,sizeof(form),1,f);
	fwrite(&source1,sizeof(source1),1,f);
	fwrite(&source2,sizeof(source2),1,f);
	fwrite(&nr1,sizeof(nr1),1,f);
	fwrite(&nr2,sizeof(nr2),1,f);
	fwrite(whatinfo,sizeof(int),sizeof(6),f);

	fwritestr(title1,f);
	fwritestr(title2,f);
	fwritestr(text1,f);
	fwritestr(text2,f);
	fwritestr(date1,f);
	fwritestr(date2,f);

	fwrite(&customsize,sizeof(customsize),1,f);

	if (fclose(f)) return FALSE;
	return TRUE;
}

BOOL _labelformat::load(const char* file)
{
	delete title1;
	delete title2;
	delete text1;
	delete text2;
	delete date1;
	delete date2;
	FILE *f=fopen(file,"rb");
	if ( !f ) { _labelformat(); return FALSE; }

	char *signature=new char[strlen(filebanner)+2];
	fread(signature,strlen(filebanner)+2,1,f);
	// controlla che sia mia!
	if ( strcmp(filebanner,signature) ) { 
		_labelformat(); 
		delete signature; 
		return FALSE;	
	}			
	// controlla la versione
	if ( signature[strlen(filebanner)+1] != version ) { 
		_labelformat(); 
		delete signature;
		return FALSE; 
	}

	fread(&size,sizeof(size),1,f);
	fread(&flaps,sizeof(flaps),1,f);
	fread(&title1font,sizeof(title1font),1,f);
	fread(&title2font,sizeof(title2font),1,f);
	fread(&text1font,sizeof(text1font),1,f);
	fread(&text2font,sizeof(text2font),1,f);
	fread(&datefont,sizeof(datefont),1,f);
	fread(&sourcefont,sizeof(sourcefont),1,f);
	fread(&nrfont,sizeof(nrfont),1,f);
	fread(&form,sizeof(form),1,f);
	fread(&source1,sizeof(source1),1,f);
	fread(&source2,sizeof(source2),1,f);
	fread(&nr1,sizeof(nr1),1,f);
	fread(&nr2,sizeof(nr2),1,f);
	fread(whatinfo,sizeof(int),sizeof(6),f);

	freadstr(title1,f);
	freadstr(title2,f);
	freadstr(text1,f);
	freadstr(text2,f);
	freadstr(date1,f);
	freadstr(date2,f);

	fread(&customsize,sizeof(customsize),1,f);
	writecustomfixpoints();

	delete signature;
	fclose(f);
	return TRUE;
}

void _labelformat::newformat(_labelformat* nf)
{
	// load format information from newformat 
	// do not update text fields...
	flaps=nf->flaps;
	size=nf->size; 
	for (int i=0; i<6; i++) whatinfo[i]=nf->whatinfo[i];
	
	title1font=nf->title1font;

	title2font=nf->title2font;

	text1font=nf->text1font;
	text2font=nf->text2font;
	datefont=nf->datefont;
	sourcefont=nf->sourcefont;
	nrfont=nf->nrfont;

	form=nf->form;
	customsize=nf->customsize;
	writecustomfixpoints();
}

void _labelformat::writecustomfixpoints() // write fixpoints structure with customsize data
{
	PMPoint pos(100,100);
	fixpoints[3][8]=pos;
	fixpoints[3][9]=pos;
	fixpoints[3][9].x+=customsize.width;
	for (int i=3; i>=0; i--)
	{
		pos.y+=customsize.flap[i];
		fixpoints[3][i*2]=pos;
		fixpoints[3][i*2+1]=pos;
		fixpoints[3][i*2+1].x+=customsize.width;
	}
}

// label size:
// 0 - vecchio tipo 3/4 flaps
// 1 - cassette TDK nuove (piccole) 3 flaps
// 2 - cassette Sony nuove (piccole) 2 flaps

POINTL fixpoints[4][10] = { 
	{
		{ 100,1675}, { 1110,1675},
		{ 100,1515}, { 1110,1515},
		{ 100,1385}, { 1110,1385},
		{ 100,740 }, { 1110,740 },
		{ 100,100 }, { 1110,100 }
	},
	{
		{ 100,970 }, { 1110,970 },
		{ 100,870 }, { 1110,870 },
		{ 100,745 }, { 1110,745 },
		{ 100,100 }, { 1110,100 },
		{ 100,100 }, { 1110,100 }
	},
	{
		{ 100,840 }, { 1120,840 },
		{ 100,840 }, { 1120,840 },
		{ 100,740 }, { 1120,740 },
		{ 100,100 }, { 1120,100 },
		{ 100,100 }, { 1120,100 }
	},
	{
		{ 100,840 }, { 1120,840 },
		{ 100,840 }, { 1120,840 },
		{ 100,740 }, { 1120,740 },
		{ 100,100 }, { 1120,100 },
		{ 100,100 }, { 1120,100 }
	}
};

// TIPI DI NR

char *apchSources[]={ "CD","LP","MC","Radio","Master","Live","VTR" };
char *apchNR[]={ "None","Dolby","Dolby B","Dolby C","Dolby S","dbx" };

