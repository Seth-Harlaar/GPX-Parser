#include "../include/GPXParser.h"
#include "../include/GPXhelpers.h"

int main( int argc, char * argv[] ){

  if( argc != 2){
    printf("usage: ./mainTest <fileName>\n");
    exit(0);
  }

  printf("This is the main file\n");
  
  // start by setting up the simple xml parser
  GPXdoc * testgpxDoc = NULL;

  // get gpxdoc
  testgpxDoc = createGPXdoc(argv[1]);
  printf("Doc version: %0.2f\n", testgpxDoc->version);

  // print the doc -- remember to free the temp string used aftewards to avoid memory leaks
  char * tempString;

  tempString = GPXdocToString(testgpxDoc);

  printf("%s\n", tempString);

  free(tempString);
  deleteGPXdoc(testgpxDoc);
}