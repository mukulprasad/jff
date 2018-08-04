#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include "jff.h"

int main (int argc, char *argv[])
{
  daydata_t  dd;
  FILE      *fp, *tfp;
  int        count = 0;
  ldd_t     *pldd  = NULL; // for malloc n data assignment
  ldd_t     *hpldd = NULL; // this is fixed head pointer
  ldd_t     *tpldd = NULL; // this is fixed tail pointer
  ldd_t     *cpldd = NULL; // this is current pointer

//-----------------------

  DIR *dp;
  struct dirent *de;
  char filename[128];
  char slash[]="/";
  
  dp = opendir(argv[1]);
  if (dp == NULL) {
    printf ("Unable to open the directory %s\n", argv[1]);
    return 1;
  }

  while ((de = readdir(dp)) != NULL) {
    if (de->d_type == 0x4) {
      continue;
    }
    pldd  = NULL; 
    hpldd = NULL; 
    tpldd = NULL; 
    cpldd = NULL; 
    count = 0;

    strcpy(filename, argv[1]);

    int len = strlen(filename);
    if (filename[len-1] == '/') {
      filename[len-1] = '\0';
    }

    strcat(filename, slash);
    strcat(filename, de->d_name);
    printf ("Sorting file %s\n", filename);
//-----------------------
    fp = fopen(filename, "r+");
    if (fp == NULL) {
      printf("Error opening file %s\n", argv[1]);
      continue;
    }
    while (fread(&dd, sizeof(daydata_t), 1, fp)) {
 
      pldd = (ldd_t*) malloc(sizeof(ldd_t));
      pldd->dd = dd;
      pldd->next = NULL;
      pldd->prev = NULL;
 
      if (hpldd == NULL) {  // if this is the first element, set head and tail
        hpldd = tpldd = pldd;
      } else {                // else perform insertion
        insertinlist(&hpldd, &tpldd, pldd);
      }
      count += 1;
    }
    printf("Number of elements read %d\n", count);
 
    // List is sorted, put it back in file
    char *tempfname = (char *)malloc(256);
    strcpy(tempfname, filename);
    strcat(tempfname, ".tmp");
 
    tfp = fopen(tempfname, "w");
    if (tfp == NULL) {
      printf ("Error opening the temp file for writing sorted content\n");
      fclose(fp);
      continue;
    }
    
    cpldd = hpldd;
    while (cpldd != NULL) {
      printf("writing for date %d, ptr %p\n", cpldd->dd.date, cpldd);
      fwrite (&cpldd->dd, sizeof(daydata_t), 1, tfp);
      cpldd = cpldd->next;
    }
    fclose (tfp);
    fclose(fp);
    // delete the original file and rename the tempfile
    remove(filename);
    rename(tempfname, filename);
  }
  return 0;
}

void insertinlist(ldd_t **head, ldd_t **tail, ldd_t *pldd)
{
  ldd_t *cpldd;
  ldd_t *temp;

  ldd_t *hpldd; 
  ldd_t *tpldd; 
  
  hpldd = *head;
  tpldd = *tail;

  if (hpldd == tpldd) {                   // case of single element in list
    if (pldd->dd.date < hpldd->dd.date) {
      pldd->next = hpldd;
      pldd->prev = NULL;
      hpldd->prev = pldd;
      hpldd = pldd;
    } else {
      pldd->next = NULL;
      pldd->prev = tpldd;
      tpldd->next = pldd;
      tpldd = pldd;
    }
    *head = hpldd;
    *tail = tpldd;
    return;
  }
  cpldd = hpldd;
  while(pldd->dd.date > cpldd->dd.date){
    if (cpldd->next != NULL) {
      cpldd = cpldd->next;
    }else {     // we are at the tail so double check and append
      if (cpldd == tpldd) {
        tpldd->next = pldd;
        pldd->next = NULL;
        pldd->prev = tpldd;
        tpldd = pldd;
      } else {
        printf("We are in deep shit... \n");
      }

      *head = hpldd;
      *tail = tpldd;
      return;
    }
  }
  // at this point pldd date is less than cpldd date, so insert before cpldd
  // if cpldd is head treat special
  if (cpldd == hpldd){
    pldd->next = hpldd;
    pldd->prev = NULL;
    hpldd->prev = pldd;
    hpldd = pldd;
  } else {
    temp        = cpldd->prev;
    pldd->next  = cpldd;
    pldd->prev  = temp;
    cpldd->prev = pldd;
    temp->next  = pldd;
  }
  *head = hpldd;
  *tail = tpldd;
  return;  
}


