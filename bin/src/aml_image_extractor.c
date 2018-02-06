//From https://www.cnx-software.com/2016/11/19/how-to-create-a-bootable-recovery-sd-card-for-amlogic-tv-boxes/

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
 
uint32_t convert(uint8_t *test, uint64_t loc) {
  return ntohl((test[loc] << 24) | (test[loc+1] << 16) | (test[loc+2] << 8) | test[loc+3]);
}
 
void main (int argc, char **argv) {
  FILE *fileptr;
  uint8_t *buffer;
  long filelen;
 
  FILE *f;
  char *filename;
  uint64_t record;
  uint64_t record_loc;
  uint64_t file_loc;
  uint64_t file_size;
 
  if (argc <= 1) {
    printf("Usage: %s [firmware-file-name]\n", argv[0]);
    exit (0);
  }
 
  fileptr = fopen(argv[1], "rb");
  fseek(fileptr, 0, SEEK_END);
  filelen = ftell(fileptr);
  rewind(fileptr);
 
  buffer = (uint8_t *)malloc((filelen+1)*sizeof(uint8_t));
  fread(buffer, filelen, 1, fileptr);
  fclose(fileptr);
 
  for (record = 0; record < (uint8_t)buffer[0x18]; record = record + 1){
    record_loc = 0x40 + (record * 0x240);
 
    filename = (malloc(32));
    sprintf(filename,"tmp/%s.%s",(char *)&buffer[record_loc+0x120], (char *)&buffer[record_loc+0x20]);

    printf("    Extracting %s\n", filename);
 
    file_loc = convert(buffer,record_loc+0x10);
    file_size = convert(buffer,record_loc+0x18);
 
    f = fopen(filename, "wb");
    if (f == NULL) {
     printf("ERROR: could not open output\n");
     printf("the error was: %s\n",strerror(errno));
     free(filename);
     continue;
    }
    fwrite(&(buffer[file_loc]), sizeof(uint8_t), (size_t)file_size, f);
    fclose(f);
    free(filename);
  }
  free(buffer);
}
