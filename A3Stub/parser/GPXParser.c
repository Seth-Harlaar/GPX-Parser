// Seth Harlaar -- 1109524

#include "GPXParser.h"
#include "GPXhelpers.h"
#include <stdlib.h>


// * * * * * * * * * * * * * * * * * * * * 
// ********  GPXDoc Functions  ***********
// * * * * * * * * * * * * * * * * * * * *

// make the gpx doc
GPXdoc * createGPXdoc( char* fileName ){

  if( fileName == NULL ){
    return NULL;
  }

  // vars 
  GPXdoc * returnDoc = malloc( sizeof(GPXdoc) );
  xmlDoc * doc = NULL;
  xmlNode * headNode = NULL;
  xmlAttr * iterPoint = NULL;

  // get doc and check if null
  doc = xmlReadFile(fileName, NULL, 0);

  if( doc == NULL ){
    xmlCleanupParser();
    free( returnDoc );
    return NULL;
  }

  headNode = xmlDocGetRootElement(doc);

  // get namespace -- must be initialized
  xmlNs * nameSpace = headNode->ns;
  strcpy( returnDoc->namespace, (char *) nameSpace->href );

  // this loop was taken from libxmlexample.c
  for( iterPoint = headNode->properties; iterPoint != NULL; iterPoint = iterPoint->next ){
    xmlNode * value = iterPoint->children;
    
    char * attrName = (char *) iterPoint->name;
    char * cont  = (char *)(value->content);

    // get creator 
    if( strcmp(attrName, "creator") == 0 ){
      returnDoc->creator = malloc( sizeof(char) * strlen(cont) + 1 );
      strcpy( returnDoc->creator, cont );
    } else if( strcmp( attrName, "version" ) == 0 ){
      returnDoc->version = strtod( cont, NULL );
    }
  }


  // list of waypoints -- cannot be null, may be empty
  List * wayPoints = NULL;
  wayPoints = getWaypointsList( headNode, 0 );
  returnDoc->waypoints = wayPoints;

  // list of routes -- cannot be null, may be empty
  List * routes = NULL;
  routes = getRoutesList( headNode );
  returnDoc->routes = routes;

  // list of tracks -- cannot be null, may be empty
  List * tracks = NULL;
  tracks = getTracksList( headNode );
  returnDoc->tracks = tracks;

  xmlFreeDoc(doc);

  return returnDoc;
}

// get rid of the gpx doc
void deleteGPXdoc(GPXdoc* doc){
  if( doc != NULL ){
    // destroy all the data in there
    // free the creator value because its malloced
    free( doc->creator );
    // free the different lists
    freeList( doc->waypoints );
    freeList( doc->routes );
    freeList( doc->tracks );

    // free the struct itself
    free(doc);
    xmlCleanupParser();
  }
}

// turn the gpx doc into a string
// the code for this function was heavily inspired by the structListDemo.c file
char* GPXdocToString(GPXdoc* doc){
  if( doc == NULL ){
    return NULL;
  }
  char * tempString;
  char * tempWptString;
  char * tempRouteString;
  char * tempTrackString;

  // length will be very big
  int length = 0;

  // for the gpxDoc data
  length += (256 + 8 + 1);
  // add length for creator


  if( getLength( doc->waypoints ) != 0 ){
    tempWptString = toString( doc->waypoints );
  } else {
    tempWptString = malloc( sizeof(char) * 50 );
    strcpy( tempWptString, "There are no waypoints in this file\n" );
  }
  
  length += strlen( tempWptString );

  if( getLength( doc->routes) != 0 ){
    tempRouteString = toString( doc->routes );
  } else {
    tempRouteString = malloc( sizeof(char) * 50 );
    strcpy( tempRouteString, "There are no routes in this file.\n");
  }
  length += strlen( tempRouteString );

  if( getLength( doc->tracks) != 0 ){
    tempTrackString = toString( doc->tracks );
  } else {
    tempTrackString = malloc( sizeof(char) * 50);
    strcpy( tempTrackString, "There are no tracks in this file.\n");
  }
  length += strlen( tempTrackString );

  tempString = malloc( sizeof( char ) * length );

  // add the data
  sprintf( tempString, "GPX doc:\n Version: %.1f\n Creator: %s \n NameSpace:  %s\n%s\n%s\n%s\n", doc->version, doc->creator, doc->namespace, tempWptString, tempRouteString, tempTrackString );

  // free strings used
  free( tempWptString );
  free( tempRouteString );
  free( tempTrackString );

  return tempString;
}



// * * * * * * * * * * * * * * * * * * * * 
// ******** Waypoint Functions ***********
// * * * * * * * * * * * * * * * * * * * *

void deleteWaypoint( void * data ){
  Waypoint * wpt = data;

  free(wpt->name);
  freeList(wpt->otherData);
  free(wpt);
}

// return a string displaying the waypoint nicely
char * waypointToString( void * data ){
  Waypoint * wpt = data;

  char * otherDataString;

  // ? for name, 16 for lon/lat, add in the otherData
  if( getLength(wpt->otherData) != 0 ){
    otherDataString = toString( wpt->otherData );
  } else {
    otherDataString = malloc( sizeof(char) * 50);
    strcpy( otherDataString, "");
  }
  
  int length = strlen(otherDataString) + 16 + strlen(wpt->name) + 50;

  char * wptString = malloc( sizeof(char) * length );

  sprintf(wptString, "  Waypoint, Name: %s\n    Lat: %0.3f, Lon: %0.3f \n%s", wpt->name, wpt->latitude, wpt->longitude, otherDataString);

  free(otherDataString);

  return wptString;
}

