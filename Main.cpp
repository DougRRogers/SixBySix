/*------------------------------------------------------

	6x6main.cpp - A solitaire card game; the main module.

                   Doug Rogers, 08/19/95

Changes:
--------
		08/19/95 15:27 - First writing
		08/23/95 15:12 - Mods to handle bent stacks         
					   - resized window to fit layout
		08/24/95 13:43 - Mods to handle resources             
					   - Ctrl N, Ctrl D, hand cursor, icon, dbl click
					   - design suits and faces
		08/25/95 12:15 - Add congratulations, are you sure?, click count
		09/10/95 11:30 - Use invalidate regions instead of redrawing all
2.11 09/11/95 12:35 - Speed up drawing by drawing only necessary bits
2.20 09/15/95 14:07 - Add time, menu options, fix up drawing

Functions:
----------
int	AskConfirmation(HWND hWnd, char * szTopic)
int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
						LPSTR lpszCmdParam, int nCmdShow)
long FAR PASCAL WndProc (HWND, UINT, WPARAM, LPARAM) ;       

---------------------------------------------------------------*/
#define VERSION 2.20

//#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>        //for sprintf functions
#include <stdlib.h>    //for rand() and srand()

#include "6x6defs.h"
#include "6x6cldef.h"
#include "resource.h"
#include "6x6fndef.h"


extern "C"
{
void MakeAboutDlg(HWND hWnd);
void	MakeShortCutsDlg(HWND hWnd);
void	MakeNotesAndTipsDlg(HWND hWnd);
} 


/******** Global Variables *****************************************/
FARPROC	glpfnAboutDlgProc;
FARPROC	glpfnSetLinesDlgProc;
FARPROC	glpfnShortCutsDlgProc;
FARPROC  glpfnNotesAndTipsDlgProc;
FARPROC	glpfnRulesOfGameDlgProc;
float		gfVersion=VERSION;
char 		gszAppName[] = "Six By Six" ;
//char		gszAppName[]="SixBySix;
char		gszFullAppName[]="Six By Six";
char		gszDEBUGBuff[160];     //Buffer used for debugging messages
char		gszMessageBuff[160];   //Buffer used for message boxes
int		giCardHeight=CARDHEIGHT;
int		giCardWidth=CARDWIDTH;
int		giHomePileSpace=HOMEPILESPACE;
int		giColSpacing=COLSPACE;
int		giTableMargin=TABLEMARGIN;
float		gfVFracShowing=VERTFRACSHOWING;
float		gfHFracShowing=HORFRACSHOWING;
TDeck		gDeck;
TTable	gTable;
char		gcValues[]="   A 2 3 4 5 6 7 8 910 J Q K";
char		gcSuits[]=" HSDC";
int 		giColTop=giTableMargin + giCardHeight + giHomePileSpace;
								//Y coord of top of columns
BOOL		gbStackSaved=0;		//Flag indicating a card stack has been
							//"erased" and is saved, and =this value
int		giCardStack[52];	//Stack to store cards being moved
int		giClientHeight, giClientWidth;   //Size of the client window
int		giWindowHeight, giWindowWidth;   //Size of the client window
int		giWidenWindow=0;     //Flag to indicate window should be widened
HBITMAP	ghSuitBitMap[4];  //Handles to the bitmaps for the suits
							 //[0-3]=HSDC, [0-1]=small, big
HBITMAP	ghCardBitMap[16];  //Handles to the bitmaps for the card faces
							 //[0-4]=Aces, [5-15]=other cards
BOOL		gbDrawBMaps=TRUE;    //Flag for drawing suits or writing symbols
int		giCardsInPiles;  //Count to see if game has been won								 
int		giClickCount;   //How many total clicks made so far
HRGN		ghrgnTmp[6];
HRGN		gClipRgn;
BOOL		gbShowScore=TRUE;
BOOL		gbShowTime=FALSE;
long		giInitialTickCount;

