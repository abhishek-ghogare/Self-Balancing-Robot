/* medfilt_new.c    Median filter in C*/

#include <stdio.h>
#include <limits.h>
#include "medfilter.h"
#define STOPPER INT_MIN /* Smaller than any datum */


void InitMedFilter (MedFilter *mf) {
	int i;

	mf->datpoint 	= mf->buffer;
	mf->small 		= (struct pair){NULL, STOPPER};
	mf->big 		= (struct pair){&mf->small, 0};

	for(i=0 ; i<NWIDTH ; ++i) {
		mf->buffer[i] = (struct pair){NULL, 0};
	}
}


int medfilter(MedFilter *mf, int inValue)
{
	unsigned int i;
	struct pair *successor   ;  /* pointer to successor of replaced data item */
	struct pair *scan        ;  /* pointer used to scan down the sorted list */
	struct pair *scanold     ;  /* previous value of scan */
	struct pair *median;     ;  /* pointer to median */

	if(inValue == STOPPER) inValue = STOPPER + 1; /* No stoppers allowed. */
	if( (++(mf->datpoint) - mf->buffer) >= NWIDTH)
		mf->datpoint=mf->buffer;  /* increment and wrap data in pointer.*/
	mf->datpoint->value=inValue        ;  /* Copy in new datum */
	successor=mf->datpoint->point    ;  /* save pointer to old value's successor */
	median = &(mf->big)                ;  /* median initially to first in chain */
	scanold = NULL               ;  /* scanold initially null. */
	scan = &(mf->big)                  ;  /* points to pointer to first (largest) datum in chain */
	/* Handle chain-out of first item in chain as special case */
	    if(scan->point == mf->datpoint ) scan->point = successor;
	    scanold = scan ;            /* Save this pointer and   */
	    scan = scan->point ;        /* step down chain */
	/* loop through the chain, normal loop exit via break. */
	for( i=0 ;i<NWIDTH ; i++ )
	{
	 /* Handle odd-numbered item in chain  */
	    if( scan->point == mf->datpoint ) scan->point = successor;  /* Chain out the old datum.*/
	    if( (scan->value < inValue) )        /* If datum is larger than scanned value,*/
	    {
	    	mf->datpoint->point = scanold->point;          /* chain it in here.  */
	        scanold->point = mf->datpoint;          /* mark it chained in. */
	        inValue = STOPPER;
	    };
	/* Step median pointer down chain after doing odd-numbered element */
	    median = median->point       ;       /* Step median pointer.  */
	    if ( scan == &(mf->small) ) break ;        /* Break at end of chain  */
	    scanold = scan ;          /* Save this pointer and   */
	    scan = scan->point ;            /* step down chain */
	/* Handle even-numbered item in chain.  */
	    if( scan->point == mf->datpoint ) scan->point = successor;
	    if( (scan->value < inValue) )
	    {
	    	mf->datpoint->point = scanold->point;
	        scanold->point = mf->datpoint;
	        inValue = STOPPER;
	    };
	    if ( scan == &(mf->small) ) break;
	    scanold = scan ;
	    scan = scan->point;
	};
	return( median->value );
}
