/*--------------------------------------------------------------------------
			
	6x6_dlgf.cpp - Functions for W32 compatible dialog routines
			
                   Doug Rogers, 09/04/95
			
Changes:
--------
09/04/95 16:55 - First creation 
			
Functions:
----------
MakeAboutDlg(HWND hWnd)
AboutDlg_OldDlgProc(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
AboutDlg_DlgProc( HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
AboutDlg_OnInitDialog( HWND hDlg, HWND hwndFocus, LPARAM lParam)
AboutDlg_OnCommand( HWND hDlg, int iCmd, HWND hwndCntl, UINT uCodeNotify)
MakeShortCutsDlg(HWND hWnd)
ShortCutsDlg_OldDlgProc(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
ShortCutsDlg_DlgProc( HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
ShortCutsDlg_OnInitDialog( HWND hDlg, HWND hwndFocus, LPARAM lParam)
ShortCutsDlg_OnCommand( HWND hDlg, int iCmd, HWND hwndCntl, UINT uCodeNotify)
MakeNotesAndTipsDlg(HWND hWnd)
NotesAndTipsDlg_OldDlgProc(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
NotesAndTipsDlg_DlgProc( HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
NotesAndTipsDlg_OnInitDialog( HWND hDlg, HWND hwndFocus, LPARAM lParam)
NotesAndTipsDlg_OnCommand( HWND hDlg, int iCmd, HWND hwndCntl, UINT uCodeNotify)
Make*DLGNAME*(HWND hWnd)
*DLGNAME*_OldDlgProc(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
*DLGNAME*_DlgProc( HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
*DLGNAME*_OnInitDialog( HWND hDlg, HWND hwndFocus, LPARAM lParam)
*DLGNAME*_OnCommand( HWND hDlg, int iCmd, HWND hwndCntl, UINT uCodeNotify)

Macros:
-------
AboutDlg_DefProc( hWnd, uMessage, wParam, lParam) 
NotesAndTipsDlg_DefProc( hWnd, uMessage, wParam, lParam) 
ShortCutsDlg_DefProc( hWnd, uMessage, wParam, lParam)
*DLGNAME*_DefProc( hWnd, uMessage, wParam, lParam)
			
--------------------------------------------------------------------------*/
#define WINVER 0x30A
			
#define STRICT          //enforce strict type matching
#include <windows.h>                                  
#include <windowsx.h>   //for extra macros and Win32 stuff                               
						
#include <stdio.h>      //for sprintf functions
#include <stdlib.h>     //for rand(), srand(), other standard things
						
#include "6x6defs.h"
//#include "6x6cldef.h"
#include "resource.h"

//This is a macro to make the windowsx.h macro look like a DefWndProc                                                                     
#define AboutDlg_DefProc( hWnd, uMessage, wParam, lParam) \
	 DefDlgProcEx( hWnd, uMessage, wParam, lParam, &gbAboutDlgRecursing)	

#define NotesAndTipsDlg_DefProc( hWnd, uMessage, wParam, lParam) \
	 DefDlgProcEx( hWnd, uMessage, wParam, lParam, &gbNotesAndTipsDlgRecursing)	
						
#define ShortCutsDlg_DefProc( hWnd, uMessage, wParam, lParam) \
	 DefDlgProcEx( hWnd, uMessage, wParam, lParam, &gbNotesAndTipsDlgRecursing)	
						
//#define *DLGNAME*_DefProc( hWnd, uMessage, wParam, lParam) \
//			DefDlgProcEx( hWnd, uMessage, wParam, lParam, &gb*DLGNAME*Recursing)	
						
/*++++++ Function Prototypes +++++++++++++++++++++++++++++++++++++++++++++*/
#include "6x6fndef.h"     //Header file with all function prototypes
			
/*++++++ Global Variables ++++++++++++++++++++++++++++++++++++++++++++++++*/
//extern float gfVersion;
extern char gszAppName[];
extern char gszFullAppName[];
extern char gszDEBUGBuff[];     //Buffer used for debugging messages
extern char gszMessageBuff[];   //Buffer used for message boxes

/*------------------ Local globals ---------------------------------------*/
static BOOL gbAboutDlgRecursing = FALSE;          //To abort recursive calls
static BOOL gbNotesAndTipsDlgRecursing = FALSE; //To abort recursive calls
static BOOL gbShortCutsDlgRecursing = FALSE;    //To abort recursive calls
//static BOOL gb*DLGNAME*Recursing = FALSE;       //To abort recursive calls

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
			
/*
sprintf(gszMessageBuff," ",LinesToScroll);
MessageBox (hwnd, gszMessageBuff, gszFullAppName, MB_OK);
*/
			                                                                     

/////////// "AboutDlg" Dialog procedures ////////////////////////////////////
			
