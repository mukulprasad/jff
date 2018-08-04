#include <string.h>

typedef struct {
  unsigned int ltbbl:1;
  unsigned int gtbbh:1;

}flags_t;

//TODO add date field, 20dma, 50dma, bblow, bbhigh, 
typedef struct {
  flags_t flag;
  char    sym[16];
  char    ser[16];
  float   op;
  float   hi;
  float   lo;
  float   cl;
  float   last;
  float   prevcl;
  double  ttqnt;
  double  ttval;
  int     date;
  int     ttrd;
  float   dma20;
  float   dma50;
  float   stdev20;
  float   bblow;
  float   bbhigh;
} daydata_t;

typedef struct ldd {
  daydata_t    dd;
  struct ldd  *next;
  struct ldd  *prev;
} ldd_t;

// Function prototypes
int   process_file (char *);
void  process_retcode(int );
char *tokenize(char *, char *, char);
int   extract_data (char *);
int   update_db (daydata_t *);
int   strtodate(char *);
void  process_bollinger(ldd_t* );
void  insertinlist(ldd_t **, ldd_t **, ldd_t *);
int   process_tech(char *fname);
float calcdma(void *vfloat, int count); 
float calcstdev (void *vfloat, float dma, int count);
int report (char *filename, FILE *fpb, FILE* fpt);