/**********************************************************************/


BOOL FAR PASCAL RulesOfGameDlgProc(HWND hDlg, UINT uMessage,
											 WORD wParam, LPARAM lParam)
{
	HINSTANCE	hInst=GetWindowInstance(hDlg);
	char szNAndTText[100];  //To store the name of T&N TEXT resource
	HRSRC hFoundResource;
	
	static HGLOBAL hResource;
	char far	*lpText;
	char far	*lpStartOfText;
			
	switch (uMessage)
		{
		case WM_INITDIALOG:
			// Load the name of the NotesAndTips TEXT resource...
         LoadString(hInst, 
               IDS_RULESOFGAME_RESNAME, szNAndTText, sizeof(szNAndTText) );
			// Now load the text itself
			hFoundResource=FindResource(hInst, szNAndTText, "TEXT");
			hResource=LoadResource( hInst, hFoundResource);
			lpStartOfText=lpText = LockResource(hResource);
			
			
			while(*lpText != '\\' && *lpText != '\0') //Find the "\" char
				lpText = AnsiNext(lpText);             // indicating end of text
			*lpText='\0';

			SetDlgItemText(hDlg, IDC_RULESOFGAME_TEXT, lpStartOfText);
			
			GlobalUnlock(hResource);    
			
			//put the cursor at the top of the text, with no selection
			SendDlgItemMessage(hDlg, IDC_RULESOFGAME_TEXT, EM_SETSEL, 0, MAKELPARAM(0,10));

			return TRUE;

		case WM_COMMAND:
			switch(wParam)
				{
				case IDOK:
					EndDialog(hDlg, 0);
					return TRUE;
				}
			break;
		}
	return FALSE;
}


BOOL FAR PASCAL NotesAndTipsDlgProc(HWND hDlg, UINT uMessage,
											 WORD wParam, LPARAM lParam)
{
	HINSTANCE	hInst=GetWindowInstance(hDlg);
	char szNAndTText[100];  //To store the name of T&N TEXT resource
	HRSRC hFoundResource;
	
	static HGLOBAL hResource;
	char far	*lpText;
	char far	*lpStartOfText;
			
	switch (uMessage)
		{
		case WM_INITDIALOG:
			// Load the name of the NotesAndTips TEXT resource...
         LoadString(hInst, 
               IDS_NANDTTEXT_RESNAME, szNAndTText, sizeof(szNAndTText) );
			// Now load the text itself
			hFoundResource=FindResource(hInst, szNAndTText, "TEXT");
			hResource=LoadResource( hInst, hFoundResource);
			lpStartOfText=lpText = LockResource(hResource);
			
			
			while(*lpText != '\\' && *lpText != '\0') //Find the "\" char
				lpText = AnsiNext(lpText);             // indicating end of text
			*lpText='\0';

			SetDlgItemText(hDlg, IDC_NANDT_TEXT, lpStartOfText);
			
			GlobalUnlock(hResource);    
			
			//put the cursor at the top of the text, with no selection
			SendDlgItemMessage(hDlg, IDC_NANDT_TEXT, EM_SETSEL, 0, MAKELPARAM(0,10));

			return TRUE;

		case WM_COMMAND:
			switch(wParam)
				{
				case IDOK:
					EndDialog(hDlg, 0);
					return TRUE;
				}
			break;
		}
	return FALSE;
}