int compareWaypoints( const void * first, const void * second ){
  const Waypoint * firstWpt = first;
  const Waypoint * secondWpt = second;

  // compare name, lon, lat
  if( strcmp( firstWpt->name, secondWpt->name) == 0 ){
    if( firstWpt->longitude == secondWpt->longitude ){
      if( firstWpt->latitude == secondWpt->latitude ){
        if( compareOtherDataList( firstWpt->otherData, secondWpt->otherData ) == 1 ){
          return 1;
        }
      }
    }
  }
  return 0;
}



// * * * * * * * * * * * * * * * * * * * * 
// ******** gpxData Functions ************
// * * * * * * * * * * * * * * * * * * * *

void deleteGpxData( void * data ){
  GPXData * gpxData = data;

  free(gpxData);
}

char * gpxDataToString( void * data ){
  GPXData * gpxData = data;
  int length = 50 + 256 + strlen(gpxData->value) + 1;

  char * returnString = malloc( sizeof(char) * length );

  sprintf(returnString, "    |->gpxData, Name: %s\n         Value: %s ", gpxData->name, gpxData->value);

  return returnString;
}

int compareGpxData( const void * first, const void * second ){
  const GPXData * firstData = first;
  const GPXData * secondData = second;

  if( strcmp(firstData->name, secondData->name) == 0 ){
    if( strcmp(firstData->value, secondData->value) == 0 ){
      return 1;
    }
  }
  return 0;
}


// * * * * * * * * * * * * * * * * * * * * 
// ********  Route Functions  ************
// * * * * * * * * * * * * * * * * * * * *
void deleteRoute( void * data ){
  Route * route = data;

  free(route->name);
  freeList(route->waypoints);
  freeList(route->otherData);
  free(route);
}

void dummyDeleteRoute( void * data ){
  // does nothing
}

char* routeToString( void* data ){
  Route * route = data;

  char * tempWptString;
  char * tempOtherDataString;
  char * returnString;
  int length = 0;

  // get string for waypoints
  if( getLength(route->waypoints) != 0 ){
    tempWptString = toString(route->waypoints);
  
  } else {
    tempWptString = malloc( sizeof(char) * 50 );
    strcpy(tempWptString, "    There are no waypoints for this route\n");
  }
  length += ( strlen(tempWptString) + 1 );


  // get string for other data
  if( getLength(route->otherData) != 0 ){
    tempOtherDataString = toString(route->otherData);
  
  } else {
    tempOtherDataString = malloc( sizeof(char) * 50 );
    strcpy( tempOtherDataString, "    There is no other data for this route \n");
  }
  length += ( strlen(tempOtherDataString) + 1 );

  length += ( 50 + strlen(route->name) );

  returnString = malloc( sizeof(char) * length );

  sprintf(returnString, "Route, name: %s\n  Waypoints:\n%s  \n  Other Data:\n%s", route->name, tempWptString, tempOtherDataString );

  free(tempOtherDataString);
  free(tempWptString);

  return returnString;
}

int compareRoutes(const void *first, const void *second){

  const Route * firstRoute = first;
  const Route * secondRoute = second;

  if( strcmp( firstRoute->name, secondRoute->name ) == 0 ){
    if( compareWaypointList( firstRoute->waypoints, secondRoute->waypoints) == 1 ){
      if( compareOtherDataList( firstRoute->otherData, secondRoute->otherData) == 1 ){
        return 1;
      }
    }
  }
  return 0;
}


// * * * * * * * * * * * * * * * * * * * * 
// ********  Track Functions  ************
// * * * * * * * * * * * * * * * * * * * *
void deleteTrack( void * data ){
  Track * track = data;

  free( track->name );
  freeList( track->segments );
  freeList( track->otherData );
  free( track );
}

void dummyDeleteTrack( void * data ){
  // does nothing
}

char* trackToString( void * data ){
  Track * track = data;
  
  int length = 0;

  char * tempSegmentString;
  char * tempOtherString;
  char * returnString;

  if( getLength( track->otherData ) != 0 ){
    tempOtherString = toString( track->otherData );
  } else {
    tempOtherString = malloc( sizeof(char) * 50 );
    strcpy( tempOtherString, "  There is no other data for this track\n");
  }
  length += strlen( tempOtherString ) + 1;

  if( getLength( track->segments) != 0 ){
    tempSegmentString = toString( track->segments );
  } else {
    tempSegmentString = malloc( sizeof(char) * 50 );
    strcpy( tempSegmentString, "  There are no segments for this track\n");
  }
  length += strlen( tempSegmentString ) + 1;
 
  length += strlen( track->name ) + 1;

  length += 50;

  returnString = malloc( sizeof(char) * length );

  sprintf( returnString, "Track, name: %s\n  Segments: %s\n  Other data: %s\n", track->name, tempSegmentString, tempOtherString );

  free( tempOtherString );
  free( tempSegmentString );

  return returnString;
}

