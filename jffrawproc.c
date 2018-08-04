#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "jff.h"

// Function to read the ascii file as received from nif and write into 
// separate co specific files in structured manner. 
// @ input: filename 
//          file content should be in the following form:
//          SYMBOL //          SERIES //          OPEN //          HIGH
//          LOW //             CLOSE //           LAST //          PREVCL
//          TRDQNT //          TRDVAL //          DATE
//


int process_file(char *filename)
{
  FILE      *fp;
  char       line[256];

  fp = fopen(filename, "r");
  if (fp == NULL) {
    return 1;
  }

  // read file one line at a time.
  while ( fgets (line, sizeof line, fp) != NULL) {
    if (line[0] == 'S' && line[1] == 'Y' && line[2] == 'M' && line[3] == 'B' &&
        line[4] == 'O' && line[5] == 'L') {
      printf ("This is heading line\n");
      printf("Data read: %s\n", line);
    } else {
      // extract data from line read from file
      extract_data (line);
    }
  }
  
  fclose (fp);
  
  return 0;
}
  
int  extract_data (char *line)
{
  char       token[64];
  char      *tptr;
  int        tokcntr = 0;
  daydata_t  dd;
  
  tptr = line;
 
  bzero ((void *)&dd, sizeof(daydata_t));

  while (tptr !=NULL) {
    bzero(token, 64); 
    tptr = tokenize(tptr, token, ',');

    switch (tokcntr) {
      case 0: strcpy (dd.sym, token); break;
      case 1: strcpy (dd.ser, token); break;
      case 2: dd.op = strtof (token, NULL); break;
      case 3: dd.hi = strtof (token, NULL); break;
      case 4: dd.lo = strtof (token, NULL); break;
      case 5: dd.cl = strtof (token, NULL); break;
      case 6: dd.last = strtof (token, NULL); break;
      case 7: dd.prevcl = strtof (token, NULL); break;
      case 8: dd.ttqnt = strtod (token, NULL); break;
      case 9: dd.ttval = strtod (token, NULL); break;
      case 10: dd.date = strtodate (token); break;
      case 11: dd.ttrd = atoi (token); break;
      default: break;
    }
    tokcntr += 1;
  }
  // check if series is EQ then only update in db
  if (dd.ser[0] == 'E' && dd.ser[1] == 'Q' && dd.ser[2] =='\0') {
    update_db (&dd);
    printf ("%d, %-10s, %08.2f, %08.2f, %08.2f, %08.2f, %08.2f\n", 
        dd.date, dd.sym, dd.op, dd.hi, dd.lo, dd.cl, dd.last);
  }
  return 0;
}

// Convert the string into decimal such that
// 23september2018 in decimal is 20180923
//

int strtodate (char *datestring)
{
  char  day[16];
  char  mon[16];
  char  yrr[16];
  int   retval = 0;

  strcpy(day, datestring);
  strcpy(mon, &datestring[3]);
  strcpy(yrr, &datestring[7]);
  day[2] = '\0';
  mon[3] = '\0';

  retval = atoi(yrr);
  retval = retval*10000;
  
  // ja jun jul  f mar may  ap au s o n d 

  switch (mon[0]) {
    case 'F': retval += 200; break;
    case 'S': retval += 900; break;
    case 'O': retval += 1000; break;
    case 'N': retval += 1100; break;
    case 'D': retval += 1200; break;
    case 'A': 
        switch (mon[1]) {
          case 'P': retval += 400; break;
          case 'U': retval += 800; break;
        }
        break;
    case 'J':
        switch (mon[1]) {
          case 'A': retval += 100; break;
          case 'U': 
                    if (mon[2] == 'N') 
                      retval += 600; 
                    else
                      retval += 700;
                    break;
        }
        break;
    case 'M':
        switch (mon[2]) {
          case 'R': retval += 300; break;
          case 'Y': retval += 500; break;
        }
        break;
    default: break;
  }
  retval += atoi(day);
  return retval;
}

int update_db (daydata_t *pdd)
{
  FILE *lfp;
  char filename[128] = "./dat/";
  
  strcat(filename, pdd->sym);

  lfp = fopen(filename, "a");
  if (lfp == NULL) {
    printf("Unable to open file %s for writing\n", pdd->sym);
    return 1;
  }
  
  fwrite (pdd, sizeof(daydata_t), 1, lfp);
  fclose(lfp);
  return 0;
}

char *tokenize(char *string, char *token, char separator)
{
  //char separator = ',';
  int i = 0;

  while (string[i] != separator &&
         string[i] != '\0') {
    token[i] = string[i];
    i += 1;
  }
  
  token[i] = '\0';

  if (string[i] == '\0' || string[i] == '\n') {
    return (NULL);
  } else {
    return &string[i+1];
  }
}

void process_retcode(int retcode)
{
  if (retcode) {
    printf ("Fault %d in system... exitting\n", retcode);
    exit(1);
  }
  return;
} 


