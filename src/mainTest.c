#include "GPXParser.h"
#include "GPXhelpers.h"

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

  // print the doc -- remember to free the temp string used aftewards to avoid memory leaks
  char * tempString;

  tempString = GPXdocToString(testgpxDoc);

  printf("%s\n", tempString);

  printf("get data:\n");

  printf("  GetNumWaypoints: %d\n", getNumWaypoints(testgpxDoc) );
  printf("  getNumRoutes: %d\n", getNumRoutes(testgpxDoc) );
  printf("  getNumTracks: %d\n", getNumTracks(testgpxDoc) );
  printf("  getNumSegments: %d\n", getNumSegments(testgpxDoc) );
  printf("  getNumGPXData: %d\n", getNumGPXData(testgpxDoc) );


  Waypoint * wpt = getWaypoint( testgpxDoc, "Raven Dance Peak 1647m" );
  char * wptString = waypointToString( wpt );
  printf(" Waypoint found: %s\n", wptString );

  wpt = getWaypoint( testgpxDoc, "Foo" );
  if( wpt == NULL ){
    printf(" NULL test passed\n");
  }

  Route * route = getRoute( testgpxDoc, "Some route");
  char * routeString = routeToString( route );
  printf(" Route found: %s\n", routeString );

  Track * track = getTrack( testgpxDoc, "Mount Steele Trail");
  char * trackString = trackToString( track );
  printf(" Track found: %s\n", trackString );

  track = getTrack( testgpxDoc, "Some other Trail" );
  if( track == NULL ){
    printf(" Second NULL test passed \n");
  }

  // test the compare functions
  free(tempString);
  deleteGPXdoc(testgpxDoc);
}