//=============== MakeAboutDlg ==============================================
// Purpose: Make the AboutDlg dialog box, activate it, then release the 
//				ProcInst when done
//===========================================================================
void MakeAboutDlg(HWND hWnd)
{
	DLGPROC lpfnAboutDlg;
	HINSTANCE hInst=GetWindowInstance(hWnd);
				
	lpfnAboutDlg=(DLGPROC)MakeProcInstance( 
													(FARPROC)AboutDlg_OldDlgProc, hInst);
				                                                     
	//Show the dialog box	                                                     
	DialogBoxParam( hInst, "HELP_ABOUTDLG", hWnd, lpfnAboutDlg, 0L);
				
	FreeProcInstance( (FARPROC)lpfnAboutDlg);
	return;
}                                                                      
			

//=============== AboutDlg_OldDlgProc ======================================
// Purpose: This is the "Old Style" dialog interface, with two macros 
//          from windowsx.h to check the recursion flag, and send back 
//          LRESULTS if necessary
//==========================================================================
BOOL FAR PASCAL AboutDlg_OldDlgProc(HWND hDlg, UINT uMessage,
												 WPARAM wParam, LPARAM lParam)
{
	CheckDefDlgRecursion( &gbAboutDlgRecursing );	
				
	return(SetDlgMsgResult( hDlg, uMessage, 
								AboutDlg_DlgProc(hDlg, uMessage, wParam, lParam) ) );
}
			                                                                     

//=============== AboutDlg_DlgProc =========================================
// Purpose: This is the "New Style" dialog proc, looking a lot like a WndProc
//				it handles the two messages AboutDlg might get: Init and Command
//==========================================================================
LRESULT AboutDlg_DlgProc( HWND hDlg, UINT uMessage, 
															WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
		HANDLE_MSG(hDlg, WM_INITDIALOG, AboutDlg_OnInitDialog);
					
		HANDLE_MSG(hDlg, WM_COMMAND, AboutDlg_OnCommand);
					
		default:
			return( AboutDlg_DefProc(hDlg, uMessage, wParam, lParam) );
	}
}
			

//=============== AboutDlg_OnInitDialog ====================================
// Purpose: Initalize dialog box, mainly by adding the program version 
//				number to the dialog box IDD_VERSION_NUM control
//==========================================================================
BOOL AboutDlg_OnInitDialog( HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	char szBuff[20];
	float gfVersion=2.0;
							
	sprintf(szBuff, "Version %4.2f", gfVersion);
							
	SetDlgItemText(hDlg, IDD_VERSION_NUM, szBuff);
			
	return TRUE;
}
			

//=============== AboutDlg_OnCommand =======================================
// Purpose: Process command messages which, for the AboutDlg box, 
//	is just "OK"
//==========================================================================
void AboutDlg_OnCommand( HWND hDlg, int iCmd, 
														HWND hwndCntl, UINT uCodeNotify)
{
	switch(iCmd) 
		{
		case OK:
			EndDialog(hDlg, TRUE);
			return;
		}
}			



/////////// ShortCutsDlg Dialog procedures /////////////////////////////////


//=============== MakeShortCutsDlg =========================================
// Purpose: Make the ShortCutsDlg dialog box, activate it, then release the 
//				ProcInst when done
//==========================================================================
void MakeShortCutsDlg(HWND hWnd)
{
	DLGPROC lpfnShortCutsDlg;
	HINSTANCE hInst=GetWindowInstance(hWnd);
				
	lpfnShortCutsDlg=(DLGPROC)MakeProcInstance( 
											(FARPROC)ShortCutsDlg_OldDlgProc, hInst);
				                                                     
	//Show the dialog box	                                                     
	DialogBox( hInst, "HELP_SHORTCUTSDLG", hWnd, lpfnShortCutsDlg);
				
	FreeProcInstance( (FARPROC)lpfnShortCutsDlg);
	return;
}                                                                      
			

//=============== ShortCutsDlg_OldDlgProc ==================================
// Purpose: This is the "Old Style" dialog interface, with two macros 
//         from windowsx.h to check the recursion flag, and send back 
//         LRESULTS if necessary
//==========================================================================
BOOL FAR PASCAL ShortCutsDlg_OldDlgProc(HWND hDlg, UINT uMessage,
												 WORD wParam, LPARAM lParam)
{
	CheckDefDlgRecursion( &gbShortCutsDlgRecursing );	
				
	return( SetDlgMsgResult( hDlg, uMessage, 
							ShortCutsDlg_DlgProc(hDlg, uMessage, wParam, lParam) ) );
}
			                                                                     