int compareTracks( const void *first, const void *second ){

  const Track * firstTrack = first;
  const Track * secondTrack = second;

  if( strcmp( firstTrack->name, secondTrack->name ) == 0 ){
    if( compareTrackSegList( firstTrack->segments, secondTrack->segments ) == 1 ){
      if( compareOtherDataList( firstTrack->otherData, secondTrack->otherData ) == 1 ){
        return 1;
      }
    }
  }

  return 0;
}


// * * * * * * * * * * * * * * * * * * * * 
// *****  Track Segment Functions  *******
// * * * * * * * * * * * * * * * * * * * *

void deleteTrackSegment(void* data){
  TrackSegment * trackSeg = data;
  freeList( trackSeg->waypoints );
  free( trackSeg );
}

char* trackSegmentToString(void* data){
  char * tempWptString;
  char * returnString;

  int length = 0;

  TrackSegment * trackSeg = data;

  if( getLength( trackSeg->waypoints ) != 0 ){
    tempWptString = toString( trackSeg->waypoints );
  } else {
    tempWptString = malloc( sizeof(char) * 50 );
    strcpy( tempWptString, "  There are no waypoints for this track segment\n");
  }
  length += strlen( tempWptString ) + 1;
  length += 50;

  returnString = malloc( sizeof(char) * length );

  sprintf( returnString, "Track Segment\n  Waypoints:%s", tempWptString );

  free( tempWptString );

  return returnString;
}

int compareTrackSegments(const void *first, const void *second){

  const TrackSegment * firstTrackSeg = first;
  const TrackSegment * secondTrackSeg = second;

  if( compareWaypointList( firstTrackSeg->waypoints, secondTrackSeg->waypoints ) == 1 ){
    return 1;
  }
  return 0;
}


// * * * * * * * * * * * * * * * * * * * * 
// **********  Get Functions  ************
// * * * * * * * * * * * * * * * * * * * *

int getNumWaypoints(const GPXdoc* doc){
  if( doc == NULL ){
    return 0;
  }

  int wptCount;
  wptCount = getLength( doc->waypoints );
  return wptCount;
}

int getNumRoutes(const GPXdoc* doc){
  if( doc == NULL ){
    return 0;
  }

  int routeCount;
  routeCount = getLength( doc->routes );
  return routeCount;
}

int getNumTracks(const GPXdoc* doc){
  if( doc == NULL ){
    return 0;
  }

  int trackCount;
  trackCount = getLength( doc->tracks );
  return trackCount;
}

int getNumSegments(const GPXdoc* doc){
  Track * track;
  int segCount = 0;
  
  if( doc == NULL || doc->tracks == NULL ){
    return 0;
  }

  ListIterator trackIter = createIterator( doc->tracks );

  for( track = nextElement( &trackIter ); track != NULL; track = nextElement( &trackIter ) ){
    segCount += getLength( track->segments );
  }

  return segCount;
}

int getNumGPXData(const GPXdoc* doc){
  Waypoint * wpt;
  Track * track;
  Route * route;
  TrackSegment * seg;
  
  ListIterator wptIter;
  ListIterator routeIter;
  ListIterator trackIter;
  ListIterator segIter;

  int dataCount = 0;

  // if doc is null return 0
  if( doc == NULL ){
    return 0;
  }

  // if doc is null return 0
  if( doc->waypoints == NULL || doc->routes == NULL || doc->tracks == NULL ){
    return 0;
  }

  // count the waypoints other data
  if( getLength( doc->waypoints) != 0 ){
    wptIter = createIterator( doc->waypoints );

    for( wpt = nextElement( &wptIter ); wpt != NULL; wpt = nextElement( &wptIter ) ){
      dataCount += getLength( wpt->otherData );
      if( strcmp( wpt->name, "" ) != 0 ){
        dataCount++;
      }
    }
  }

  // route other data
  if( getLength( doc->routes) != 0 ){
    routeIter = createIterator( doc->routes );

    for( route = nextElement( &routeIter ); route != NULL; route = nextElement( &routeIter ) ){
      dataCount += getLength( route->otherData );
      if( strcmp( route->name, "" ) != 0 ){
        dataCount++;
      }
      // count the other data in each waypoint
      if( getLength( route->waypoints) != 0 ){
        wptIter = createIterator( route->waypoints );
        for( wpt = nextElement( &wptIter ); wpt != NULL; wpt = nextElement( &wptIter ) ){
          dataCount += getLength( wpt->otherData );
          if( strcmp( wpt->name, "" ) != 0 ){
            dataCount++;
          }
        }
      }
    }
  }

  // track other data
  if( getLength( doc->tracks) != 0 ){
    trackIter = createIterator( doc->tracks );

    for( track = nextElement( &trackIter ); track != NULL; track = nextElement( &trackIter ) ){
      dataCount += getLength( track->otherData );
      if( strcmp( track->name, "" ) != 0 ){
        dataCount++;
      }
      // count the other data in each track segment
      if( getLength( track->segments ) != 0 ){
        segIter = createIterator( track->segments );
        // for each track segment
        for( seg = nextElement( &segIter ); seg != NULL; seg = nextElement( &segIter ) ){
          if( getLength(seg->waypoints) != 0 ){
            // which each have a list of waypoints
            wptIter = createIterator( seg->waypoints );
            for( wpt = nextElement( &wptIter ); wpt != NULL; wpt = nextElement( &wptIter ) ){
              dataCount += getLength( wpt->otherData );
              if( strcmp( wpt->name, "") != 0 ){
                dataCount++;
              }
            }
          }
        }
      }
    }
  }

  return dataCount;
}