BOOL FAR PASCAL ShortCutsDlgProc(HWND hDlg, UINT uMessage,                    
											 WORD wParam, LPARAM lParam)
{
	HINSTANCE	hInst=GetWindowInstance(hDlg);
	char szTextRsrcName[100];  //To store the name of TEXT resource
	HRSRC hFoundResource;
	
	static HGLOBAL hResource;
	char far	*lpText;
	char far	*lpStartOfText;
			
	switch (uMessage)
		{
		case WM_INITDIALOG:
			// Load the name of the NotesAndTips TEXT resource...
         LoadString(hInst, 
               IDS_SHORTCUTS_RESNAME, szTextRsrcName, sizeof(szTextRsrcName) );
			// Now load the text itself
			hFoundResource=FindResource(hInst, szTextRsrcName, "TEXT");
			hResource=LoadResource( hInst, hFoundResource);
			lpStartOfText=lpText = LockResource(hResource);
			
			while(*lpText != '\\' && *lpText != '\0') //Find the "\" char
				lpText = AnsiNext(lpText);             // indicating end of text				
			*lpText='\0';

			SetDlgItemText(hDlg, IDC_SHORTCUTS_TEXT, lpStartOfText);
			
			GlobalUnlock(hResource);

			//put the cursor at the top of the text, with no selection
			SendDlgItemMessage(hDlg, IDC_SHORTCUTS_TEXT, EM_SETSEL, 0, MAKELPARAM(0,0));

			return TRUE;

		case WM_COMMAND:
			switch(wParam)
				{
				case IDOK:
					EndDialog(hDlg, 0);
					return TRUE;
				}
			break;
		}
	return FALSE;
}


BOOL FAR PASCAL AboutDlgProc(HWND hDlg, UINT uMessage,
											 WORD wParam, LPARAM lParam)
{
	HINSTANCE	hInst=GetWindowInstance(hDlg);
	char szRsrcTextName[100];  //To store the name of TEXT resource
	HRSRC hFoundResource;
	
	static HGLOBAL hResource;
	char far	*lpText;
	char far	*lpStartOfText;
			

	char szBuff[20];
	
	sprintf(szBuff, "Version %4.2f", gfVersion);
	
	switch (uMessage)
		{
		case WM_INITDIALOG:                          
			SetDlgItemText(hDlg, IDC_ABOUT_APPNAME, gszAppName);
			SetDlgItemText(hDlg, IDC_ABOUT_ONELINER, "Created in the beautiful Sierras!");
			SetDlgItemText(hDlg, IDC_ABOUT_NAMEANDDATE, "Doug Rogers, Aug. 1995");
			SetDlgItemText(hDlg, IDD_VERSION_NUM, szBuff);

			// Load the name of the AboutAddress TEXT resource...
         LoadString(hInst, 
               IDS_ABOUTADDRES_RESNAME, szRsrcTextName, sizeof(szRsrcTextName) );
			// Now load the text itself
			hFoundResource=FindResource(hInst, szRsrcTextName, "TEXT");
			hResource=LoadResource( hInst, hFoundResource);
			lpStartOfText=lpText = LockResource(hResource);
			
			
			while(*lpText != '\\' && *lpText != '\0') //Find the "\" char
				lpText = AnsiNext(lpText);             // indicating end of text
			*lpText='\0';

			SetDlgItemText(hDlg,  IDC_ABOUT_ADDRESS, lpStartOfText);
			
			GlobalUnlock(hResource);    
			
			//put the cursor at the top of the text, with no selection
			SendDlgItemMessage(hDlg, IDC_ABOUT_ADDRESS, EM_SETSEL, 0, MAKELPARAM(0,10));

			return TRUE;


		case WM_COMMAND:
			switch(wParam)
				{
				case IDOK:
					EndDialog(hDlg, 0);
					return TRUE;
				}
			break;
		}

	return FALSE;
}



int	AskConfirmation(HWND hWnd, char * szTopic)
	{
	char	szBuff[80];
	
	sprintf(szBuff,"Do you really want to %s?", szTopic);		
	return MessageBox(hWnd, szBuff,
						gszAppName, MB_YESNO | MB_ICONQUESTION);
	}

