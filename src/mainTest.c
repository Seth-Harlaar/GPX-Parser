
#include "../include/GPXParser.h"


int main( int argc, char * argv[] ){
  printf("This is the main file\n");
  
  // start by setting up the simple xml parser
  GPXdoc * testDoc = NULL;

  testDoc = createGPXdoc("simple.gpx");

  if( testDoc == NULL ){
    printf("Failed\n");
  } else {
    printf("Passed\n");
  }
}