/*----------------------------------------------------------------------

	6x6ttabl.cpp -  Class implementations for the solitaire 
	              card game "six by six"
	
            Doug Rogers, 08/19/95

Classes:
--------
TTable

Changes:
--------
08/19/95 15:06 - First implementation
08/23/95 15:12 - Mods to handle bent stacks
2.15  09/12/95 06:30 - Broke this file off from climp.cpp

-----------------------------------------------------------------------*/

#include <stdlib.h>    //for rand() and srand()
#include <time.h>		//for srand(), random seed value
#include <windows.h>
#include <windowsx.h>
//#include <time.h>    //for randomize()
//#include <dos.h>		//for delay()

#include "6x6defs.h"
#include "6x6cldef.h"
#include "6x6fndef.h"

/******** Global Variables ***********/
extern	int		giCardHeight;
extern	int		giCardWidth;
extern	int		giHomePileSpace;
extern	int		giColSpacing;
extern	int		giTableMargin;
extern	float	gfVFracShowing, gfHFracShowing;
extern	TDeck	gDeck;
extern	TTable	gTable;
extern	char	gcSuits[], gcValues[];
extern	BOOL	gbStackSaved;	//Flag indicating a stack has been
								//"erased" and is saved 
extern	int		giCardStack[52];	//Stack to store cards being moved
extern	int		giColTop;                        
extern	int		giClientHeight, giClientWidth;   //Size of the client window
extern	BOOL	giWidenWindow;			//Flag (and size) to widen window
extern	HBITMAP	ghSuitBitMap[4];
extern	BOOL	gbDrawBMaps;
extern	HBITMAP	ghCardBitMap[16];//Handles to the bitmaps for the card faces
								 //[0-4]=Aces, [5-15]=other cards
extern	int		giCardsInPiles;  //Count to see if game has been won								 
extern	int		giClickCount;   //How many total clicks made so far								

extern	BOOL	gbShowScore;
extern	BOOL	gbShowTime;

BOOL	gbFlagZone[6][2];

extern HRGN ghrgnTmp[6];  //Temporary regions for copying, etc.
extern HRGN gClipRgn;     //The Clip Region to be used when erasing and drawing

RECT	grBlankBentZone[6]; //Bent zones to be erased

typeCardsToDraw gCardsToDraw;	




/***********************************************************
Class: 	TTable
Purpose: Represents the layout of the cards on the table
***********************************************************/

/*=============== TTable::TTable() and ~TTable() ==========================
Purpose: Default constructor and destructors
======================================================================*/
TTable::TTable(void)
{
	Clear();

	for(int iCol=0; iCol<6; iCol++)
		{
		Col[iCol].CRgn=CreateRectRgn(0,0,0,0);
		Col[iCol].BRgn=CreateRectRgn(0,0,0,0);
		Col[iCol].CBRgn=CreateRectRgn(0,0,0,0);
		Col[iCol].ClipRgn=CreateRectRgn(0,0,0,0);
		}
}

TTable::~TTable(void)
{
	for(int iCol=0; iCol<6; iCol++)
		{
		DeleteRgn(Col[iCol].CRgn);
		DeleteRgn(Col[iCol].BRgn);
		DeleteRgn(Col[iCol].CBRgn);
		DeleteRgn(Col[iCol].ClipRgn);
		}
}

/*=============== TTable::Clear(void) =================================
Purpose: Clear the array of values representing the layout on the
         table.  This function avoids direct access to
         the private array data.  
Strategy:

======================================================================*/
void TTable::Clear(void)
{
	for(int iCol=0; iCol<MAXCOLS; iCol++)
		{
		Col[iCol].iNumCards=0;
		Col[iCol].iNumBentCards=0;
		Col[iCol].iWhichCardBent=0;
		Col[iCol].iRightEdge=0;
		for(int iRow=0; iRow<MAXROWS; iRow++)
			Card[iRow][iCol]=0;
		}
}


/*========= TTable::GetCard(int iRow, int iCol) =================
Purpose: Return the value of the card on the table at the
		 position iRow, iCol.
Strategy:

======================================================================*/
int	TTable::GetCard(int iRow, int iCol)
{
	return Card[iRow][iCol];
}


/*=================== TTable::GetCardsInCol(int iCol) ==========
Purpose: Obtain the count of the number of cards in a
         column of the layout
Strategy:

======================================================================*/
int	TTable::GetCardsInCol(int iCol)
{
	return	Col[iCol].iNumCards;
}
                    
