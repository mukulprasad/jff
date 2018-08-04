
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include "jff.h"

//TODO take directory as parameter, and process all the files in it 
//     starting with cm
int main (int argc, char* argv[])
{
  DIR *dp;
  FILE       *fpb;
  FILE       *fpt;
  struct dirent *de;
  char   filename[128];
  char   slash[]="/";

  dp = opendir(argv[1]);
  if (dp == NULL) {
    printf ("Unable to open the directory %s \n", argv[1]);
    return 1;
  }
  /* open files for writing report
   */
  fpb = fopen("bulk.op", "a+");
  if (fpb == NULL) {
    printf("Error opening file for report");
    return 1;
  }
  
  fpt = fopen("today.op", "a+");
  if (fpb == NULL) {
    printf("Error opening file for report");
    return 1;
  }

  while ((de = readdir(dp)) != NULL) {
    if (de->d_type == 0x8) {
      strcpy(filename, argv[1]);
      strcat(filename, slash);
      strcat(filename, de->d_name);
      
      printf ("reporting for file %s\n", filename);
      report(filename, fpb, fpt);
    }
  }
  fclose(fpb);
  fclose(fpt);

}

int report (char *filename, FILE *fpb, FILE* fpt)
{
  FILE       *fp;

  daydata_t   dd;

  fp = fopen(filename, "r+");
  if (fp == NULL) {
    printf("Error opening file %s\n", filename);
    return 1;
  }

  while (fread(&dd, sizeof(daydata_t), 1, fp)) {
    if (dd.flag.ltbbl) {
      fprintf(fpb, "BBL, %d, %s, %.2f,%.2f\n", dd.date, dd.sym, dd.cl, dd.bblow);
    } 
    if (dd.flag.gtbbh) {
      fprintf(fpb, "BBH, %d, %s, %.2f,%.2f\n", dd.date, dd.sym, dd.cl, dd.bbhigh);
    } 
    
  }

  if (dd.flag.ltbbl) {
    fprintf(fpt, "BBL, %d, %s, %.2f,%.2f\n", dd.date, dd.sym, dd.cl, dd.bblow);
  }
  if (dd.flag.gtbbh) {
    fprintf(fpt, "BBH, %d, %s, %.2f,%.2f\n", dd.date, dd.sym, dd.cl, dd.bbhigh);
  }
  fclose(fp);
  return 0;
}

