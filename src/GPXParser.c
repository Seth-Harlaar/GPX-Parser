// Seth Harlaar -- 1109524


#include "GPXParser.h"
#include "GPXhelpers.h"
#include <stdlib.h>



// * * * * * * * * * * * * * * * * * * * * 
// ********  GPXDoc Functions  ***********
// * * * * * * * * * * * * * * * * * * * *

// make the gpx doc
GPXdoc * createGPXdoc(char* fileName){

  if( fileName == NULL ){
    return NULL;
  }

  // vars 
  GPXdoc * returnDoc = malloc( sizeof(GPXdoc) );
  xmlDoc * doc = NULL;
  xmlNode * headNode = NULL;

  // get doc and check if null
  doc = xmlReadFile(fileName, NULL, 0);

  if( doc == NULL ){
    return NULL;
  }

  headNode = xmlDocGetRootElement(doc);

  // get namespace -- cannot be an empty string, assume there is only one
  

  // get version -- must be initialized
  double tempVers = strtod((char *) doc->version, NULL);

  returnDoc->version = tempVers;

  // get creator -- cannot be null, cannot be an empty string

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
  xmlCleanupParser();

  return returnDoc;
}

// get rid of the gpx doc
void deleteGPXdoc(GPXdoc* doc){
  // destroy all the data in there
  // free the creator value because its malloced
  
  // free the different lists
  freeList( doc->waypoints );
  freeList( doc->routes );
  freeList( doc->tracks );

  // free the struct itself
  free(doc);
}

// turn the gpx doc into a string
// the code for this function was heavily inspired by the structListDemo.c file
char* GPXdocToString(GPXdoc* doc){
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
  sprintf( tempString, "GPX doc:\n Version: %.1f\n %s\n%s\n%s\n", doc->version, tempWptString, tempRouteString, tempTrackString );

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

  sprintf(returnString, "Route, name: %s\n  Waypoints:\n%s  \nOther Data:\n%s", route->name, tempWptString, tempOtherDataString );

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
    strcpy( tempOtherString, "  There is no other data for this file\n");
  }
  length += strlen( tempOtherString ) + 1;

  if( getLength( track->segments) != 0 ){
    tempSegmentString = toString( track->segments );
  } else {
    tempSegmentString = malloc( sizeof(char) * 50 );
    strcpy( tempSegmentString, "  There are no segments for this file\n");
  }
  length += strlen( tempSegmentString ) + 1;
 
  length += strlen( track->name ) + 1;

  length += 50;

  returnString = malloc( sizeof(char) * length );

  sprintf( returnString, "Track, name: %s\n  Segments: %s\n Other data: %s\n", track->name, tempSegmentString, tempOtherString );

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