Waypoint * getWaypoint( const GPXdoc * doc, char * name ){
  if( doc == NULL || name == NULL ){
    return NULL;
  }
  
  ListIterator wptIter;
  Waypoint * wpt;

  // loop the waypoints and compare the names
  if( getLength( doc->waypoints) != 0 ){
    wptIter = createIterator( doc->waypoints );
    for( wpt = nextElement( &wptIter ); wpt != NULL; wpt = nextElement( &wptIter ) ){
      if( strcmp( wpt->name, name ) == 0 ){
        return wpt;
      }
    }
  }
  return NULL;
}

Track * getTrack( const GPXdoc * doc, char * name){
  if( doc == NULL || name == NULL ){
    return NULL;
  }

  Track * track;
  ListIterator trackIter;

  // loop through tracks and compare names
  if( getLength( doc->tracks) != 0 ){
    trackIter = createIterator( doc->tracks );
    for( track = nextElement( &trackIter ); track != NULL; track = nextElement( &trackIter ) ){
      if( strcmp( track->name, name ) == 0 ){
        return track;
      }
    }
  }
  return NULL;
}

Route * getRoute( const GPXdoc * doc, char * name ){
  if( doc == NULL || name == NULL ){
    return NULL;
  }

  ListIterator routeIter;
  Route * route;

  // route other data
  if( getLength( doc->routes) != 0 ){
    routeIter = createIterator( doc->routes );

    for( route = nextElement( &routeIter ); route != NULL; route = nextElement( &routeIter ) ){
      if( strcmp( route->name, name ) == 0 ){
        return route;
      }
    }
  }
  
  return NULL;
}




// * * * * * * * * * * * * * * * * * * * * 
// ********    A2 Functions    ***********
// * * * * * * * * * * * * * * * * * * * *

GPXdoc * createValidGPXdoc( char * fileName, char * gpxSchemaFile ){

  xmlDoc * doc;

  if( fileName == NULL || gpxSchemaFile == NULL ){
    return NULL;
  }

  // get the document, pass it to the validator
  doc = xmlReadFile(fileName, NULL, 0);
  
  if( doc == NULL ){
    return NULL;
  }

  // if the file is valid, create the gpx doc and then return it
  if( validateTree( doc, gpxSchemaFile ) ){
    xmlCleanupParser();
    xmlFreeDoc( doc );
    return createGPXdoc( fileName );
  } else {
    xmlCleanupParser();
    xmlFreeDoc( doc );
    return NULL;
  }
}

bool writeGPXdoc( GPXdoc * doc, char * fileName ){
  int ret;
  xmlDoc * printDoc;
  
  // check args
  if( doc == NULL || fileName == NULL ){
    return false;
  }

  // pass the input to docToDoc
  printDoc = docToDoc( doc );

  // save the returned document to file
  ret = xmlSaveFormatFileEnc( fileName, printDoc, "ISO-8859-1", 1);

  xmlFreeDoc( printDoc );
  xmlCleanupParser();

  if( ret != -1 ){
    return true;
  }
  return false;
}

// check proper xml format and proper gpxDoc constraints
bool validateGPXDoc( GPXdoc * gpxDoc, char * gpxSchemaFile ){
  bool validReturn;
  bool wptReturn;
  bool routeReturn;
  bool trackReturn;

  xmlDoc * xmlTree;
  
  // check args
  if( gpxDoc == NULL || gpxSchemaFile == NULL ){
    return false;
  }

  // turn the gpx doc into a xml doc
  xmlTree = docToDoc( gpxDoc );

  // check return
  if( xmlTree == NULL ){
    return false;
  }
  
  // use validate tree
  validReturn = validateTree( xmlTree, gpxSchemaFile );

  if( !validReturn ){
    return false;
  }

  // check constraints in gpxparser.h
  // check gpxDoc variables
  if( strcmp( gpxDoc->namespace, "") == 0 ){
    // namespace is empty and therefore false
    return false;
  }

  if( gpxDoc->creator != NULL ){
    if( strcmp( gpxDoc->creator, "" ) == 0 ){
      return false;
    }
  } else {
    return false;
  }

  // check waypoints
  wptReturn = checkWaypointList( gpxDoc->waypoints );
  
  if( !wptReturn ){
    return false;
  }

  // check routes
  routeReturn = checkRouteList( gpxDoc->routes );

  if( !routeReturn ){
    return false;
  }

  // check tracks
  trackReturn = checkTrackList( gpxDoc->tracks );

  if( !trackReturn ){
    return false;
  }

  // cleanup
  xmlFreeDoc( xmlTree );
  xmlCleanupParser();

  return true;
}


// * * * * * * * * * * * * * * * * * * * * 
// *****    A2 Mod 2 Functions    ********
// * * * * * * * * * * * * * * * * * * * *