/*=================== TTable::IncCardsInCol(int iCol) ==========
Purpose: Increase the count of the number of cards in a
         column of the layout
Strategy:

======================================================================*/
int	TTable::IncCardsInCol(int iCol)
{

	Col[iCol].iNumCards++;
		
	return OK;
}
                    

/*=================== TTable::DecCardsInCol(int iCol) ==========
Purpose: Decrease the count of the number of cards in a
         column of the layout

Strategy:

======================================================================*/
int	TTable::DecCardsInCol(int iCol)
{

	Col[iCol].iNumCards--;
	
	return OK;
}
                    
                    
	
/*=============== TTable::Draw(HWND hWnd) ===============================
Purpose: Draw the table layout

Strategy:

======================================================================*/
void TTable::Draw( HWND hWnd ) 
{
	char	szBuff[10];         
	int iCol, iRow;
	HBRUSH hbrushOld, hbrushWhite;

	if(giClickCount>0) //Click count <=0 is a flag to redraw all
		{
		if(gCardsToDraw.bScore)
			if(gbShowScore)
				ShowClickCount(hWnd);    
						
		HDC hDC=GetDC(hWnd);
		if(gCardsToDraw.bDeck)
			{
			TextOut(hDC, 
				giTableMargin + XVALUEOFFSET, 
				giTableMargin + YVALUEOFFSET+5,
				szBuff, 
				wsprintf(szBuff, "  %2d  ",gDeck.GetCardsRemaining()) );	
		
			TextOut(hDC, 
				giTableMargin + XVALUEOFFSET, 
				giTableMargin + YVALUEOFFSET + VTEXTSPACING+5,
				szBuff,
				wsprintf(szBuff, " Card%c ", 
					    (gDeck.GetCardsRemaining()==1)?' ':'s') );	

		   }
	   ReleaseDC(hWnd, hDC);
						
		if(gCardsToDraw.iHomePile)
			{
			DrawCard(hWnd, 0, gCardsToDraw.iHomePile+1); //HomePiles count 1,2,3,4
			gCardsToDraw.iHomePile=0;
			}
	
/* Reminder:
		int	iNumCards;   	//The current number of cards in the stack
		int	iNumBentCards;  //How many cards go sideways (after first)
		int	iWhichCardBent;	//The number of the last vertical card
		int 	iRightEdge;		//X coord of right edge of bent cards
*/	         


//Now erase any bent cards which are going to be rearranged 
		hDC=GetDC(hWnd);
	
	   hbrushWhite=GetStockObject(WHITE_BRUSH);
		hbrushOld=SelectObject(hDC, hbrushWhite); //Solid white brush
	
		for(iCol=5; iCol>=0; iCol--)
			if(gCardsToDraw.bZone[iCol][1])
			   FillRect(hDC, &grBlankBentZone[iCol], hbrushWhite);

	   SelectObject(hDC, hbrushOld); //restore previous brush
	
	   ReleaseDC(hWnd, hDC);


//Now draw the cards which need to be drawn...
		for(iCol=5; iCol>=0; iCol--)
			{
					
			if(gCardsToDraw.bZone[iCol][0])
				{
				for(iRow=1; iRow<=Col[iCol].iWhichCardBent; iRow++)
						DrawCard(hWnd, iRow, iCol);
				}
			if(gCardsToDraw.bZone[iCol][1])
				{
				for(iRow=Col[iCol].iWhichCardBent+1; iRow<=GetCardsInCol(iCol); iRow++)
						DrawCard(hWnd, iRow, iCol);
				}
			}	
		}
	else //In cases like REDO_ALL, draw the whole thing over:
		{           
		giClickCount*=(-1);
		for(iCol=5; iCol>=0; iCol--)
			for(iRow=0; iRow<=GetCardsInCol(iCol); iRow++)
				if(!(iRow==0 && iCol==1))    //Skip space bet deck and h.p.
					DrawCard(hWnd, iRow, iCol);
		
		ShowCardsRemaining(hWnd);                       
		if(gbShowScore)
			ShowClickCount(hWnd);    
		if(gbShowTime)
			ShowElapsedTime(hWnd);    
		}

//Clear all the Zone paint flags
	for(iCol=0; iCol<6; iCol++)
		{
		gbFlagZone[iCol][0]=FALSE;
		gbFlagZone[iCol][1]=FALSE;
		}
}


