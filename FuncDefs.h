/*----------------------------------------------------------------------------
	6x6fndef.h - Function definitions 
						
                   Doug Rogers, 09/04/95
						
---------------------------------------------------------------------------*/
						
#ifndef __6x6fndef_H     /* prevent multiple includes */
#define __6x6fndef_H


typedef struct tagCardsToDraw
{
	BOOL	bDeck;
	BOOL	bScore;
	int	iHomePile;
	int	bZone[NUMBEROFZONES][2];
}typeCardsToDraw;

						
/*++++++ Function Prototypes +++++++++++++++++++++++++++++++++++++++++*/
LRESULT FAR PASCAL WndProc (HWND, UINT, WPARAM, LPARAM) ;

BOOL MakePaintRegion(int iMoveType);
int ShowCardsRemaining(HDC hdc); 
void DrawBitmap(HDC hdc, HBITMAP hBitmap, short xStart, short yStart);
void ShowClickCount(HWND hWnd);
void ShowElapsedTime(HWND hWnd);

//void		SetUpOpenFNStr(HWND hWnd);
//void		SetUpSaveAsFNStr(HWND hWnd);
//BOOL		ReadFile (HANDLE, HWND, HWND, POFSTRUCT, char *);
//BOOL		WriteFile (HANDLE, HWND, HWND, POFSTRUCT, char *, BOOL);
//LPSTR		lstrrchr(LPSTR, char);
//void		DoCaption (HWND hWnd, char * szFileName);
//short		AskAboutSave (HWND hWnd, char *szFileName);

//void MakeAboutDlg(HWND hWnd);
BOOL CALLBACK AboutDlg_OldDlgProc(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam);
LRESULT	AboutDlg_DlgProc( HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam);
BOOL	AboutDlg_OnInitDialog( HWND hDlg, HWND hwndFocus, LPARAM lParam);
void	AboutDlg_OnCommand( HWND hDlg, int iCmd, HWND hwndCntl, UINT uCodeNotify);

//void	MakeShortCutsDlg(HWND hWnd);
BOOL CALLBACK ShortCutsDlg_OldDlgProc(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam);
LRESULT	ShortCutsDlg_DlgProc( HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam);
BOOL	ShortCutsDlg_OnInitDialog( HWND hDlg, HWND hwndFocus, LPARAM lParam);
void	ShortCutsDlg_OnCommand( HWND hDlg, int iCmd, HWND hwndCntl, UINT uCodeNotify);

//void	MakeNotesAndTipsDlg(HWND hWnd);
BOOL CALLBACK NotesAndTipsDlg_OldDlgProc(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam);
LRESULT	NotesAndTipsDlg_DlgProc( HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam);
BOOL	NotesAndTipsDlg_OnInitDialog( HWND hDlg, HWND hwndFocus, LPARAM lParam);
void	NotesAndTipsDlg_OnCommand( HWND hDlg, int iCmd, HWND hwndCntl, UINT uCodeNotify);

/*
void	Make*DLGNAME*(HWND hWnd);
BOOL CALLBACK *DLGNAME*_OldDlgProc(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam);
LRESULT	*DLGNAME*_DlgProc( HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam);
BOOL	*DLGNAME*_OnInitDialog( HWND hDlg, HWND hwndFocus, LPARAM lParam);
void	*DLGNAME*_OnCommand( HWND hDlg, int iCmd, HWND hwndCntl, UINT uCodeNotify);

BOOL CALLBACK About_OldDlgProc(HWND hDlg, UINT uMessage,
												 WPARAM wParam, LPARAM lParam)
LRESULT	About_DlgProc( HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
*/
/*			
void		Make*DLGNAME*(HWND hWnd)
BOOL CALLBACK *DLGNAME*_OldDlgProc(HWND hDlg, UINT uMessage,
												 WPARAM wParam, LPARAM lParam)
LRESULT	*DLGNAME*_DlgProc( HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
*/			
			
/************* Message Handling ***************************************/
//BOOL APPNAME_OnCreate(HWND hWnd, CREATESTRUCT FAR* lpCreateStruct);
//void APPNAME_OnCommand(HWND hwnd, int iCmd, HWND hwndCtl, UINT codeNotify); 
//void APPNAME_OnPaint(HWND hWnd); 
//BOOL APPNAME_OnQueryEndSession(HWND hWnd); 
//void APPNAME_OnSize(HWND hWnd, UINT state, int cx, int cy); 
//void APPNAME_OnSetFocus(HWND hWnd, HWND hwndOldFocus);
//void APPNAME_OnTimer(HWND hWnd, UINT id); 
//void APPNAME_OnInitMenuPopup(HWND hWnd, HMENU hMenu,
//void APPNAME_OnClose(HWND hWnd); 
//void APPNAME_OnDestroy(HWND hWnd); 
/*
BOOL		About_OnInitDialog( HWND hDlg, HWND hwndFocus, LPARAM lParam)
void		About_OnCommand( HWND hDlg, int iCmd, HWND hwndCntl, UINT uCodeNotify)

BOOL *DLGNAME*_OnInitDialog( HWND hDlg, HWND hwndFocus, LPARAM lParam)
void *DLGNAME*_OnCommand(HWND hDlg, int iCmd, HWND hwndCntl, UINT uCodeNotify)
																	 int item, BOOL fSystemMenu);
*/			
#endif  /* __6x6fndef_H */