// passes harness
// works, no leaking
float round10( float len ){
  // divide by 10 -> x 
  double y = len;

  while( len -10 > 0 ){
    len -= 10;
  }

  if( len >= 5){
    y += 10;
  }

  y -= len;

  return y;
}

// passes harness
// tested for values and leaks
float getRouteLen( const Route * rt ){
  float returnLength;

  // return 0 if the route is null
  if( rt == NULL ){
    return 0;
  }

  // get the length of the route, which is just a list of waypoints
  if( getLength( rt->waypoints ) > 1 ){
    returnLength = getLengthWaypointsList( rt->waypoints );
  } else {
    return 0;
  }

  return returnLength;
}

// tested for values and leaks
float getTrackLen( const Track * tr ){
  bool nextSeg = true;

  float returnLength;

  Waypoint * wpt1;
  Waypoint * wpt2; 

  TrackSegment * curTrackSeg;
  TrackSegment * secondTrackSeg;
  ListIterator trackSegIter;
  ListIterator secondTrackSegIter;

  // return 0 if the track is null
  if( tr == NULL ){
    return 0;
  }

  returnLength = 0;

  if( getLength( tr->segments ) != 0 ){

    trackSegIter = createIterator( tr->segments );
    secondTrackSegIter = createIterator( tr->segments );

    // set second iter to second segment
    curTrackSeg = nextElement( &trackSegIter );
    secondTrackSeg = nextElement( &secondTrackSegIter );

    while( curTrackSeg != NULL ){
      nextSeg = true;
      secondTrackSeg = nextElement( &secondTrackSegIter );


      if( getLength( curTrackSeg->waypoints ) != 0 ){

        // get length of this track, add it
        returnLength += getLengthWaypointsList( curTrackSeg->waypoints );

        // length between this track seg and next
        // move seconTrackSeg to the next segment with any wpts
        while( nextSeg ){
          if( secondTrackSeg == NULL ){
            nextSeg = false;
          } else {
            if( getLength( secondTrackSeg->waypoints ) != 0 ){
              nextSeg = false;
            } else {
              secondTrackSeg = nextElement( &secondTrackSegIter );
            }
          }
        }

        // if secondTrackSeg made it to null, it is at the end of the list, and therefore there are no more calcs to be made
        if( secondTrackSeg == NULL ){
          return returnLength;
        } else {
          // get wpts, calc distance
          wpt1 = getFromBack( curTrackSeg->waypoints );
          wpt2 = getFromFront( secondTrackSeg->waypoints );

          returnLength += getLengthWaypoints( wpt1, wpt2 );
        }
      }

      // move curTrackSeg to the next segment 
      curTrackSeg = nextElement( &trackSegIter );

      // keep moving curTrackSeg if length is 0, until one without length of 0 is found
      while( getLength( curTrackSeg->waypoints ) == 0 ){
        curTrackSeg = nextElement( &trackSegIter );
      }
    }
  }

  return returnLength;
}

// passes harness
// no leaks should be possible
int numRoutesWithLength( const GPXdoc * doc, float len, float delta ){
  bool compareReturn;
  
  int routes;

  float routeLen;

  ListIterator routeIter;
  Route * route;

  if( doc == NULL || len < 0 || delta < 0 ){
    return 0;
  }

  routeIter = createIterator( doc->routes );

  routes = 0;

  // for each route in the doc
  for( route = nextElement( &routeIter ); route != NULL; route = nextElement( &routeIter ) ){
    
    // get the length of the route
    routeLen = getRouteLen( route );
    
    // compare the length
    compareReturn = compareLength( routeLen, len, delta );

    if( compareReturn ){
      routes++;
    }
  }

  return routes;
}

// test for values and leaks
// shouldnt be any memeory leaks
int numTracksWithLength( const GPXdoc * doc, float len, float delta ){
  bool compareReturn;
  
  int tracks;
  float trackLen;

  ListIterator listIter;
  Track * curTrack;

  if( doc == NULL || len < 0 || delta < 0 ){
    return 0;
  }

  tracks = 0;

  listIter = createIterator( doc->tracks );

  // loop through the track list
  if( getLength( doc->tracks ) != 0 ){
    for( curTrack = nextElement( &listIter ); curTrack != NULL; curTrack = nextElement( &listIter ) ){

      trackLen = getTrackLen( curTrack );

      compareReturn = compareLength( trackLen, len, delta );

      if( compareReturn ){
        tracks++;
      }      
    }
  }

  return tracks;
}


bool isLoopRoute( const Route * rt, float delta ){

  bool compareReturn;

  float distance;

  Waypoint * wpt1;
  Waypoint * wpt2;

  if( rt == NULL || delta < 0 ){
    return false;
  }

  // check if has at least 4 waypoints
  if( getLength( rt->waypoints ) < 4 ){
    return false;
  }

  // get the first and last points
  wpt1 = getFromFront( rt->waypoints );
  wpt2 = getFromBack( rt->waypoints );
  
  // distance between them
  distance = getLengthWaypoints( wpt1, wpt2 );

  // compare length
  compareReturn = compareLength( distance, 0, delta );

  if( compareReturn ){
    return true;
  }
  return false;
}