int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
						LPSTR lpszCmdParam, int nCmdShow)
   {
   HWND		hWnd;
   MSG		msg;
   HANDLE	hAccel;
   WNDCLASS	wndclass;                                      
   HMENU	hMenu, hSymbolsPopup;    
   HBITMAP	hSymbolsBitmap;
   int		iCol;
   
	for(int iBMNum=0; iBMNum<4; iBMNum++)
		{
		ghSuitBitMap[iBMNum]=LoadBitmap(hInstance, 
						MAKEINTRESOURCE(BITMAP_RSRC_OFFSET+iBMNum) );
		}				
				
	for(iBMNum=0; iBMNum<16; iBMNum++)
		ghCardBitMap[iBMNum]=LoadBitmap(hInstance, 
						MAKEINTRESOURCE(BITMAP_RSRC_OFFSET+iBMNum+4) );

	hMenu=LoadMenu(hInstance, MAKEINTRESOURCE (IDR_MENU1));	//The menu in the
	                                                        // resource file
	
	hSymbolsBitmap=LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_SUIT_SYMBOLS) );

	hSymbolsPopup=CreateMenu();
	AppendMenu(hSymbolsPopup, MF_STRING, ID_USE_LETTERS, " H S D C ");
	AppendMenu(hSymbolsPopup, MF_BITMAP, ID_USE_SYMBOLS,
													 (LPSTR)(LONG) hSymbolsBitmap);
													 
	ModifyMenu(hMenu, ID_DECK_CARDSYMBOLS, MF_BYCOMMAND | MF_POPUP, (UINT)hSymbolsPopup, "Card &Symbols");
	
   gbDrawBMaps=TRUE;
   CheckMenuItem(hMenu, ID_USE_SYMBOLS, MF_CHECKED);						 
	
	for(iCol=1; iCol<6; iCol++)                  
		ghrgnTmp[iCol]=CreateRectRgn(0,0,0,0);
		
	gClipRgn=CreateRectRgn(0,0,0,0);

	if (!hPrevInstance)
      {
      wndclass.style		= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
      wndclass.lpfnWndProc	= WndProc;
      wndclass.cbClsExtra	= 0;
      wndclass.cbWndExtra	= 0;
      wndclass.hInstance	= hInstance;
      wndclass.hIcon		= LoadIcon (hInstance, MAKEINTRESOURCE (IDI_ICON1));
      wndclass.hCursor		= LoadCursor (hInstance, MAKEINTRESOURCE (IDC_CURSOR1));
      wndclass.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
      wndclass.lpszMenuName	= NULL;
//      wndclass.lpszMenuName	= MAKEINTRESOURCE (IDR_MENU1);
      wndclass.lpszClassName	= gszAppName;

      RegisterClass (&wndclass);
      }

   hWnd = CreateWindow (gszAppName,	// window class name
	"Six By Six",		//window caption
	WS_OVERLAPPEDWINDOW,		//window style
	INIT_X_POS,			//initial x position
	INIT_Y_POS,			//initial y position
	INIT_X_SIZE,			//initial x size
	INIT_Y_SIZE,			//initial y size
	NULL,				//parent window handle
	hMenu,				//window menu handle
	hInstance,			//program instance handle
	NULL);				//creation parameters

   ShowWindow (hWnd, nCmdShow);
   UpdateWindow (hWnd);

   hAccel=LoadAccelerators(hInstance, MAKEINTRESOURCE (IDR_ACCELERATOR1));

   while (GetMessage (&msg, NULL, 0, 0))
		{
		if(!TranslateAccelerator(hWnd, (HACCEL)hAccel, &msg))
        	{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
			}
        }

	for(iBMNum=0; iBMNum<4; iBMNum++)
		DeleteObject(ghSuitBitMap[iBMNum]);
	for(iBMNum=0; iBMNum<16; iBMNum++)
		DeleteObject(ghCardBitMap[iBMNum]);
	DeleteObject(hSymbolsBitmap);
	   
	for(iCol=1; iCol<6; iCol++)                  
		DeleteRgn(ghrgnTmp[iCol]);
	DeleteRgn(gClipRgn);

   return msg.wParam;
   }