/*=================== TTable::AddStack(HWND hWnd, int iCol) ==========
Purpose: Add the saved stack to the indicated column in the array

Strategy:

======================================================================*/
int TTable::AddStack(HWND hWnd, int iCol, BOOL bFollowRules)
{         
	int iStackPointer=0;                        
	
    if(bFollowRules)
		if( ( Col[iCol].iNumCards != 0 )  &&
			(giCardStack[0]%100) != (Card[ Col[iCol].iNumCards ][iCol]%100)-1 )

			{
			AddStack(hWnd, giCardStack[51], FALSE);   //Return the stack to the original place
			return PROBLEM;    //Card at top of stack is not one less
							   //than target card
			}
				
	while(giCardStack[iStackPointer] != 0 )
		{
		AddCard(hWnd, giCardStack[iStackPointer], iCol);
		giCardStack[iStackPointer++]=0;
		}

	gbStackSaved=FALSE;		//Lower flag
	SetHitZones(LAYOUT_MOVE);          //Recalculate hit zones 

	return NOPROBLEM;
}
         
         
/*==== TTable::AddCard(HWND hWnd, int CardValue, int iCol) =============
Purpose: Add a card to the bottom of a row in the array of 
		 data representing the table layout.  The CardValue
		 is the usual encoded card information.
Strategy:

======================================================================*/
int TTable::AddCard(HWND hWnd, int CardValue, int iCol)
{                 
	int iPrevCol;

	if(iCol<0 || iCol>MAXCOLS) return PROBLEM;

	Col[iCol].iNumCards++;
 	Card[ Col[iCol].iNumCards ] [iCol]=CardValue;  

//	DrawCard(hWnd, Col[iCol].iNumCards, iCol);

	//If the latest card goes past the edge of the window, widen it
	if(Col[iCol].iRightEdge > giClientWidth) 
				giWidenWindow=Col[iCol].iRightEdge;

	return NOPROBLEM;
}


/*======== TTable::AddToPile(HWND hWnd, int iCol, BOOL bPileKnown) =======
Purpose: Add the card saved on the stack to the home pile
		 indicated by the column in the array
Strategy:

======================================================================*/
int TTable::AddToPile(HWND hWnd, int iCol, BOOL bPileKnown)
{                                             
    int		iPileCard;
	int		iPileVal, iCardVal, iPileSuit, iCardSuit;
	int     iNumInStack=0;   
	int		iStackPointer=0;

//First, find out what the bottom card of the stack is and how many 
//   cards there are in the stack
	while(giCardStack[iNumInStack] != 0 )
		{
		iNumInStack++;
		}

	iCardVal=(giCardStack[iNumInStack-1]%100);
	iCardSuit=(giCardStack[iNumInStack-1]/100);

	if(!bPileKnown)
		{
		iCol=2;   //It will flunk on any col. if it goes all through loop
		for(int iPileNum=2; iPileNum<6; iPileNum++)
			{
			iPileCard=Card[0][iPileNum];
			if((iCardVal==1) && (iPileCard==0))  //If the card's an Ace
				{                                //drop it on the first
				iCol=iPileNum;                    //available pile
				break;
				}  
			iPileVal=(iPileCard%100);
			iPileSuit=(iPileCard/100);
			if(iCardSuit == iPileSuit)
				{                              //Otherwise search all
				iCol=iPileNum;                  //the piles for the right
				break;                         //suit
				}  
			}				
		}	

	iPileCard=Card[0][iCol];
	iPileVal=(iPileCard%100);
	iPileSuit=(iPileCard/100);


	if( ( (iPileCard==0) && (iCardVal==1) )  || 
			((iCardVal == iPileVal+1) && (iCardSuit == iPileSuit)) )
 		{
 		Card[0][iCol]=giCardStack[0];  //If the bottom card in the stack
 		                               //was OK, then since the stack was
 		                               //sequential and all one suit, the
 		                               //top card can be the new pile top
		for(iStackPointer=0; iStackPointer<iNumInStack; iStackPointer++)
			giCardStack[iStackPointer]=0;  //Clear the stack 		                               
		giCardsInPiles+=iNumInStack;  //Count to see if game has been won
		MakePaintRegion(HOME_PILE_MOVE+(iCol-1)); //Home piles are 1,2,3,4

		gbStackSaved=FALSE;		//Lower flag
		}
 		
 	  else
		{
		AddStack(hWnd, giCardStack[51], FALSE);   //Return the stack to the original place
		return PROBLEM;    //Card at top of stack is not one less
						   //than target card
		}

//	DrawCard(hWnd, 0, iCol);

	return NOPROBLEM;
}
         
         
/*===== TTable::RemoveStack(HWND hWnd, int TopCard, int iCol) ===========
Purpose: Remove a stack of cards from TopCard to the bottom of
		 the column in the array
Strategy:
======================================================================*/
int TTable::RemoveStack(HWND hWnd, int iTopCard, int iCol)
{            
	int iStackPointer=0;
	int iNumCardsInStack;
	int iBottomCardSuit;

	iNumCardsInStack=Col[iCol].iNumCards-iTopCard; //This is actally one less

	iBottomCardSuit=Card[ Col[iCol].iNumCards ][iCol]/100;
	for(iStackPointer=Col[iCol].iNumCards-1; iStackPointer>=iTopCard; iStackPointer--)
		if(Card[iStackPointer][iCol]/100 != iBottomCardSuit ) 
		return PROBLEM;
	
	for(iStackPointer=iNumCardsInStack; iStackPointer>=0; iStackPointer--)
		{
		giCardStack[iStackPointer]=Card[ Col[iCol].iNumCards ] [iCol];
		RemoveCard(hWnd, iCol);				
		}   
	giCardStack[51]=iCol;	//Place to store info about where stack 
							//came from	

	gbStackSaved=TRUE;		//Raise flag
	SetHitZones(LAYOUT_MOVE);          //Recalculate hit zones 

	return NOPROBLEM;

}