// passes harness
// no reason for leak
bool isLoopTrack( const Track * tr, float delta ){
  bool compareReturn;

  int wptCount;
  float distance;

  Waypoint * wpt1;
  Waypoint * wpt2;

  TrackSegment * trackSeg1;
  TrackSegment * trackSeg2;

  TrackSegment * curTrackSeg;
  ListIterator trackSegIter;
  
  if( tr == NULL || delta < 0 ){
    return false;
  }

  wptCount = 0;

  trackSegIter = createIterator( tr->segments );

  // check each track seg length
  for( curTrackSeg = nextElement( &trackSegIter ); curTrackSeg != NULL; curTrackSeg = nextElement( &trackSegIter ) ){
    wptCount = getLength( curTrackSeg->waypoints );

    if( wptCount >= 4 ){
      curTrackSeg = NULL;
    }
  }

  if( wptCount < 4 ){
    return false;
  }

  // get first wpt from first track seg, last wpt from last track segment
  trackSeg1 = getFromFront( tr->segments );
  trackSeg2 = getFromBack( tr->segments );

  wpt1 = getFromFront( trackSeg1->waypoints );
  wpt2 = getFromBack( trackSeg2->waypoints );

  // distance between them
  distance = getLengthWaypoints( wpt1, wpt2 );

  // compare length
  compareReturn = compareLength( distance, 0, delta );

  if( compareReturn ){
    return true;
  }
  return false;
}

// passes harness
List * getRoutesBetween( const GPXdoc * doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){

  bool start;
  bool end;

  float startDif;
  float endDif;

  Waypoint * wpt;
  Route * route;
  ListIterator routeIter;

  List * returnList;

  if( doc == NULL ){
    return NULL;
  }

  routeIter = createIterator( doc->routes );

  //create the list first
  returnList = initializeList( routeToString, dummyDeleteRoute, compareRoutes );

  // loop through each route
  for( route = nextElement( &routeIter ); route != NULL; route = nextElement( &routeIter ) ){
    if( getLength( route->waypoints) != 0 ){
      wpt = getFromFront( route->waypoints );
      
      // check if start matches start point provided
      if( wpt == NULL ){
      }
      startDif = haversine( wpt->latitude, wpt->longitude, sourceLat, sourceLong );

      wpt = getFromBack( route->waypoints );
      // check if end matches end point provided
      endDif = haversine( wpt->latitude, wpt->longitude, destLat, destLong );

      start = compareLength( startDif, 0, delta );
      end = compareLength( endDif, 0, delta );

      if( start && end ){
        // if true add the route to return list
        insertBack( returnList, route );
      }
    }
  }

  // check length == 0, if so delete the list, return null
  if( getLength( returnList ) == 0 ){
    freeList( returnList );
    return NULL;
  }

  return returnList;
}

List * getTracksBetween( const GPXdoc * doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta ){

  bool start;
  bool end;

  float startDif;
  float endDif;

  Waypoint * wpt;
  Track * track;
  TrackSegment * trackSeg;
  ListIterator trackIter;

  List * returnList;

  if( doc == NULL ){
    return NULL;
  }

  trackIter = createIterator( doc->tracks );

  returnList = initializeList( trackToString, dummyDeleteTrack, compareTracks );

  // loop through each track
  for( track = nextElement( &trackIter ); track != NULL; track = nextElement( &trackIter ) ){

    // get first wpt from first track seg
    trackSeg = getFromFront( track->segments );
    wpt = getFromFront( trackSeg->waypoints );
  
    // check if start matches start point provided
    startDif = haversine( wpt->latitude, wpt->longitude, sourceLat, sourceLong );


    // get last wpt from last track seg
    trackSeg = getFromBack( track->segments );
    wpt = getFromBack( trackSeg->waypoints );

    // check if start matches start point provided
    endDif = haversine( wpt->latitude, wpt->longitude, destLat, destLong );

    start = compareLength( startDif, 0, delta );
    end = compareLength( endDif, 0, delta );

    if( start && end ){
      // if true add the route to return list
      insertBack( returnList, track );
    }
  }

  // check length == 0, if so delete the list, return null
  if( getLength( returnList ) == 0 ){
    freeList( returnList );
    return NULL;
  }

  return returnList;
}






// * * * * * * * * * * * * * * * * * * * * 
// *****    A2 Mod 3 Functions    ********
// * * * * * * * * * * * * * * * * * * * *

// tested for values and leaks
char * routeToJSON( const Route * rt ){

  char * returnString;
  int strSize;

  // need name, num points, length, loop status
  char * name;
  int numPoints;
  float routeLen;
  bool loopStat;

  // set base size for required spaces, brackets etc
  // "name":"","numPoints":,"len":,"loop":false}
  strSize = 75;

  returnString = malloc( strSize );

  if( rt == NULL ){
    sprintf( returnString, "{}" );
    return returnString;
  }

  // get the name
  if( strcmp( rt->name, "" ) == 0 ){
    name = malloc( 10 );
    strcpy( name, "None" );
  
  } else {
    name = malloc( strlen( rt->name ) + 1 );
    strcpy( name, rt->name );
  }
  strSize += strlen( name + 1);

  // get the numPoints
  numPoints = getLength( rt->waypoints );

  // get the routeLen
  routeLen = getRouteLen( rt );
  routeLen = round10( routeLen );

  // get the loopStat
  loopStat = isLoopRoute( rt, 10 );

  returnString = realloc( returnString, strSize );

  // if its a loop
  if( loopStat ){
    sprintf(returnString, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%0.1f,\"loop\":true}", name, numPoints, routeLen );

  // if its not
  } else {
    sprintf(returnString, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%0.1f,\"loop\":false}", name, numPoints, routeLen );
  }

  // clean up
  free( name );

  return returnString;
}

