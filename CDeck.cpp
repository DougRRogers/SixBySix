/*----------------------------------------------------------------------

	6x6tdeck.cpp -  Class implementations for the solitaire 
	              card game "six by six"
	
            Doug Rogers, 08/19/95

Classes:
--------
TDeck

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




/****************************************
Class: 	TDeck
Purpose: Represents a deck of cards
*****************************************/

/*=============== TDeck::TDeck() and ~TDeck() ==========================
Purpose: Default constructor and destructors
======================================================================*/
TDeck::TDeck(void)
{
}

TDeck::~TDeck(void)
{
}


/*=============== void TDeck::Shuffle(void) ===========================================================
Purpose: Shuffle the deck, make the first card the CurrentCard, and
			set CardsRemaining and CurrentCardIndex appropriately.

Strategy:

============================================================================*/
void TDeck::Shuffle(void)
{
	int	iSeries, iSuit, iCardInDeck;
	
	srand( (unsigned)time( NULL ) );


	for(iCardInDeck=0; iCardInDeck<52; iCardInDeck++)
		Card[iCardInDeck]=0;
	                              
	for(iSuit=100; iSuit<500; iSuit+=100)
		{
		for(iSeries=1; iSeries<14; iSeries++)
			{
			iCardInDeck=rand()%52;
			while(Card[iCardInDeck]>0)
				{
				iCardInDeck++;
				if(iCardInDeck>51)iCardInDeck=0;
				}
			Card[iCardInDeck]=iSuit+iSeries;
			}
		}
		
	iCurrentCardIndex=0;
	iCurrentCard=Card[iCurrentCardIndex];
	iCardsRemaining=52;		//This includes the current card, since
									// it hasn't been "removed" (CardIndex
									// pointing to next card) yet
	
}

/*=============== TDeck::Deal(TTable & Table, BOOL SlowDeal) ==============
Purpose: Deal the deck in the opening six by six manner by filling
         in the table array with the newly shuffled deck.
Strategy:

=======================================================================*/
void TDeck::Deal(TTable & Table, HWND hWnd)
{

	Table.Clear();

	for(int iRow=1; iRow<7; iRow++)	//Row 0 is reserved for the undealt
												//deck and the home piles, so start
												//the array at Row 1
		for(int iCol=0; iCol<6; iCol++)
			Table.AddCard(hWnd, NextCard(hWnd), iCol);


}


/*=============== TDeck::NextCard(HWND hWnd) ===================================
Purpose: Advance to the next card in the deck, incrementing the
			current card and decrementing CardsRemaining, and return
			the value of the new current card.
Strategy:

=======================================================================*/
int TDeck::NextCard(HWND hWnd)
{

	if(iCardsRemaining==0) return PROBLEM;  //If there is only one card
														// left (the current card),
														// then there is no next card
	
	iCurrentCard=Card[iCurrentCardIndex];	//Index starts at 0
	iCurrentCardIndex++;
	iCardsRemaining--;								//Counting the current one
	                                  
	ShowCardsRemaining(hWnd);
	ShowClickCount(hWnd);    
                                  
	return iCurrentCard;
}


/*=============== TDeck::CardsRemaining(void) ===================================
Purpose: Return the number of cards remaining in the deck.
Strategy:

=======================================================================*/
int TDeck::GetCardsRemaining(void)
{
	
	return iCardsRemaining;
}