long FAR PASCAL WndProc (HWND hWnd, UINT message,
							WPARAM wParam, LPARAM lParam)
	{
	char		szBuffer[10];
//	HDC		hdc;
	PAINTSTRUCT	ps;
	RECT		rect;           
	HMENU		hMenu;
	int		iHClickPos, iVClickPos;  //Position of mouse click
	int		iClickedZone;			//Which zone was clicked  
	int		iCardCol, iCard;		//The column, and which card in the col
	HINSTANCE hInst=GetWindowInstance(hWnd);

	switch (message)
		{
		case WM_CREATE:
			gDeck.Shuffle();
			gDeck.Deal(gTable, hWnd);     

			GetClientRect (hWnd, &rect);
			giClientHeight=rect.bottom-rect.top;
			giClientWidth=rect.right-rect.left;
			gTable.SetHitZones(REDO_ALL);			
			giCardsInPiles=0;
			giClickCount=0;             
			SetTimer(hWnd, 1, 1000, NULL);
			giInitialTickCount=GetTickCount();			
			
			return 0;			

		case WM_SIZE:
			GetClientRect (hWnd, &rect);
			giClientHeight=rect.bottom-rect.top;
			giClientWidth=rect.right-rect.left;
			GetWindowRect (hWnd, &rect);
			giWindowHeight=rect.bottom-rect.top;
			giWindowWidth=rect.right-rect.left;
			gTable.SetHitZones(REDO_ALL);			
			return 0;
			

		case WM_TIMER:
			if(gbShowTime);
				ShowElapsedTime(hWnd);
			return 0;
			

		case WM_PAINT:
			BeginPaint (hWnd, &ps);
			gTable.Draw(hWnd);

			EndPaint (hWnd, &ps);
			return 0;

		case WM_ACTIVATE:
		   if(wParam==WA_ACTIVE || wParam==WA_CLICKACTIVE)
				MakePaintRegion(REDO_ALL);
			InvalidateRect(hWnd, NULL, TRUE);
			return DefWindowProc (hWnd, message, wParam, lParam);

		case	WM_LBUTTONDOWN:
			iHClickPos = LOWORD (lParam);
			iVClickPos = HIWORD (lParam);

			iClickedZone=gTable.GetZone(iHClickPos, iVClickPos);	
			giClickCount++;  
			switch(iClickedZone)
				{
				case 0:		//on Deck
					if(gDeck.GetCardsRemaining() != 0)
						{
						gTable.AddCard(hWnd, gDeck.NextCard(hWnd), 0);
						gTable.SetHitZones(CARD_DEALT_MOVE);			
					    if(giWidenWindow)
					    	{
					    	SetWindowPos
					    	  (hWnd, HWND_TOP, 0, 0, 
					    	  	giWidenWindow+giTableMargin, giWindowHeight, SWP_NOMOVE);
					    	giWidenWindow=0;
					    	}
						InvalidateRect(hWnd, NULL, FALSE);
						UpdateWindow(hWnd);
						}
					break;
				case 2:		//Home pile 1
				case 3:		//Home pile 2
				case 4:		//Home pile 3
				case 5:		//Home pile 4
					iCardCol=iClickedZone;
					if(gbStackSaved)
						//See if a stack is saved...
						{   // and if so, try to add it to the home pile
					    gTable.AddToPile(hWnd, iCardCol, PILEKNOWN);
					    gbStackSaved=FALSE;
						InvalidateRect(hWnd, NULL, FALSE);
						UpdateWindow(hWnd);
						if(giCardsInPiles==52) //Game has been won!
							MessageBox(hWnd, "CONGRATULATIONS !",
								gszAppName, MB_OK | MB_ICONEXCLAMATION); 
					    }
					break;
				case 6:		//Col 1
				case 7:		//Col 2
				case 8:		//Col 3
				case 9:		//Col 4
				case 10:	//Col 5
				case 11:	//Col 6
					iCardCol=iClickedZone-COLZONEOFFSET;
					iCard=gTable.GetCardInCol(iHClickPos, iVClickPos, 
															iClickedZone);
	                  
					if(gbStackSaved) //See if a card is saved...
						{          // and if so, add it to the column
					    gTable.AddStack(hWnd, iCardCol);
					    gbStackSaved=FALSE;
					    if(giWidenWindow)
					    	{
					    	SetWindowPos
					    	  (hWnd, HWND_TOP, 0, 0, 
					    	  	giWidenWindow+giTableMargin, giWindowHeight, SWP_NOMOVE);
					    	giWidenWindow=0;
					    	}
						InvalidateRect(hWnd, NULL, FALSE);
						UpdateWindow(hWnd);
					    }
					  else  //Otherwise, if there is a card here, remove it
					  	if(gTable.GetCardsInCol(iCardCol) != 0 )
						  	{
							gTable.RemoveStack(hWnd, iCard, iCardCol);
							gbStackSaved=TRUE;
							InvalidateRect(hWnd, NULL, FALSE);
							UpdateWindow(hWnd);
           				}
					break;
				case 1:
				case NOTINAZONE:	//Click was outside of all zones
					break;
				}       
			return 0;			

		case WM_LBUTTONDBLCLK:
			if(gbStackSaved)
				//See if a stack is saved...
				{   // and if so, try to add it to the home pile
//				hdc = BeginPaint( hWnd, &ps );
			    gTable.AddToPile(hWnd, iCardCol, PILENOTKNOWN);
			    gbStackSaved=FALSE;
//				InvalidateRgn(hWnd, ghrgnRepaintRgn, TRUE);
				InvalidateRect(hWnd, NULL, FALSE);
				UpdateWindow(hWnd);
				if(giCardsInPiles==52) //Game has been won!
					MessageBox(hWnd, "CONGRATULATIONS !",
						gszAppName, MB_OK | MB_ICONEXCLAMATION); 
//	            EndPaint(hWnd, &ps);
			    }

			return 0;
						
		case WM_COMMAND:
			hMenu=GetMenu(hWnd);

			switch (wParam)
				{
/*
				case IDM_OPEN:
				case IDM_SAVE:
	  			case IDM_SAVEAS:               	
				case IDM_UNDO:
*/
				case ID_GAME_NEW:
					if ( (gDeck.GetCardsRemaining() != 0)  &&
						  (IDNO == AskConfirmation(hWnd, "start a new game") ) )
						return 0;

//					hdc = BeginPaint (hWnd, &ps);
			
					gDeck.Shuffle();
					gDeck.Deal(gTable, hWnd);     
			
//					EndPaint (hWnd, &ps);                             
			
					GetClientRect (hWnd, &rect);
					giClientHeight=rect.bottom-rect.top;
					giClientWidth=rect.right-rect.left;
					gTable.SetHitZones(REDO_ALL);			
					giCardsInPiles=0;
					giClickCount=0;
					giInitialTickCount=GetTickCount();			
					InvalidateRect(hWnd, NULL, TRUE);
					UpdateWindow(hWnd);

					return 0;			
					
				case ID_DECK_DEALCARD:
					if(gDeck.GetCardsRemaining() != 0)
						{
						giClickCount++;  
						gTable.AddCard(hWnd, gDeck.NextCard(hWnd), 0);
						gTable.SetHitZones(CARD_DEALT_MOVE);			
					    if(giWidenWindow)
					    	{
					    	SetWindowPos
					    	  (hWnd, HWND_TOP, 0, 0, 
					    	  	giWidenWindow+giTableMargin, giWindowHeight, SWP_NOMOVE);
					    	giWidenWindow=0;
					    	}
//						InvalidateRgn(hWnd, ghrgnRepaintRgn, TRUE);
						InvalidateRect(hWnd, NULL, FALSE);
						UpdateWindow(hWnd);
						}
					return 0;

				case ID_USE_LETTERS:
	                gbDrawBMaps=FALSE;
						 CheckMenuItem(hMenu, ID_USE_SYMBOLS, MF_UNCHECKED);
						 CheckMenuItem(hMenu, ID_USE_LETTERS, MF_CHECKED);
						InvalidateRect(hWnd, NULL, TRUE);
						UpdateWindow(hWnd);
	                return 0;
				
				case ID_USE_SYMBOLS:
	                gbDrawBMaps=TRUE;
						 CheckMenuItem(hMenu, ID_USE_LETTERS, MF_UNCHECKED);
						 CheckMenuItem(hMenu, ID_USE_SYMBOLS, MF_CHECKED);						 
						InvalidateRect(hWnd, NULL, TRUE);
						UpdateWindow(hWnd);
	                return 0;
				
				case IDM_OPTIONS_SHOWCLICKS:
					CheckMenuItem (hMenu, IDM_OPTIONS_SHOWCLICKS, gbShowScore ? MF_UNCHECKED : MF_CHECKED);
					gbShowScore=!gbShowScore;
					ShowClickCount(hWnd);
					return 0;	

				case IDM_OPTIONS_SHOWTIME:
					CheckMenuItem (hMenu, IDM_OPTIONS_SHOWTIME, gbShowTime ? MF_UNCHECKED : MF_CHECKED);
					gbShowTime=!gbShowTime;
					ShowElapsedTime(hWnd);
					return 0;				

				case ID_GAME_EXIT:
					SendMessage(hWnd, WM_CLOSE, 0, 0L);
					return 0;

		case IDM_HELP_ABOUT:
			glpfnAboutDlgProc=MakeProcInstance((FARPROC)AboutDlgProc, hInst);
			DialogBox(hInst, "HELP_ABOUTDLG", hWnd, (DLGPROC) glpfnAboutDlgProc);
			FreeProcInstance(glpfnAboutDlgProc);
        	return 0;          					

		case IDM_HELP_SHORTCUTKEYS:
			glpfnShortCutsDlgProc=MakeProcInstance((FARPROC)ShortCutsDlgProc, hInst);
			DialogBox(hInst, "HELP_SHORTCUTSDLG", hWnd, (DLGPROC) glpfnShortCutsDlgProc);
			FreeProcInstance(glpfnShortCutsDlgProc);
        	return 0;
		
		case IDM_HELP_NOTESANDTIPS:
			glpfnNotesAndTipsDlgProc=MakeProcInstance((FARPROC)NotesAndTipsDlgProc, hInst);
			DialogBox(hInst, "HELP_NOTESANDTIPSDLG", hWnd, (DLGPROC) glpfnNotesAndTipsDlgProc);
			FreeProcInstance(glpfnNotesAndTipsDlgProc);
        	return 0;

		case IDM_HELP_RULESOFGAME:
			glpfnRulesOfGameDlgProc=MakeProcInstance((FARPROC)RulesOfGameDlgProc, hInst);
			DialogBox(hInst, "HELP_RULESOFGAMEDLG", hWnd, (DLGPROC) glpfnRulesOfGameDlgProc);
			FreeProcInstance(glpfnRulesOfGameDlgProc);
        	return 0;

				}
		break;
		

		case WM_CLOSE:
			if( IDYES == AskConfirmation(hWnd, "quit") )
				DestroyWindow(hWnd);
			return 0;
			
		case WM_QUERYENDSESSION:
			if( IDYES == AskConfirmation(hWnd, "quit") )
				return 1L;
			  else
			  	return 0;
			
		case WM_DESTROY:
			KillTimer(hWnd, 1);
			PostQuitMessage (0);
			return 0;
		}

	return DefWindowProc (hWnd, message, wParam, lParam);
    }