// tested for values and leaks
char * trackToJSON( const Track * tr ){
  char * returnString;
  int strSize;

  // need name, length, loop status
  char * name;
  float trackLen;
  bool loopStat;

  // set base size for required spaces, brackets etc
  // "name":"","numPoints":,"len":,"loop":false}
  strSize = 75;

  returnString = malloc( strSize );

  if( tr == NULL ){
    sprintf( returnString, "{}" );
    return returnString;
  }

  // get the name
  if( strcmp( tr->name, "" ) == 0 ){
    name = malloc( 10 );
    strcpy( name, "None" );
  
  } else {
    name = malloc( strlen( tr->name ) + 1 );
    strcpy( name, tr->name );
  }
  strSize += strlen( name + 1);

  // get trackLen
  trackLen = getTrackLen( tr );
  trackLen = round10( trackLen );

  // get loop status
  loopStat = isLoopTrack( tr, 10 );

  returnString = realloc( returnString, strSize );

  // if its a loop
  if( loopStat ){
    sprintf(returnString, "{\"name\":\"%s\",\"len\":%0.1f,\"loop\":true}", name, trackLen );

  // if its not
  } else {
    sprintf(returnString, "{\"name\":\"%s\",\"len\":%0.1f,\"loop\":false}", name, trackLen );
  }

  // cleanup
  free( name );

  return returnString;
}


// tested for values and leaks
char * routeListToJSON( const List * list ){

  char * returnString;

  Route * route;
  Route * secondRoute;
  ListIterator routeIter;
  ListIterator secondRouteIter;

  returnString = malloc( sizeof( char ) * 10 );

  if( list == NULL || getLength( ( List * ) list ) == 0 ){
    strcpy( returnString, "[]" );
    return returnString;
  }
  
  routeIter = createIterator( (List * ) list );
  secondRouteIter = createIterator( (List * ) list );

  strcpy( returnString, "[");

  //move secondRouteIter to second route
  secondRoute = nextElement( &secondRouteIter );
  secondRoute = nextElement( &secondRouteIter );

  // loop through each route
  for( route = nextElement( &routeIter ); route != NULL; route = nextElement( &routeIter ) ){
    // get the route JSON string
    char * tempRouteString = routeToJSON( route );

    // add its length plus a little bit to the over string length
    int newLen = strlen( returnString ) + 10 + strlen( tempRouteString );

    // reallocate space for the new string
    returnString = realloc( returnString, newLen );

    // add the new string
    strcat( returnString, tempRouteString );

    // if it is not the last route in the list, add the comma to sepereate each route JSON string
    if( secondRoute != NULL ){
      strcat( returnString, "," );
    }
    secondRoute = nextElement( &secondRouteIter );

    free( tempRouteString );

  }

  strcat( returnString, "]" );

  return returnString;
}

// tested for values and leaks
char * trackListToJSON( const List * list ){
  
  char * returnString;

  Track * track;
  Track * secondTrack;
  ListIterator trackIter;
  ListIterator secondTrackIter;

  returnString = malloc( sizeof( char ) * 10 );

  if( list == NULL || getLength( ( List * ) list ) == 0 ){
    strcpy( returnString, "[]" );
    return returnString;
  }
  
  strcpy( returnString, "[");

  trackIter = createIterator( (List * ) list );
  secondTrackIter = createIterator( (List * ) list );

  //move secondTrackIter to second route
  secondTrack = nextElement( &secondTrackIter );
  secondTrack = nextElement( &secondTrackIter );

  for( track = nextElement( &trackIter ); track != NULL; track = nextElement( &trackIter ) ){
    // get the track JSON string
    char * tempTrackString = trackToJSON( track );

    // add the new length
    int newLen = strlen( returnString ) + 10 + strlen( tempTrackString );

    // reallocate space for the new string
    returnString = realloc( returnString, newLen );

    // add the new string
    strcat( returnString, tempTrackString );

    // if it is not the last track, add a comma
    if( secondTrack != NULL ){
      strcat(returnString, ",");
    }

    secondTrack = nextElement( &secondTrackIter );
  
    free( tempTrackString );
  }

  strcat( returnString, "]" );

  return returnString;
}

// tested for values and leaks
char * GPXtoJSON( const GPXdoc * gpx ){

  char * returnString;
  int strSize;

  int wptCount;
  int routeCount;
  int trackCount;

  // if gpx is null, return empty JSON string
  returnString = malloc( sizeof( char ) * 10 );

  if( gpx == NULL ){
    strcpy( returnString, "{}" );
    return returnString;
  }

  // "version":vers,"creator":"","numWaypoints":numW,"numRoutes":numR,"numTracks":numT}
  strSize = 100;

  strSize += ( strlen( gpx->creator ) + 1);

  returnString = realloc( returnString, sizeof( char ) * strSize );

  // get wpt, route, and track counts
  wptCount    = getNumWaypoints( gpx );
  routeCount  = getNumRoutes( gpx );
  trackCount  = getNumTracks( gpx );

  sprintf(returnString, "{\"version\":%0.1f,\"creator\":\"%s\",\"numWaypoints\":%d,\"numRoutes\":%d,\"numTracks\":%d}", gpx->version, gpx->creator, wptCount, routeCount, trackCount );
  
  return returnString;
}

