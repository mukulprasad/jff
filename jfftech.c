#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include "jff.h"

int main (int argc, char *argv[])
{
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
    strcpy(filename, argv[1]);
    strcat(filename, slash);
    strcat(filename, de->d_name);
    printf ("Processing for technicals %s\n", filename);
    process_tech(filename);
  }
  return 0;
}

int process_tech(char *fname)
{
  daydata_t  dd;
  FILE      *fp;
  FILE      *tfp;
  int        count = 0;
  ldd_t     *pldd  = NULL; // for malloc n data assignment
  ldd_t     *hpldd = NULL; // this is fixed head pointer
  ldd_t     *tpldd = NULL; // this is fixed tail pointer


  fp = fopen(fname, "r+");
  if (fp == NULL) {
    return 1;
  }

  while (fread(&dd, sizeof(daydata_t), 1, fp)) {

    pldd = (ldd_t*) malloc(sizeof(ldd_t));
    pldd->dd = dd;

    //Put it in the list
    if (hpldd == NULL) {
      pldd->next = NULL;
      pldd->prev = NULL;
      hpldd      = tpldd = pldd;
    } else {
      tpldd->next = pldd;
      pldd->prev  = tpldd;
      pldd->next  = NULL;
      tpldd       = pldd;
    }
    count += 1;
  }
#if 0  
  /* test the link list */
  int tcnt = 0;
  pldd = hpldd;
  for (tcnt = 0; tcnt < count; tcnt++) {
    printf("Prev: %p, Next: %p\n", pldd->prev, pldd->next); 
    pldd = pldd->next;
  }
#endif

  process_bollinger(hpldd);
  //TODO Do the file writing job here and not in the process bollinger fn.
  char *tempfname = (char *)malloc(256);
  strcpy(tempfname, fname);
  strcat(tempfname, ".tmp");

  tfp = fopen(tempfname, "w");
  if (tfp == NULL) {
    printf ("Error opening the temp file for writing sorted content\n");
    fclose(fp);
    return 1;
  }
  pldd = hpldd;
  while (pldd != NULL) {
    fwrite (&pldd->dd, sizeof(daydata_t), 1, tfp);
    pldd = pldd->next;
  }
  fclose (tfp);
  fclose(fp);

  // delete the original file and rename tempfile
  remove(fname);
  rename(tempfname, fname);

  return 0;
}

//TODO make it take pointer to pointer for hpldd and tpldd, 
//     and stop writing into file in function
//
void process_bollinger (ldd_t *hpldd)
{
  ldd_t  *cpldd = hpldd;

  float  asma20[20];
  int    isma20 = 0;

  float  asma50[50];
  int    isma50 = 0;
  int    trig20 = 0;
  int    trig50 = 0;


  bzero((void *)asma20, 20 * sizeof(float));
  bzero((void *)asma50, 50 * sizeof(float));
  
  printf("Date, Open, High, Low, close, DMA20, Blow, Bhigh\n");
  while (cpldd) {
    asma20[isma20] = cpldd->dd.cl;
    isma20 += 1;
    if (isma20 >=20) isma20 = 0;
    if (isma20 == 19) trig20 = 1;

    if (trig20) {
      cpldd->dd.dma20   = calcdma (&asma20[0], 20);
      cpldd->dd.stdev20 = calcstdev (&asma20[0], cpldd->dd.dma20, 20);
      cpldd->dd.bblow  = cpldd->dd.dma20 - (2 * cpldd->dd.stdev20);
      cpldd->dd.bbhigh = cpldd->dd.dma20 + (2 * cpldd->dd.stdev20);
    }

    asma50[isma50] = cpldd->dd.cl;
    isma50 += 1;
    if (isma50 >=50) isma50 = 0;
    if (isma50 == 49) trig50 = 1;
    
    if (trig50) {
      cpldd->dd.dma50 = calcdma(&asma50[0], 50);
    }

    printf("%d, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f",
      cpldd->dd.date, cpldd->dd.op, cpldd->dd.hi, cpldd->dd.lo, cpldd->dd.cl,
      cpldd->dd.dma20, cpldd->dd.bblow, cpldd->dd.bbhigh);

    if (trig20) {
      if (cpldd->dd.cl < cpldd->dd.bblow) {
        cpldd->dd.flag.ltbbl = 1;
        printf(", bblow crossed\n");
      }
      else if (cpldd->dd.cl > cpldd->dd.bbhigh) {
        cpldd->dd.flag.gtbbh = 1;
        printf(", bbhigh crossed\n");
      }
      else {
        printf("\n");
      }
    } else {
      printf("\n");
    }

    cpldd = cpldd->next;
  }
  return;
}

float calcdma(void *vfloat, int count) 
{
  int i;
  float sum = 0;
  float *pfloat = (float*) vfloat;

  for (i = 0; i < count; i++) {
    sum += pfloat[i];
  }
  sum = sum/count;
  return sum;
}

/* calculate standard deviation
 * stdev = sqrt (( sum (val-mean)^2) / (count -1))
 *            /---------------------
 *           /  SUM (x-mean)^2  
 *  stdev = /   --------------
 *         V     count -1
 */

float calcstdev (void *vfloat, float dma, int count)
{
  int i;
  float summeansqr = 0;
  float *pfloat = (float *)vfloat;
  
  for (i = 0; i < count; i++) {
    summeansqr += (pfloat[i] - dma) * (pfloat[i] - dma);
  }

  summeansqr = summeansqr/count;
  return (sqrt(summeansqr));
}  
  