//=============== ShortCutsDlg_DlgProc =====================================
// Purpose: This is the "New Style" dialog proc, looking a lot like a WndProc
//				it handles the two messages AboutDlg might get: Init and Command
//==========================================================================
LRESULT ShortCutsDlg_DlgProc( HWND hDlg, UINT uMessage, 
															WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
		HANDLE_MSG(hDlg, WM_INITDIALOG, ShortCutsDlg_OnInitDialog);
					
		HANDLE_MSG(hDlg, WM_COMMAND, ShortCutsDlg_OnCommand);
					
		default:
			return( ShortCutsDlg_DefProc(hDlg, uMessage, wParam, lParam) );
	}
}
			

//=============== ShortCutsDlg_OnInitDialog ================================
// Purpose: Initialize the dialog box by loading the name of the ShortCuts
//				TEXT resource for this dialog, then loading the text itself, then
//				changing the last character of the text (\) to '/0' and then 
//				pouring the text into the dialog box's read only edit control.
//==========================================================================
BOOL ShortCutsDlg_OnInitDialog( HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	HINSTANCE	hInst=GetWindowInstance(hDlg);
	char szDlgTextRsrcName[100];  //For the name of the dialog's TEXT resource
	HRSRC hFoundResource;
	
	static HGLOBAL hResource;
	char far	*lpText;
	char far	*lpStartOfText;

	// Load the name of the ShortCuts TEXT resource...
   LoadString(hInst, 
       IDS_SHORTCUTS_RESNAME, szDlgTextRsrcName, sizeof(szDlgTextRsrcName) );

	// Now load the text itself
	hFoundResource=FindResource(hInst, szDlgTextRsrcName, "TEXT");
	hResource=LoadResource( hInst, hFoundResource);
	lpStartOfText=lpText = (char far *) LockResource(hResource);
			
	while(*lpText != '\\' && *lpText != '\0') //Find the "\" char
		lpText = AnsiNext(lpText);             // indicating end of text
	*lpText='\0';

	SetDlgItemText(hDlg, IDC_SHORTCUTS_TEXT, lpStartOfText);
			
	GlobalUnlock(hResource);

	return TRUE;
}
			

//=============== ShortCutsDlg_OnCommand ===================================
// Purpose: Process command messages which, for the ShortCutsDlg box, is
//			just "OK"
//==========================================================================
void ShortCutsDlg_OnCommand( HWND hDlg, int iCmd, 
														HWND hwndCntl, UINT uCodeNotify)
{
	switch(iCmd) 
		{
		case OK:
			EndDialog(hDlg, TRUE);
			return;
		}
}			



/////////// NotesAndTipsDlg Dialog procedures //////////////////////////////


//=============== MakeNotesAndTipsDlg ======================================
//	Purpose: Make the NotesAndTipsDlg dialog box, activate it, then release 
//				the ProcInst when done
//==========================================================================
void MakeNotesAndTipsDlg(HWND hWnd)
{
	DLGPROC lpfnNotesAndTipsDlg;
	HINSTANCE hInst=GetWindowInstance(hWnd);
				
	lpfnNotesAndTipsDlg=(DLGPROC)MakeProcInstance(
										 (FARPROC)NotesAndTipsDlg_OldDlgProc, hInst);
				                                                     
	//Show the dialog box	                                                     
	DialogBoxParam( hInst, "HELP_NOTESANDTIPSDLG", hWnd, lpfnNotesAndTipsDlg, 0L);
				
	FreeProcInstance( (FARPROC)lpfnNotesAndTipsDlg);
	return;
}                                                                      
			

//=============== NotesAndTipsDlg_OldDlgProc ===============================
//	Purpose: This is the "Old Style" dialog interface, with two macros 
//	         from windowsx.h to check the recursion flag, and send back 
//	         LRESULTS if necessary
//==========================================================================
BOOL FAR PASCAL NotesAndTipsDlg_OldDlgProc(HWND hDlg, UINT uMessage,
												 WPARAM wParam, LPARAM lParam)
{
	CheckDefDlgRecursion( &gbNotesAndTipsDlgRecursing );	
				
	return( SetDlgMsgResult( hDlg, uMessage, 
						NotesAndTipsDlg_DlgProc(hDlg, uMessage, wParam, lParam) ) );
}
			                                                                     

//=============== NotesAndTipsDlg_DlgProc ==================================
//	Purpose: This is the "New Style" dialog proc, looking a lot like a WndProc
//			it handles the two messages AboutDlg might get: Init and Command
//==========================================================================
LRESULT NotesAndTipsDlg_DlgProc( HWND hDlg, UINT uMessage, 
															WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
		HANDLE_MSG(hDlg, WM_INITDIALOG, NotesAndTipsDlg_OnInitDialog);
					
		HANDLE_MSG(hDlg, WM_COMMAND, NotesAndTipsDlg_OnCommand);
					
		default:
			return( NotesAndTipsDlg_DefProc(hDlg, uMessage, wParam, lParam) );
	}
}
			

