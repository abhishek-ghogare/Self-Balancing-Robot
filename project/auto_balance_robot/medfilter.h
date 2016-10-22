/*
 * medfilter.h
 *
 *  Created on: Oct 18, 2016
 *      Author: Thor
 */

#ifndef MEDFILTER_H_
#define MEDFILTER_H_

#define NWIDTH 10 /* Size of the data buffer; length of the sequence. */

struct pair
{
	struct pair   *point;  /* Pointers forming list linked in sorted order */
	int  value;  /* Values to sort */
};


typedef struct MedFilter {
	//int NWIDTH; 	/* Size of the data buffer; length of the sequence. */
    struct pair buffer[NWIDTH];  /* Buffer of nwidth pairs */
    struct pair *datpoint;  /* pointer into circular buffer of data */
    struct pair small ;  /* chain stopper. */
    struct pair big;  /* pointer to head (largest) of linked list.*/
} MedFilter;

int medfilter(MedFilter *mf, int inValue);
void InitMedFilter (MedFilter *mf);



#endif /* MEDFILTER_H_ */