// not tested
void addWaypoint( Route * rt, Waypoint * pt ){
  if( rt != NULL && pt != NULL ){
    // add waypoint
    insertBack( rt->waypoints, pt );
  }
}


// not tested
void addRoute( GPXdoc * doc, Route * rt ){
  if( doc != NULL && rt != NULL ){
    // add the route
    insertBack( doc->routes, rt );
  }
}

// tested for values and leaks
GPXdoc * JSONtoGPX( const char * gpxString ){
  
  char * token;
  char * versionPtr;
  char * creatorPtr;
  
  GPXdoc * returnDoc;

  if( gpxString == NULL ){
    return NULL;
  }

  // create a copy of the string
  char * string = malloc( sizeof( char ) * ( strlen( gpxString ) + 1 ) );
  strcpy( string, gpxString );


  // tokenize the gpxString by ","
  token = strtok( string, "," );

  // first token is version
  versionPtr = strtok( token, ":" );
  versionPtr = strtok( NULL, ":" );


  // reset the string
  strcpy( string, gpxString );

  // get the second token, the creator
  token = strtok( string, "," );
  token = strtok( NULL, "," );

  // get the creator string itself
  creatorPtr = strtok( token, ":" );
  creatorPtr = strtok( NULL, ":" );

  // cleanup the creator string
  creatorPtr = strchr( creatorPtr, '\"' );
  creatorPtr += 1;
  creatorPtr[ strlen( creatorPtr ) - 2 ] = '\0';
  
  // allocate a new gpxDoc and put in the data
  returnDoc = malloc( sizeof( GPXdoc ) );
  returnDoc->creator = malloc( sizeof( char ) * strlen( creatorPtr ) + 1 );

  strcpy( returnDoc->creator, creatorPtr );
  returnDoc->version = strtod( versionPtr, NULL );
  strcpy( returnDoc->namespace, "http://www.topografix.com/GPX/1/1" );

  // initialize the lists
  List * wptList = initializeList( waypointToString, deleteWaypoint, compareWaypoints );
  List * routeList = initializeList( routeToString, deleteRoute, compareWaypoints );
  List * trackList = initializeList( trackToString, deleteTrack, compareTracks );

  returnDoc->waypoints = wptList;
  returnDoc->routes = routeList;
  returnDoc->tracks = trackList;

  free( string );

  return returnDoc;
}

// tested for values and leaks
Waypoint * JSONtoWaypoint( const char * gpxString ){
  
  char * token;
  char * string;
  char * lat;
  char * lon;

  double latValue;
  double lonValue;

  Waypoint * returnWpt;

  if( gpxString == NULL ){
    return NULL;
  }

  // create a copy of the string
  string = malloc( sizeof( char ) * ( strlen( gpxString ) + 1 ) );
  strcpy( string, gpxString );


  // tokenize the gpxString by ","
  token = strtok( string, "," );

  // get the lattitude
  lat = strtok( token, ":" );
  lat = strtok( NULL, ":" );

  latValue = strtod( lat, NULL );

  strcpy( string, gpxString );
  
  // get the longitude 
  token = strtok( string, "," );
  token = strtok( NULL, "," );

  // get the lattitude
  lon = strtok( token, ":" );
  lon = strtok( NULL, ":" );

  lon[ strlen( lon ) - 1 ] = '\0';

  lonValue = strtod( lon, NULL );


  // malloc the wpt and input values
  returnWpt = malloc( sizeof( Waypoint ) );

  returnWpt->latitude = latValue;
  returnWpt->longitude = lonValue;

  // initalize the otherData list
  List * otherDataList = initializeList(gpxDataToString, deleteGpxData, compareGpxData );
  returnWpt->otherData = otherDataList;

  // set the name to empty string
  returnWpt->name = malloc( sizeof( char ) * 10 );

  strcpy( returnWpt->name, "" );

  free( string );
  
  return returnWpt;
}


// tested for values and leaks
Route * JSONtoRoute( const char * gpxString ){

  char * token;
  char * string;

  Route * returnRoute;  

  if( gpxString == NULL ){
    return NULL;
  }

  string = malloc( sizeof( char ) * strlen( gpxString ) + 1 );
  strcpy( string, gpxString );

  token = strtok( string, "\"" );
  token = strtok( NULL, "\"" );
  token = strtok( NULL, "\"" );
  token = strtok( NULL, "\"" );

  returnRoute = malloc( sizeof( Route ) );

  returnRoute->name = malloc( sizeof( char ) * strlen( token ) + 1 );
  strcpy( returnRoute->name, token );

  List * otherDataList  = initializeList( gpxDataToString, deleteGpxData, compareGpxData );
  returnRoute->otherData = otherDataList;

  List * wptList        = initializeList( waypointToString, deleteWaypoint, compareWaypoints );
  returnRoute->waypoints = wptList;


  free( string );
  return returnRoute;
}