//=============== NotesAndTipsDlg_OnInitDialog =============================
//	Purpose: Initialize the dialog box by loading the name of the NotesAndTips
//				TEXT resource for this dialog, then loading the text itself, then
//				changing the last character of the text (\) to '/0' and then 
//				pouring the text into the dialog box's read only edit control.
//==========================================================================
BOOL NotesAndTipsDlg_OnInitDialog( HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	HINSTANCE	hInst=GetWindowInstance(hDlg);
	char szDlgTextRsrcName[100];  //For the name of the dialog's TEXT resource
	HRSRC hFoundResource;
	
	static HGLOBAL hResource;
	char far	*lpText;
	char far	*lpStartOfText;

	// Load the name of the NotesAndTips TEXT resource...
   LoadString(hInst, IDS_NANDTTEXT_RESNAME, szDlgTextRsrcName, 
   															sizeof(szDlgTextRsrcName) );

	// Now load the text itself
	hFoundResource=FindResource(hInst, szDlgTextRsrcName, "TEXT");
	hResource=LoadResource( hInst, hFoundResource);
	lpStartOfText=lpText = (char far *)LockResource(hResource);
			
	while(*lpText != '\\' && *lpText != '\0') //Find the "\" char
		lpText = AnsiNext(lpText);             // indicating end of text
	*lpText='\0';

	SetDlgItemText(hDlg, IDC_NANDT_TEXT, lpStartOfText);
			
	GlobalUnlock(hResource);

	return TRUE;
}
			

//=============== NotesAndTipsDlg_OnCommand ================================
//	Purpose: Process command messages which, for the NotesAndTipsDlg box, is
//			just "OK"
//==========================================================================
void NotesAndTipsDlg_OnCommand( HWND hDlg, int iCmd, 
														HWND hwndCntl, UINT uCodeNotify)
{
	switch(iCmd) 
		{
		case OK:
			EndDialog(hDlg, TRUE);
			return;
		}
}			



/*

/////////// *DLGNAME* Dialog procedures ////////////////////////////////////

//=============== Make*DLGNAME* ============================================
//	Purpose: Make the *DLGNAME* dialog box, activate it, then release the 
//				ProcInst when done
//==========================================================================
void Make*DLGNAME*(HWND hWnd)
{
	DLGPROC lpfn*DLGNAME*;
	HINSTANCE hInst=GetWindowInstance(hWnd);
				
	lpfn*DLGNAME*=(DLGROC)MakeProcInstance( 
										(FARPROC)*DLGNAME*_OldDlgProc, hInst);
				                                                     
	//Show the dialog box	                                                     
	DialogBoxParam( hInst, "*DLGNAME*", hWnd, lpfn*DLGNAME*, 0L);
				
	FreeProcInstance( (FARPROC)lpfn*DLGNAME*);
	return;
}                                                                      
			

//=============== *DLGNAME*_OldDlgProc =====================================
//	Purpose: This is the "Old Style" dialog interface, with two macros 
//	         from windowsx.h to check the recursion flag, and send back 
//	         LRESULTS if necessary
//==========================================================================
BOOL CALLBACK *DLGNAME*_OldDlgProc(HWND hDlg, UINT uMessage,
												 WPARAM wParam, LPARAM lParam)
{
	CheckDefDlgRecursion( &gb*DLGNAME*Recursing );	
				
	SetDlgMsgResult( hDlg, uMessage, 
						*DLGNAME*_DlgProc(hDlg, uMessage, wParam, lParam) );
}
			                                                                     

//=============== *DLGNAME*_DlgProc ========================================
//	Purpose: This is the "New Style" dialog proc, looking a lot like a WndProc
//				it handles the two messages *DLGNAME* might get: Init and Command
//==========================================================================
LRESULT *DLGNAME*_DlgProc( HWND hDlg, UINT uMessage, 
														WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
		HANDLE_MSG(hDlg, WM_INITDIALOG, *DLGNAME*_OnInitDialog);
					
		HANDLE_MSG(hDlg, WM_COMMAND, *DLGNAME*_OnCommand);
					
		default:
			return( *DLGNAME*_DefProc(hDlg, uMessage, wParam, lParam) );
	}
}
			

//=============== *DLGNAME*_OnInitDialog ===================================
//	Purpose: Initialize the dialog box by 
//	Strategy:
//			
//==========================================================================
BOOL *DLGNAME*_OnInitDialog( HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
			
	return TRUE;
}
			

//=============== *DLGNAME*_OnCommand ======================================
//	Purpose: Process command messages which, for the *DLGNAME* box, is
//			just "OK"
//==========================================================================
void *DLGNAME*_OnCommand( HWND hDlg, int iCmd, 
														HWND hwndCntl, UINT uCodeNotify)
{
	switch(iCmd) 
		{
		case OK:
			EndDialog(hDlg, TRUE);
			return;
	// case OTHER CASES...		
		}
}			

*/
