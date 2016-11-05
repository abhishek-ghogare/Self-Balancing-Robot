/*
*
* Team Id           : self_balancing_bot
* Author List       : Abhishek Ghogare
* Filename          : medfilter.h
* Functions         : void  MF_init_med_filter (MedFilter *mf),
*                     int   MF_med_filter(MedFilter *mf, int inValue)
* Global Variables  : None
*
*/

#ifndef MEDFILTER_H_
#define MEDFILTER_H_

#define NWIDTH 10 /* Size of the data buffer; length of the sequence. */

struct MF_pair
{
    struct MF_pair   *point;    /* Pointers forming list linked in sorted order */
    int  value;                 /* Values to sort */
};


typedef struct MedFilter {
    //int NWIDTH;                   /* Size of the data buffer; length of the sequence. */
    struct MF_pair buffer[NWIDTH];  /* Buffer of nwidth pairs */
    struct MF_pair *datpoint;       /* pointer into circular buffer of data */
    struct MF_pair small ;          /* chain stopper. */
    struct MF_pair big;             /* pointer to head (largest) of linked list.*/
} MedFilter;


void MF_init_med_filter (MedFilter *mf);    // Initialize median filter

// Takes in value, returns median of last NWIDTH in values
int MF_med_filter(MedFilter *mf, int inValue);



#endif /* MEDFILTER_H_ */




