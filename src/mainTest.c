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
  GPXdoc * testgpxDoc2 = NULL;

  // get gpxdoc
  testgpxDoc = createGPXdoc(argv[1]);
  testgpxDoc2 = createGPXdoc(argv[1]);

  // print the doc -- remember to free the temp string used aftewards to avoid memory leaks
  char * tempString;
  char * tempString2;

  tempString = GPXdocToString(testgpxDoc);
  tempString2 = GPXdocToString(testgpxDoc2);

  printf("%s\n", tempString);
  printf("gpx doc 2\n");
  printf("%s\n", tempString2);

  // test the compare functions
  int result;
  int result2;
  int result3;

  printf("Comparing some stuff\n");
  result = compareWaypointList( testgpxDoc->waypoints, testgpxDoc2->waypoints );
  

  Track * firstTrack = getFromFront( testgpxDoc->tracks );
  Track * secondTrack = getFromFront( testgpxDoc2->tracks );

  result2 = compareTracks( firstTrack, secondTrack );

  Route * firstRoute = getFromFront( testgpxDoc->routes );
  Route * secondRoute = getFromFront( testgpxDoc2->routes );

  result3 = compareRoutes( firstRoute, secondRoute );

  printf("Compare Results:\n Waypoints: %d\n Tracks: %d\n Routes: %d\n", result, result2, result3 );

  free(tempString);
  deleteGPXdoc(testgpxDoc);
}