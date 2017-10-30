/*------------------------------------------------------
	6x6subf.cpp - Subfunctions for the Six by Six program
	
                   Doug Rogers, 08/19/95

Changes:
--------
08/20/95 23:27 - First writing
08/23/95 15:12 - Mods to handle bent stacks

Functions:
----------
int		GetZone(iHClickPos, iVClickPos)

--------------------------------------------------------*/
#include <windows.h>
#include <windowsx.h>

#include "6x6defs.h"
#include "6x6cldef.h"
#include "6x6fndef.h"

/******** Global Variables ***********/
extern  int		giCardHeight;
extern  int		giCardWidth;
extern  int		giHomePileSpace;
extern  int		giColSpacing;
extern  int		giTableMargin;
extern  float	gfVFracShowing, gfHFracShowing;
extern  TDeck	gDeck;
extern  TTable	gTable;
extern  char	gcValues[];
extern  char	gcSuits[];
extern 	int		giColTop;
extern	HBITMAP	ghSuitBitMap[4];
extern	BOOL	gbDrawBMaps;   
extern	int		giClickCount;   //How many total clicks made so far								
extern	HRGN	ghrgnDeckRgn, ghrgnScoreRgn, ghrgnHPRgn[4];
extern	HRGN	ghrgnRepaintRgn;                      
extern	BOOL	gbShowScore;
extern	BOOL	gbShowTime;
extern	typeCardsToDraw gCardsToDraw;
extern	BOOL	gbFlagZone[][2];
extern	RECT	grBlankBentZone[6]; //Bent zones to be erased
extern	long	giInitialTickCount;

/*====== MakePaintRegion(HDC hDC) ===========================================================
Purpose: Draw the valueof the card which is at the location
		 iCardCol, iCardRow in the card table.
Strategy:

============================================================================*/
BOOL MakePaintRegion(int iMoveType)
{
	static RECT OldHZ[6][2];
	int iCol, iPrevCol;            
	RECT NullRect;
					
//Possible moves:
// CARD_DEALT_MOVE	
// LAYOUT_MOVE        
// WINDOW_RESIZE_MOVE 
// HOME_PILE_MOVE		
// REDO_ALL				
// UPDATE_SCORES_ONLY

 	
 	if(iMoveType==REDO_ALL)
		{                        
		giClickCount*=(-1);  //This is a way to flag ReDo All in Draw()		
		SetRect(&NullRect, 0, 0, 0, 0);
		for(iCol=0; iCol<6; iCol++)
			for (int j=0; j<2; j++)
		   	OldHZ[iCol][j]=NullRect;
		}
					
	if(gbShowScore)
		gCardsToDraw.bScore=TRUE;
	 else
		gCardsToDraw.bScore=FALSE;
					
	if(iMoveType==CARD_DEALT_MOVE || iMoveType==REDO_ALL) 
		gCardsToDraw.bDeck=TRUE;
	 else
		gCardsToDraw.bDeck=FALSE;
					
	if(iMoveType>HOME_PILE_MOVE)   //iMoveType - H_P_MOVE = the target pile
		gCardsToDraw.iHomePile=iMoveType-HOME_PILE_MOVE;
	 else
		gCardsToDraw.iHomePile=0;

	for(iCol=5; iCol>=0; iCol--)
		for (int j=0; j<2; j++)
		{                                            
		if( !EqualRect(&gTable.Col[iCol].HZ[j], &OldHZ[iCol][j])
		        || gbFlagZone[iCol][j] )
			{
			if(j==1) //if a bent zone has changed it will have to be erased...
				{
				SetRect(&grBlankBentZone[iCol], 
				  gTable.Col[iCol].HZ[0].right, //just right edge of col.
				  OldHZ[iCol][1].top, 
				  OldHZ[iCol][1].right, 
				  OldHZ[iCol][1].bottom);
				for(iPrevCol=5; iPrevCol>iCol; iPrevCol--) //prev cols have to be checked 
					{													//for redraw needs
					if(gTable.Col[iPrevCol].HZ[0].bottom > OldHZ[iCol][1].top
						&& gTable.Col[iPrevCol].HZ[0].left < OldHZ[iCol][1].right)
						gCardsToDraw.bZone[iPrevCol][0]=TRUE;
					if(gTable.Col[iPrevCol].HZ[1].bottom > OldHZ[iCol][1].top
						&& gTable.Col[iPrevCol].HZ[1].left < OldHZ[iCol][1].right)
						gCardsToDraw.bZone[iPrevCol][1]=TRUE;
					}
				}
			OldHZ[iCol][j]=gTable.Col[iCol].HZ[j];
			gCardsToDraw.bZone[iCol][j]=TRUE;
			}
		 else
			gCardsToDraw.bZone[iCol][j]=FALSE;
		}
								
	return TRUE;
}



