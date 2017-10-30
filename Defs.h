/*----------------------------------------------------------------------------
	6x6defs.h -  miscellaneous definitions of constants for
					 the six by six program

                   Doug Rogers, 08/20/95

Changes:
--------
08/20/95 09:07 - first created
08/23/95 15:12 - Mods to handle bent stacks

---------------------------------------------------------------------------*/
#ifndef __6x6DEFS_H     /* prevent multiple includes */
#define __6x6DEFS_H

#define	PROBLEM		-100  //Return value from a function indicating there was a problem
#define	NOPROBLEM	100	//Return value indicating there was no problem
#define OK 100			//same as NOPROBLEM

#define BITMAP_RSRC_OFFSET  120  //Bitmaps are: 
								 //  small HSDC, med HSDC (120-127)

#define ID_USE_LETTERS	170    //Menu id's for symbol selection
#define ID_USE_SYMBOLS	171

#define SMALL_BM	0	//Subscripts for bitmap specification
#define MED_BM		1

#define	INIT_X_POS		100		//initial x position
#define	INIT_Y_POS		20		//initial y position
#define	INIT_X_SIZE		380		//initial x size
#define	INIT_Y_SIZE		450		//initial y size

#define	MAXROWS	41			//Maximum number of rows in the table array
#define	MAXCOLS	6			//Maximum number of columns in the table array

#define VTEXTSPACING 17		//Vertical text spacing for mult lines

#define CORNERELLIPSE	6		//Small ellipse in corner of rectangles
#define	CARDHEIGHT		80		//Height of a full card image
#define	CARDWIDTH		50		//Width of a full card image
#define	HOMEPILESPACE	15		//Distance between home piles at top and 
										// the main layout of cards
#define	COLSPACE			10		//Space between columns
#define	TABLEMARGIN		10		//Space around edge of layout
#define	VERTFRACSHOWING		0.25	//Fraction of card which shows when card
										// is overlapped by another         
#define	HORFRACSHOWING		0.63	//Fraction of card which shows when card
							           //overlaps horizontally			
#define XVALUEOFFSET	2		//Offset from corner of card
#define YVALUEOFFSET	3		//  where value should be drawn										
#define SBMHOFFSET		20		// and to where suit bitmap is drawn
#define SBMVOFFSET		5		
#define CARDBM_HOFFSET	5       //Offset for big picture of card
#define CARDBM_VOFFSET	20      //

#define NUMBEROFZONES	12  //Deck + Space + 4 Home Piles + Six Cols
#define COLZONEOFFSET	6	//There are several "Zones" where the mouse
							//can hit; this is how many Zones there are
							//before the mouse is hitting columns
							//(Deck + four home piles)
#define NOTINAZONE -1		//Value returned if click is not in a zone

#define PILENOTKNOWN	0   //When adding to a pile, has the pile been
#define PILEKNOWN		1	//  clicked on, or was a double click used

#define CARD_DEALT_MOVE		900
#define LAYOUT_MOVE        901
#define WINDOW_RESIZE_MOVE 902
#define HOME_PILE_MOVE		950
#define REDO_ALL				910
#define UPDATE_SCORES_ONLY	911

#endif  /* __6x6DEFS_H */