/*=================== TTable::RemoveCard(HWND hWnd, int iCol) ==========
Purpose: Remove the last card from a column in the array

Strategy:

======================================================================*/
int TTable::RemoveCard(HWND hWnd, int iCol)
{
	int iPrevCol, iCheckCol;

	if(Col[iCol].iNumCards != 0)     
		{   
 		Card[ Col[iCol].iNumCards ] [iCol]=0;

      //Check to see if we accidentally erase any cards in cols to the right
      
		if(iCol<5 && (gTable.Col[iCol].iNumBentCards > 0)) //If bent cards, check if cards above will
		           														//be messed up by erasure
			{
			for(iCheckCol=5; iCheckCol>iCol; iCheckCol--)
				if( (Col[iCheckCol].HZ[0].left
				                          < Col[iCol].iRightEdge)
						&& (Col[iCol].HZ[1].top <= 
								( (Col[iCheckCol].iNumBentCards > 0) ?
						        (Col[iCheckCol].HZ[1].bottom) :
						        (Col[iCheckCol].HZ[0].bottom)
						            ) ) )//And it bends before the bottom of this col...

						 	{                                                                  
						 	gbFlagZone[iCheckCol][0]=TRUE;  //change the hit zone a little
						 	if(gTable.Col[iCheckCol].iNumBentCards > 0)
							 	gbFlagZone[iCheckCol][1]=TRUE;  //change the hit zone a little
							}
			}

		EraseCard(hWnd, Col[iCol].iNumCards, iCol); 

		Col[iCol].iNumCards--;

		}

	return NOPROBLEM;
}


/*====== TTable::EraseCard(HWND hWnd, int iCardRow, int iCardCol) ===========================================================
Purpose: Erase the card which is at the location iCardCol,
		 iCardRow in the card table.
Strategy:

============================================================================*/
int	TTable::EraseCard(HWND hWnd, int iCardRow, int iCardCol)
{     
	HPEN hpenOld, hpenWhite;
	HBRUSH hbrushOld, hbrushWhite;
	RECT rBlankRect;
	HDC hDC=GetDC(hWnd);

	SelectClipRgn(hDC, Col[iCardCol].ClipRgn);	 
	                                     
   hpenWhite=GetStockObject(WHITE_PEN);
   hpenOld=SelectObject(hDC, hpenWhite);
                                     
   hbrushWhite=GetStockObject(WHITE_BRUSH);
	hbrushOld=SelectObject(hDC, hbrushWhite); //Solid white brush

		if( (Col[iCardCol].iNumBentCards==0) || 
					(iCardRow <= Col[iCardCol].iWhichCardBent) )
			{
			SetRect(&rBlankRect, 
			  Col[iCardCol].HZ[0].left, 
			  giColTop+(iCardRow-1)*(gfVFracShowing*giCardHeight),
			  Col[iCardCol].HZ[0].left + giCardWidth,
			  giColTop+(iCardRow-1)*(gfVFracShowing*giCardHeight)+giCardHeight
			  );
		   Col[iCardCol].HZ[0].bottom -= (gfVFracShowing*giCardHeight);
			}       
		  else
		  	{
			SetRect(&rBlankRect, 
			  Col[iCardCol].HZ[1].right - giCardWidth,
			  Col[iCardCol].HZ[1].top, 
			  Col[iCardCol].HZ[1].right, 
			  Col[iCardCol].HZ[1].bottom
			  );
		   Col[iCardCol].HZ[1].right -= giCardWidth*gfHFracShowing;  //Adjust hit zone...
		   Col[iCardCol].iRightEdge -= giCardWidth*gfHFracShowing;  //Adjust hit zone...
			}     

   FillRect(hDC, &rBlankRect, hbrushWhite);
   SelectObject(hDC, hbrushOld); //restore previous brush
   SelectObject(hDC, hpenOld); //restore old pen

	if(iCardRow>1)
		DrawCard(hWnd, iCardRow-1, iCardCol); 

	SelectClipRgn(hDC, NULL);//restore the full window region	

   ReleaseDC(hWnd, hDC);
	return OK;
}	      
                                 

