/*----------------------------------------------------------------------------
	6x6cldef.h -  Class definitions for the solitaire card game "six by six"

                   Doug Rogers, 08/19/95

Structures:
-----------
typeCol

Classes:
--------
TCard
TDeck
TTable


Changes:
--------
08/19/95 15:06 - First implementation
08/23/95 15:12 - Mods to handle bent stacks (defined tagStack)

---------------------------------------------------------------------------*/

#ifndef __6x6CLDEF_H     /* prevent multiple includes */
#define __6x6CLDEF_H

#include "6x6defs.h"
#include <windows.h>    //for definition of HDC

//Functions: ***********************
int		GetZone(int iHClickPos, int iVClickPos);
void    SetHitZones(int iMoveType);


//Structures: **************************

typedef struct tagCol
		{
		int	iNumCards;   	//The current number of cards in the stack
		int	iNumBentCards;  //How many cards go sideways (after first)
		int	iWhichCardBent;	//The number of the first card out of line
		int	iRightEdge;		//X coord of right edge of bent cards
		RECT  HZ[3];        //Three Hit Zones: 0=main col, 1=bent, 2=pile
		HRGN	CRgn;       //Region covering main column
		HRGN	BRgn;       //Region covering bent part
		HRGN	HPRgn;      //Region covering home pile associated with this col
		HRGN	CBRgn;       //Region covering both parts
		HRGN	ClipRgn;    //Valid region to draw into 
		}typeCol;

//Classes: **************************

class TCard;	//pre-definition of classes
class TDeck;
class TTable;

/*********
Class: 	TCard
Purpose: Represents a card in the deck
**********/
class TCard 
{
public:
	TCard(void);
	~TCard(void);
	void	Draw(int iRow, int iCol);
	void	DrawTop(int iRow, int iCol);
	void	Erase(int iRow, int iCol);
	void	EraseTop(int iRow, int iCol);
	
private:
	char	Suit;
	char	Number;
};

	
/*********
Class: 	TDeck
Purpose: Represents a deck of cards
**********/

class TDeck
{
public:
	TDeck(void);
	~TDeck(void);
	void	Shuffle(void);
	void	Deal(TTable & Table, HWND hWnd);
	int		NextCard(HWND hWnd);
	int 	GetCardsRemaining(void);
	
private:
	int	Card[52];
	int	iCurrentCardIndex;
	int	iCurrentCard;
	int	iCardsRemaining;
};	

	
/*********
Class: 	TTable
Purpose: Represents the layout of the cards on the table
**********/
class TTable
{
public:
	TTable(void);
	~TTable(void);
	void	Draw(HWND hWnd);
	void	Clear(void);
	int		GetCard(int iRow, int iCol);
	int		AddCard(HWND hWnd, int CardValue, int iCol);
	int		RemoveCard(HWND hWnd, int iCol);
	int		AddStack(HWND hWnd, int iCol, BOOL bFollowRules=TRUE);
	int		AddToPile(HWND hWnd, int iCol, BOOL bPileKnown);
	int		RemoveStack(HWND hWnd, int TopCard, int iCol);
	int		GetCardsInCol(int iCol);
	int		IncCardsInCol(int iCol);
	int		DecCardsInCol(int iCol);
	int		GetZone(int iHClickPos, int iVClickPos);
	void     SetHitZones(int iMoveType);                      
	int		GetCardInCol(int iHClickPos, int iVClickPos, int iZoneNum);
	int		DrawCard(HWND hWnd, int iCardRow, int iCardCol);
	int		EraseCard(HWND hWnd, int iCardRow, int iCardCol);
	int		DrawValue(HWND hWnd, int iCardRow, int iCardCol);
	friend	BOOL MakePaintRegion(int iMoveType);
	
	
private:
	int	Card[MAXROWS][MAXCOLS];		//Array representing the layout of the 
									//cards on the table, where Row 0 is
									//reserved for the undealt deck and the
									//home piles.
//	RECT HitZone[NUMBEROFZONES][2];	//These are "hit zones" on the table        
	RECT OldHitZone[6][2];	//These are to compare to the updated ones

	typeCol Col[6];	//Structure filled with info about the cards
							//in the column
/*
struct tagCol
		{
		int	iNumCards;   	//The current number of cards in the stack
		int	iNumBentCards;  //How many cards go sideways (after first)
		int	iWhichCardBent;	//The number of the first card out of line
		int iRightEdge;		//X coord of right edge of bent cards
		};
*/

};
	
	
#endif  /* __6x6CLDEF_H */

