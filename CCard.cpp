/*----------------------------------------------------------------------

	6x6tcard.cpp -  Class implementations for the solitaire 
	              card game "six by six", class TCard
	
            Doug Rogers, 08/19/95

Classes:
--------
TCard

Changes:
--------
		08/19/95 15:06 - First implementation
		08/23/95 15:12 - Mods to handle bent stacks
2.15  09/12/95 06:30 - Broke this file off from climp.cpp
-----------------------------------------------------------------------*/

#include <stdlib.h>    //for rand() and srand()
#include <time.h>		//for srand(), random seed value
//#include <time.h>    //for randomize()
//#include <dos.h>		//for delay()

#include "6x6defs.h"
#include "6x6cldef.h"
#include "6x6fndef.h"

/******** Global Variables ***********/
/***************************************
Class: 	TCard
Purpose: Represents a card in the deck
****************************************/

/*=============== TCard::TCard() and ~TCard() ==========================
Purpose: Default constructor and destructors
======================================================================*/
TCard::TCard(void)
{
}

TCard::~TCard(void)
{
}

/*=============== TCard::Draw(int iRow, int iCol) ===========================================================
Purpose: Draw the full image of a card at the iRow and iCol indicated, 
			where the coordinates represent card positions on the table,
			not actual coordinates.  If the iRoq is 0 then the 
			card goes on the home piles, on top of the main layout.

Strategy:

============================================================================*/
void TCard::Draw(int iRow, int iCol)
{

}

/*=============== TCard::DrawTop(int iRow, int iCol) ===========================================================
Purpose: Just draw the top of the card because its mostly
         covered by an overlapping card.
Strategy:

============================================================================*/
void TCard::DrawTop(int iRow, int iCol)
{

}

/*=============== TCard::Erase(int iRow, int iCol) ===========================================================
Purpose: Remove the image of a whole card.

Strategy:

============================================================================*/
void TCard::Erase(int iRow, int iCol)
{

}


/*=============== void TCard::EraseTop(int iRow, int iCol) ===========================================================
Purpose: Remove the image of the top of a card

Strategy:

============================================================================*/
void TCard::EraseTop(int iRow, int iCol)
{

}