/*=============== TTable::SetHitZones(void) ===========================================================
Purpose: Define all the RECTs for all the interesting zones a mouse can
		 hit:
 Col[0].HZ[2] = The Deck
 Col[1].HZ[2] = dead space
 Col[2-5].HZ[2] = Home Piles
 Col[0-5].HZ[0]= Card Column, vertical part
            [1]= Card Column, bent part, including all of bottom vertical
                     card

Strategy:

============================================================================*/
void TTable::SetHitZones(int iMoveType)
{   
	int	iCol, iLeftCol, iPrevCol;   
	int iMaxRightEdge=0;               //To figure out how far over the
	                                 // widest pile extends
	static BOOL bAlreadyDone=FALSE;   //the first batch of hit zones
								      // only have to be done once								
	if( ! bAlreadyDone )
	{	
	//The Deck pile:
    	Col[0].HZ[2].top = giTableMargin;
    	Col[0].HZ[2].left = giTableMargin;
    	Col[0].HZ[2].bottom = Col[0].HZ[2].top + giCardHeight;
    	Col[0].HZ[2].right = Col[0].HZ[2].left + giCardWidth;

	//The four home piles:
	    for(iCol=1; iCol<6; iCol++)
    		{
	    	Col[iCol].HZ[2].top = giTableMargin;
    		Col[iCol].HZ[2].left = Col[0].HZ[2].right + giColSpacing + (iCol-1)*(giCardWidth+giColSpacing);
    		Col[iCol].HZ[2].bottom = giTableMargin + giCardHeight;
    		Col[iCol].HZ[2].right = Col[iCol].HZ[2].left + giCardWidth;                
    		}
		bAlreadyDone=TRUE;
	}    		
                      
//The six columns:                      

//First, analyze the whole layout
	//Figure the max number of cards which will fit in the current window
	//size
    int iMaxCardFit=(giClientHeight-giColTop)/(gfVFracShowing*giCardHeight);
	if(iMaxCardFit<7)iMaxCardFit=7;  // 7 is the minimum for a bend in col 0

//Now figure out the parameters for the first column...
/* Reminder:
		int	iNumCards;   	//The current number of cards in the stack
		int	iNumBentCards;  //How many cards go sideways (after first)
		int	iWhichCardBent;	//The number of the last vertical card
		int iRightEdge;		//X coord of right edge of bent cards
*/	         
//Now recursively figure out the parameters for the other columns...	         
	for(iCol=0; iCol<6; iCol++)
		{
		Col[iCol].iNumBentCards = 0;  //Clear this initially

		//First, see if the previous columns are bent, and if
		//   the bend interferes with this column
		if(iCol>0)
			for(iLeftCol=iCol-1; iLeftCol>=0; iLeftCol--)
				{
				if((Col[iLeftCol].iNumBentCards != 0) && 
					(Col[iLeftCol].iWhichCardBent <= Col[iCol].iNumCards) &&
						(Col[iLeftCol].iRightEdge > Col[iCol].HZ[0].left) )
					{
					Col[iCol].iWhichCardBent=Col[iLeftCol].iWhichCardBent-1;
					Col[iCol].iNumBentCards=Col[iCol].iNumCards
				    	                       -Col[iCol].iWhichCardBent;
					Col[iCol].iRightEdge=Col[iCol].HZ[0].left + (Col[iCol].iNumBentCards*gfHFracShowing + 1)*giCardWidth;

					break;
					}
				}			               
		if(Col[iCol].iNumBentCards == 0)
			{		
			if(Col[iCol].iNumCards > iMaxCardFit)
				{
				Col[iCol].iNumBentCards=Col[iCol].iNumCards-iMaxCardFit;
				Col[iCol].iWhichCardBent=iMaxCardFit;
				Col[iCol].iRightEdge=Col[iCol].HZ[0].left + 
				    (Col[iCol].iNumBentCards*gfHFracShowing + 1)*giCardWidth;
				}
		  	  else
				{
				Col[iCol].iNumBentCards=0;
				Col[iCol].iWhichCardBent=Col[iCol].iNumCards; 
								//No bent cards, but this is the bottom 
								//  of the vertical part of the stack
				Col[iCol].iRightEdge=Col[iCol].HZ[0].right;
				}
			}
		}				

	//See if any of the new arrangement requires widening of the window
	for(iCol=0; iCol<6; iCol++)
		if(Col[iCol].iRightEdge > iMaxRightEdge)
			iMaxRightEdge = Col[iCol].iRightEdge;
	if(iMaxRightEdge > giClientWidth) giWidenWindow = iMaxRightEdge;


/* Reminder:
		int	iNumCards;   	//The current number of cards in the stack
		int	iNumBentCards;  //How many cards go sideways (after first)
		int	iWhichCardBent;	//The number of the last vertical card
		int iRightEdge;		//X coord of ri`ght edge of bent cards
*/	         

//Then, figure out the new HitZones...
    for(iCol=0; iCol<6; iCol++)
        {
//First, figure the vertical part...
        Col[iCol].HZ[0].top = giColTop;
        Col[iCol].HZ[0].left = giTableMargin + 
        						(giCardWidth + giColSpacing)*iCol ;
        Col[iCol].HZ[0].right = Col[iCol].HZ[0].left + giCardWidth;
                                                          
		if(Col[iCol].iNumCards==0)	
			//If there are no cards in the col, the hit zone is the space
			//  of one card in the empty col	
        	Col[iCol].HZ[0].bottom = giColTop + giCardHeight+1;
          else                    //the -1 distiguishes no cards from >0 cards
            //If there's one or more cards, allow for overlap in figuring
            //  how long the column is...
			if(Col[iCol].iNumBentCards != 0) //If there is a bend in the col...
        		Col[iCol].HZ[0].bottom = giColTop + 
      						(Col[iCol].iWhichCardBent-1) * gfVFracShowing
      														*giCardHeight+2;
							
			  else
        		Col[iCol].HZ[0].bottom = giColTop + 
      						( ( (Col[iCol].iNumCards-1) * gfVFracShowing)+1)
      															*giCardHeight;
      															
//Now figure the horizontal cards (Remember, if hits are figured
//  out from the left, then overlap doesn't matter since the leftmost
//  stack will get triggered first in the overlapped area
		if( Col[iCol].iNumBentCards != 0 )
			{
        	Col[iCol].HZ[1].top = Col[iCol].HZ[0].bottom;
        	Col[iCol].HZ[1].left = Col[iCol].HZ[0].left;
        	Col[iCol].HZ[1].right = Col[iCol].HZ[0].left + 
	        	(Col[iCol].iNumBentCards*gfHFracShowing + 1)*giCardWidth;
			Col[iCol].HZ[1].bottom = Col[iCol].HZ[1].top + 
															giCardHeight;
			//(this last hit zone may overlap with ones to the left)
			}
		 else
		   SetRect(&Col[iCol].HZ[1], 0, 0, 0, 0);                                             
		}

// Now make the regions which correspond to the hit zones
	DeleteRgn(ghrgnTmp[0]);   //These were already created...
	DeleteRgn(ghrgnTmp[1]);
	DeleteRgn(ghrgnTmp[2]);

	for(iCol=0; iCol<6; iCol++)
		{
		ghrgnTmp[0]=CreateRectRgnIndirect(&Col[iCol].HZ[0]);
		ghrgnTmp[1]=CreateRectRgnIndirect(&Col[iCol].HZ[1]);
		ghrgnTmp[2]=CreateRectRgnIndirect(&Col[iCol].HZ[2]);
		CopyRgn(Col[iCol].CRgn, ghrgnTmp[0]);
		CopyRgn(Col[iCol].BRgn, ghrgnTmp[1]);
		UnionRgn(ghrgnTmp[3], Col[iCol].CRgn, Col[iCol].BRgn); //
		UnionRgn(Col[iCol].CBRgn, ghrgnTmp[3], ghrgnTmp[2]); //
		DeleteRgn(ghrgnTmp[0]);
		DeleteRgn(ghrgnTmp[1]);
		DeleteRgn(ghrgnTmp[2]);
		}

	ghrgnTmp[0]=CreateRectRgn(0,0,0,0);
	ghrgnTmp[1]=CreateRectRgn(0,0,0,0);
	ghrgnTmp[2]=CreateRectRgn(0,0,0,0);
	for(iCol=0; iCol<6; iCol++)                  
		{
		CopyRgn(ghrgnTmp[0], Col[iCol].CBRgn); //Store it here for now
	
		for(iPrevCol=0; iPrevCol<iCol; iPrevCol++)
			SubtractRgn(ghrgnTmp[iPrevCol+1], ghrgnTmp[iPrevCol], Col[iPrevCol].BRgn);
	
		CopyRgn( Col[iCol].ClipRgn, ghrgnTmp[iCol]); //Store the result
		}

	MakePaintRegion(iMoveType);
//	InvalidateRgn(hWnd, ghrgnRepaintRgn, TRUE); 

}


