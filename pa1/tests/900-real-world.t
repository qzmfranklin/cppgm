/*This program prints a calendar for a year specified. The user enters a
     year for the calendar and the programs automatically prints the calendar
     in text format.
     The codes are: day_code (0 = Sun, 1 = Mon, etc.)
                    leap_year (0 = no leap year, 1 = leap year)   */
#include <stdio.h>
#include <stdlib.h>       
#define TRUE 1
#define FALSE 0
int get_day_code (int year);
int get_leap_year (int year);
void print_calendar (FILE *fout, int year, int day_code, int leap_year);
int get_year (void);
main()
{
   
   int year, day_code, leap_year; 
   
   FILE *fout;
   
   fout = fopen ("calendar.txt", "w");
   
   year = get_year();                           
   
   day_code = get_day_code (year);
   
   leap_year = get_leap_year (year);
   
   print_calendar(fout, year, day_code, leap_year);
   
   printf("Open up \'calendar.txt\' to see your calendar...\n");
   
   system("pause");
     
}
     
int get_year (void)
{
int year;
printf ("Enter a year: ");
scanf ("%d", &year);
return year;
}             
int get_day_code (int year)
{
int day_code;
int x1, x2, x3;
	x1 = (year - 1.)/ 4.0;
	x2 = (year - 1.)/ 100.;
	x3 = (year - 1.)/ 400.;
day_code = (year + x1 - x2 + x3) %7;
return day_code;
}             
int get_leap_year (int year)
{
	
//if((year% 4) == 0 );
if(year% 4==0 && year%100 != 0 || year%400==0)
   return TRUE;
   else return FALSE;	
		
}
void print_calendar (FILE *fout, int year, int day_code, int leap_year) //function header
{
	int  days_in_month,     /* number of days in month currently 
                                                     being printed */
         day,       /* counter for day of month */
         month;     /* month = 1 is Jan, month = 2 is Feb, etc. */
     fprintf (fout,"                   %d", year);
     for ( month = 1; month <= 12; month++ ) {
          switch ( month ) { /* print name and set days_in_month */
          case 1:
               fprintf(fout,"\n\nJanuary" );
               days_in_month = 31;
               break;
          case 2:
               fprintf(fout,"\n\nFebruary" );
               days_in_month = leap_year ? 29 : 28;
               break;
          case 3:
               fprintf(fout, "\n\nMarch" );
               days_in_month = 31;
               break;
          case 4:
               fprintf(fout,"\n\nApril" );
               days_in_month = 30;
               break;
          case 5:
               fprintf(fout,"\n\nMay" );
               days_in_month = 31;
               break;
          case 6:
               fprintf(fout,"\n\nJune" );
               days_in_month = 30;
               break;
          case 7:
               fprintf(fout,"\n\nJuly" );
               days_in_month = 31;
               break;
          case 8:
               fprintf(fout,"\n\nAugust" );
               days_in_month = 31;
               break;
          case 9:
               fprintf(fout,"\n\nSeptember" );
               days_in_month = 30;
               break;
          case 10:
               fprintf(fout,"\n\nOctober" );
               days_in_month = 31;
               break;
          case 11:
               fprintf(fout,"\n\nNovember" );
               days_in_month = 30;
               break;
          case 12:
               fprintf(fout,"\n\nDecember" );
               days_in_month = 31;
               break;
          }
          fprintf(fout,"\n\nSun  Mon  Tue  Wed  Thu  Fri  Sat\n" );
          /* advance printer to correct position for first date */
          for ( day = 1; day <= 1 + day_code * 5; day++ )
               fprintf(fout," " );
          /* print the dates for one month */
          for ( day = 1; day <= days_in_month; day++ ) {
               fprintf(fout,"%2d", day );
               if ( ( day + day_code ) % 7 > 0 ) /* before Sat? */
                    /* move to next day in same week */
                    fprintf(fout,"   " );
               else  /* skip to next line to start with Sun */
                    fprintf(fout, "\n " );
          }
          /* set day_code for next month to begin */
          day_code = ( day_code + days_in_month ) % 7;
     }
}
