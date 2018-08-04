#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include "jff.h"

//TODO take directory as parameter, and process all the files in it 
//     starting with cm
int main (int argc, char* argv[])
{
#if 1	
  DIR *dp;
  struct dirent *de;
  char filename[128];
  char slash[]="/";
  int retcode;

  dp = opendir(argv[1]);
  if (dp == NULL) {
    printf ("Unable to open the directory %s \n", argv[1]);
    return 1;
  }

  while ((de = readdir(dp)) != NULL) {
    if (de->d_type == 0x8) {
      strcpy(filename, argv[1]);

      int len = strlen(filename);
      if (filename[len-1] == '/') {
        filename[len-1] = '\0';
      }

      strcat(filename, slash);
      strcat(filename, de->d_name);
      printf ("Processing file %s\n", filename);
      retcode = process_file (filename);
      process_retcode(retcode);
    }
  }
  return 0;
#else
	int i;
  int retcode;
	for (i = 1; i < argc; i++) {
    retcode = process_file (argv[i]);
    process_retcode(retcode);
	}
	printf("\n");
#endif
}