/*=============== TTable::GetZone(int iHClickPos, int iVClickPos) ===========================================================
Purpose: Find out which of the hit zones the mouse was clicked 
		 in.  Return this value, otherwise return NOTINAZONE
Strategy:

============================================================================*/
int	TTable::GetZone(int iHClickPos, int iVClickPos)
{     
	POINT ptClickPoint;
	int	iZoneNum;
	
	ptClickPoint.x=iHClickPos;
	ptClickPoint.y=iVClickPos;

//Zone numbers are 0=Deck, 1=blank, 2,3,4,5=Home Piles, 6,7,8,9,10,11=Card col	
	for(int iCol=0; iCol<6; iCol++)
		{
		if(PtInRect(&Col[iCol].HZ[0], ptClickPoint))return iCol+COLZONEOFFSET;
		if(Col[iCol].iNumBentCards)
			if(PtInRect(&Col[iCol].HZ[1], ptClickPoint))return iCol+COLZONEOFFSET;
		if(PtInRect(&Col[iCol].HZ[2], ptClickPoint))return iCol;
		}		
		
	return NOTINAZONE;
}


/*==== TTable::GetCardInCol(int iHClickPos, int iVClickPos, int iZoneNum) ====
Purpose: Find out which card in the column was hit
Strategy:

============================================================================*/
int TTable::GetCardInCol(int iHClickPos, int iVClickPos, int iZoneNum)
{            
	int iCard, iCardCol;
	POINT ptClickPoint;
	
	ptClickPoint.x=iHClickPos;
	ptClickPoint.y=iVClickPos;
	
	iCardCol=iZoneNum-COLZONEOFFSET;
	
	if(PtInRect(&Col[iCardCol].HZ[0], ptClickPoint))
		{                                            
		iCard=(iVClickPos-giColTop)/(gfVFracShowing*giCardHeight)+1;
		if( iCard > Col[iCardCol].iNumCards )
			iCard = Col[iCardCol].iNumCards;	//Bottom card is full length				
		
		}
	  else  //Must be in bent zone
	  	{   
		iCard=(iHClickPos-Col[iCardCol].HZ[1].left)/
									(gfHFracShowing*giCardWidth)
											+ Col[iCardCol].iWhichCardBent;
		if( iCard > Col[iCardCol].iNumCards )
			iCard = Col[iCardCol].iNumCards;	//Last card is full length
	  	
		}

	return iCard;
}



