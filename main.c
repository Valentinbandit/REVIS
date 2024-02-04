#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define MAP_SIZE 256
char *generate_output_path(char* input_path){
  int n = 0;
  while(input_path[n] != '.' && input_path[n] != '\0'){
    n++;
  }
  char *output_path = (char*)malloc(n+1+4);
  strncpy(output_path, input_path, n);
  strcat(output_path, ".png");
  return output_path;
}
int main(int argc, char *argv[]){
   char* program = *argv++;
  if(*argv == NULL){
    fprintf(stderr, "Usage: %s <input file>", program);
    fprintf(stderr, "ERROR: No input file was given");
    exit(1);
  }
  char* file_path = *argv++;
  FILE* input_file = fopen(file_path, "rb");
  if(input_file == NULL){
    fprintf(stderr, "ERROR: Opening %s file was not succesful: %s\n", file_path, strerror(errno));
    exit(1);
  } 
  
  uint8_t buffer0;
  fread(&buffer0, sizeof(buffer0), 1, input_file);
  uint8_t buffer1;
  uint32_t map[MAP_SIZE][MAP_SIZE] = {0};
  uint32_t pixels[MAP_SIZE][MAP_SIZE];
  while (fread(&buffer1, sizeof(buffer1), 1, input_file)){
    map[buffer0][buffer1]++;
    buffer0 = buffer1;
  }
  float max = 0;
  for (int x =0; x < MAP_SIZE; x++) {
    for (int y = 0; y < MAP_SIZE; y++) {
      float t = (map[x][y] > 0)? logf(map[x][y]) : 0.0f;
      if(t > max) max = t;
    }
  }
  for (int x =0; x < MAP_SIZE; x++) {
    for (int y = 0; y < MAP_SIZE; y++) {
      uint32_t t = (logf(map[x][y])/max)*255;
      pixels[x][y] = 0xFF000000 | t | (t<<8) | (t<<16); //RGBA a little-endian miatt ABGR lesz 
    }
  }
  char *output_path =generate_output_path(file_path);
  stbi_write_png(output_path, MAP_SIZE, MAP_SIZE, 4, pixels, MAP_SIZE*sizeof(uint32_t));
  fclose(input_file);
  free(output_path);
} 