/*====== ShowCardsRemaining(HWND hWnd) ===========================================================
Purpose: Draw the valueof the card which is at the location
		 iCardCol, iCardRow in the card table.
Strategy:

============================================================================*/
int ShowCardsRemaining(HWND hWnd) 
{
	char	szBuff[10];         
	HDC hDC=GetDC(hWnd);
	
	TextOut(hDC, 
			giTableMargin + XVALUEOFFSET, 
			giTableMargin + YVALUEOFFSET + 5,
			szBuff, 
			wsprintf(szBuff, "   %2d  ",gDeck.GetCardsRemaining()) );	
	
	TextOut(hDC, 
			giTableMargin + XVALUEOFFSET, 
			giTableMargin + YVALUEOFFSET + VTEXTSPACING +5,
			szBuff,
			wsprintf(szBuff, " Card%c ", 
			    (gDeck.GetCardsRemaining()==1)?' ':'s') );	

//			(gDeck.GetCardsRemaining()==1) ?
//			   wsprintf(szBuff, " Card"):wsprintf(szBuff, "Cards") );	

	TextOut(hDC, 
			giTableMargin + XVALUEOFFSET, 
			giTableMargin + YVALUEOFFSET + 2*VTEXTSPACING +5,
			szBuff, 
			wsprintf(szBuff, "  Left") );	

   ReleaseDC(hWnd, hDC);
	return OK;
}

                      
                      
/*== DrawBitmap(HDC hDC, HBITMAP hBitmap, short xStart, short yStart) =======
Purpose: Draw the Bitmap referenced by hBitmap at xStart, yStart

Strategy:

============================================================================*/
void DrawBitmap(HDC hDC, HBITMAP hBitmap, short xStart, short yStart)
{
	BITMAP	bm;
	HDC		hdcMem;
	POINT	ptSize, ptOrg;

	hdcMem = CreateCompatibleDC(hDC);
	SelectObject(hdcMem, hBitmap);
	SetMapMode(hdcMem, GetMapMode (hDC));
	
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR) &bm);
	
	ptSize.x=bm.bmWidth;
	ptSize.y=bm.bmHeight;
	DPtoLP(hDC, &ptSize, 1);
	
	ptOrg.x=0;
	ptOrg.y=0;
	DPtoLP(hdcMem, &ptOrg, 1);
	
	BitBlt(hDC, xStart, yStart, ptSize.x, ptSize.y, 
			hdcMem, ptOrg.x, ptOrg.y, SRCCOPY);
			
	DeleteDC(hdcMem);
}	
	                      


/*====== ShowClickCount(HWND hWnd) ===========================================================
Purpose: Show how many valid clicks have been made at this point
Strategy:

============================================================================*/
void ShowClickCount(HWND hWnd) 
{
	char	szBuff[25];         
	HDC hDC=GetDC(hWnd);
	

	if(gbShowScore)
		{
		TextOut(hDC, 
				giTableMargin + giCardWidth + giColSpacing + XVALUEOFFSET, 
				giTableMargin + YVALUEOFFSET,
				szBuff, 
				wsprintf(szBuff, "  %d",giClickCount) );	
		
		TextOut(hDC, 
				giTableMargin + giCardWidth + giColSpacing + XVALUEOFFSET, 
				giTableMargin + YVALUEOFFSET + VTEXTSPACING,
				szBuff,
				wsprintf(szBuff, "Click%c ", giClickCount==1?' ':'s') );	
		}
	 else  // Erase what's showing
		{
		TextOut(hDC, 
				giTableMargin + giCardWidth + giColSpacing + XVALUEOFFSET, 
				giTableMargin + YVALUEOFFSET,
				szBuff, 
				wsprintf(szBuff, "           ",giClickCount) );	
		
		TextOut(hDC, 
				giTableMargin + giCardWidth + giColSpacing + XVALUEOFFSET, 
				giTableMargin + YVALUEOFFSET + VTEXTSPACING,
				szBuff,
				wsprintf(szBuff, "           ", giClickCount==1?' ':'s') );	
		}

   ReleaseDC(hWnd, hDC);
}


/*====== ShowElapsedTime(HWND hWnd) ===========================================================
Purpose: Show how many valid clicks have been made at this point
Strategy:

============================================================================*/
void ShowElapsedTime(HWND hWnd) 
{
	char	szBuff[25];         
	HDC hDC=GetDC(hWnd);
	int iSecElapsed;

	iSecElapsed=(GetTickCount()-giInitialTickCount)/1000L;

	if(gbShowTime)
		{	
		TextOut(hDC, 
				giTableMargin + giCardWidth + giColSpacing + XVALUEOFFSET, 
				giTableMargin + YVALUEOFFSET + VTEXTSPACING*2,
				szBuff, 
				wsprintf(szBuff, "  %d", iSecElapsed ) );	
		
		TextOut(hDC, 
				giTableMargin + giCardWidth + giColSpacing + XVALUEOFFSET-5, 
				giTableMargin + YVALUEOFFSET + VTEXTSPACING*3,
				szBuff,
				wsprintf(szBuff, "second%c", iSecElapsed==1?' ':'s') );	
		}
	 else  //Erase what's showing...
		{	
		TextOut(hDC, 
				giTableMargin + giCardWidth + giColSpacing + XVALUEOFFSET, 
				giTableMargin + YVALUEOFFSET + VTEXTSPACING*2,
				szBuff, 
				wsprintf(szBuff, "           " ) );	
		
		TextOut(hDC, 
				giTableMargin + giCardWidth + giColSpacing + XVALUEOFFSET-5, 
				giTableMargin + YVALUEOFFSET + VTEXTSPACING*3,
				szBuff,
				wsprintf(szBuff, "               " ) );	
		}
	 
   ReleaseDC(hWnd, hDC);
}





	