/*====== TTable::DrawCard(HWND hWnd, int iCardRow, int iCardCol) ===========================================================
Purpose: Draw the card which is at the location iCardCol,
		 iCardRow in the card table.
Strategy:

============================================================================*/
int	TTable::DrawCard(HWND hWnd, int iCardRow, int iCardCol)
{                            
	HDC hDC=GetDC(hWnd);

	SelectClipRgn(hDC, Col[iCardCol].ClipRgn);	 
					
	HPEN hpenOld, hpenBlack;
	HBRUSH hbrushOld, hbrushWhite;
	                                     
   hpenBlack=GetStockObject(BLACK_PEN);
   hpenOld=SelectObject(hDC, hpenBlack);
                                     
   hbrushWhite=GetStockObject(WHITE_BRUSH);
	hbrushOld=SelectObject(hDC, hbrushWhite); //Solid white brush

	if(iCardRow==0)
		RoundRect(hDC, 
					Col[iCardCol].HZ[2].left, 
					Col[iCardCol].HZ[2].top,
					Col[iCardCol].HZ[2].right,
					Col[iCardCol].HZ[2].bottom,
					CORNERELLIPSE, CORNERELLIPSE); 						
	  else
		if( (Col[iCardCol].iNumBentCards==0) || 
					(iCardRow <= Col[iCardCol].iWhichCardBent) )
			RoundRect(hDC, 
			  Col[iCardCol].HZ[0].left, 
			  giColTop+(iCardRow-1)*(gfVFracShowing*giCardHeight),
			  Col[iCardCol].HZ[0].left + giCardWidth,
			  giColTop+(iCardRow-1)*(gfVFracShowing*giCardHeight)+giCardHeight,
			  CORNERELLIPSE, CORNERELLIPSE
			  );     
		  else
			RoundRect(hDC, 
			  Col[iCardCol].HZ[0].left 
			  				+ (iCardRow-Col[iCardCol].iWhichCardBent)
			  				   * (gfHFracShowing*giCardWidth), 
			  Col[iCardCol].HZ[1].top, 
			  Col[iCardCol].HZ[0].left 
			  				+ (iCardRow-Col[iCardCol].iWhichCardBent)
			  				   * (gfHFracShowing*giCardWidth) + giCardWidth,
			  Col[iCardCol].HZ[1].top + giCardHeight,
			  CORNERELLIPSE, CORNERELLIPSE
			  );     

	if(Card[iCardRow][iCardCol]!=0)			
		DrawValue(hWnd, iCardRow, iCardCol);
	
   SelectObject(hDC, hbrushOld); //restore previous brush
   SelectObject(hDC, hpenOld); //restore old pen
	
	SelectClipRgn(hDC, NULL);//restore the full window region	

   ReleaseDC(hWnd, hDC);
	return OK;
}	      
                                 
                                 
/*====== TTable::DrawValue(HWND hWnd, int iCardRow, int iCardCol) ===========================================================
Purpose: Draw the valueof the card which is at the location
		 iCardCol, iCardRow in the card table.
Strategy:

============================================================================*/
int	TTable::DrawValue( HWND hWnd, int iCardRow, int iCardCol ) 
{
	char	szBuff[10];
	int		iCardValue;
	char	cVal1, cVal2, cSuit;
	short	iSuitNum, iCardNum;
	short	xStart, yStart;
	HDC hDC=GetDC(hWnd);

	SelectClipRgn(hDC, Col[iCardCol].ClipRgn);	 
	
	iCardValue=gTable.GetCard(iCardRow, iCardCol);
	cVal1=gcValues[(iCardValue%100)*2];
	cVal2=gcValues[(iCardValue%100)*2+1];
	cSuit=gcSuits[(iCardValue/100)];
	iCardNum=iCardValue%100;
	iSuitNum=iCardValue/100 - 1;

	if(iCardRow==0)
		{
		xStart=Col[iCardCol].HZ[2].left;
		yStart=Col[iCardCol].HZ[2].top;
		}
	  else
		if( (Col[iCardCol].iNumBentCards==0) || 
					(iCardRow < Col[iCardCol].iWhichCardBent) )
		  	{				
			xStart=Col[iCardCol].HZ[0].left;
			yStart=giColTop+(iCardRow-1)*(gfVFracShowing*giCardHeight);
			}
		  else
            {
			xStart=Col[iCardCol].HZ[0].left 
			 				+ (iCardRow-Col[iCardCol].iWhichCardBent)
			  				   * (gfHFracShowing*giCardWidth); 
			yStart=Col[iCardCol].HZ[1].top;
			}                                             
				
	if(gbDrawBMaps)
		{
		TextOut(hDC, xStart+XVALUEOFFSET, yStart+YVALUEOFFSET, szBuff, 
			         wsprintf(szBuff, "%c%c",cVal1, cVal2 ) );
	
		DrawBitmap(hDC, ghSuitBitMap[iSuitNum],
								 xStart+SBMHOFFSET, yStart+SBMVOFFSET);
		}
	  else
	  	{
		TextOut(hDC, xStart+XVALUEOFFSET, yStart+YVALUEOFFSET, szBuff, 
			         wsprintf(szBuff, "%c%c %c",cVal1, cVal2, cSuit ) );
		}


                                
	if(iCardRow>=Col[iCardCol].iNumBentCards || iCardRow==Col[iCardCol].iNumCards || iCardRow==0) //If this is the last card in stack
		{
		if(iCardNum==1)  //If it's an Ace
			DrawBitmap(hDC, ghCardBitMap[iSuitNum],
								 xStart+CARDBM_HOFFSET, yStart+CARDBM_VOFFSET);
		  else
			DrawBitmap(hDC, ghCardBitMap[iCardNum+2],
								 xStart+CARDBM_HOFFSET, yStart+CARDBM_VOFFSET);
		}
					
	SelectClipRgn(hDC, NULL);//restore the full window region	

   ReleaseDC(hWnd, hDC);
	return OK;					
}


