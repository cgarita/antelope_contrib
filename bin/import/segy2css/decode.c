/* $Name $Revision$ $Date$  */
/********************************************************************
 *
 *
 *    sgycss/decode.c
 *
 *    Program decode serial instrument code to the  station name.
 *    The station names can be  determined in .segy2css file 
 *    or in any ASCII file which will be specified in
 *    the command line(see manual for details). Files should be in a format :
 *
 *    INS_SER_NUM  START_TIME  END_TIME  STA_NAME
 *    
 *    START_TIME and END_TIME should be in a format: YYYYDDD:HH:MM:SS.mSS.
 *    If for current serial instrument code station name was not 
 *    determined, then make station name in form : 'snum', where
 *    snum - is serial instrument code.
 *
 *********************************************************************/
#include "segcss.h"

char *decode(num, dates, name, param, parnum)
 int num;
 struct data *dates;
 char name[12];
 struct conver *param;
 int parnum;


{
   struct conver tmp;
   double stime;
   double etime;
   double crnt_time;
   int get, j, i;
   long date;

   get = FALSE;

   date = dates->yr * 1000 + dates->day;
 
/* Get start time for currend data file  */

   crnt_time = dtoepoch( date ) +
               3600.0*dates->hour + 
               60.0*dates->min + 
               (double) dates->sec + (double)dates->msec/100.0;

/* Check first do we have instrumen number conversion in file ".segy2css"  */

   if(parnum > 0)  {
      for(i = 0; i < parnum; i++)  {
          tmp = param[i];
          stime = str2epoch(tmp.stime);
          etime = str2epoch(tmp.etime);
          if(num == tmp.sernum)  
             if( (crnt_time >= stime) && (crnt_time <= etime) ) {
                    strcpy(name, tmp.sname);
                    get = TRUE;
                    break;
             } 
                
      }

   }  else get = FALSE;


/* The are not station name for current serial instrument code. 
   Make name as "num" - serial instrument code  */
    
   if(!get)  
     sprintf(name,"%d\0",num);
   
}

/* $Id$ */
