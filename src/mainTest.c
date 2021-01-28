
#include "../include/GPXParser.h"

int main( int argc, char * argv[] ){

  if( argc != 2){
    printf("usage: ./mainTest <fileName>\n");
    exit(0);
  }

  printf("This is the main file\n");
  
  // start by setting up the simple xml parser
  xmlDoc * testDoc = NULL;
  GPXdoc * testgpxDoc = NULL;

  testDoc = xmlReadFile(argv[1], NULL, 0);

  if( testDoc == NULL ){
    printf("Failed\n");
  } else {
    printf("Passed\n");
  }

  printf("Doc version straigt from file: %s\n", testDoc->version);

  xmlFreeDoc(testDoc);

  // get gpxdoc
  testgpxDoc = createGPXdoc(argv[1]);

  printf("Doc version from parser: %.1f \n", testgpxDoc->version);

  // print the doc -- remember to free the temp string used aftewards to avoid memory leaks
  char * tempString;

  tempString = GPXdocToString(testgpxDoc);

  printf("%s\n", tempString);

  free(tempString);

  deleteGPXdoc(testgpxDoc);

}