#include "../include/GPXParser.h"
#include "../include/GPXhelpers.h"



// * * * * * * * * * * * * * * * * * * * * 
// ******** Waypoint Functions ***********
// * * * * * * * * * * * * * * * * * * * *

// start the list, run the function to add the waypoints
List * getWaypointsList( xmlNode * headNode, int mode ){
  // initialize the list
  List * returnList = initializeList( waypointToString, deleteWaypoint, compareWaypoints );
  // add all the waypoints to the list
  addWaypoints( returnList, headNode, mode );

  return returnList;
}

// find all the way points and add them to the list
void addWaypoints( List * wptList, xmlNode * headNode, int mode ){
  xmlNode * iterator = NULL;

  // this loop taken from the libxmlexample.c file
  for(iterator = headNode; iterator != NULL; iterator = iterator->next){
    // check if its a waypoint
    if (iterator->type == XML_ELEMENT_NODE){
      if( strcmp((char *)(iterator->name), "wpt") == 0 && mode == 0 ){
        // parse the waypoint, add the data to the list element
        Waypoint * newWpt = malloc( sizeof(Waypoint) );
        
        newWpt->otherData = initializeList(gpxDataToString, deleteGpxData, compareGpxData );

        parseWaypoint( newWpt, iterator );

        insertFront( wptList, newWpt );
      }  else if( strcmp((char *)(iterator->name), "rtept") == 0 && mode == 1 ){
        Waypoint * newWpt = malloc( sizeof(Waypoint) );

        newWpt->otherData = initializeList(gpxDataToString, deleteGpxData, compareGpxData );

        parseWaypoint( newWpt, iterator );

        insertFront( wptList, newWpt );
        
      } else if( strcmp((char *)(iterator->name), "trkpt") == 0 && mode == 2 ){
        Waypoint * newWpt = malloc( sizeof(Waypoint) );

        newWpt->otherData = initializeList(gpxDataToString, deleteGpxData, compareGpxData );

        parseWaypoint( newWpt, iterator );

        insertFront( wptList, newWpt );
      
      } else {
        // search the children of the node
        addWaypoints( wptList, iterator->children, mode );
      }
    }   
  }
}


// parse each waypoint and get the data out of them + their children
void parseWaypoint( Waypoint * newWpt, xmlNode * curNode ){
  // fill the waypoint with data
  newWpt->name = malloc( sizeof(char) * 2 );
  strcpy(newWpt->name, "");

  // search the attributes of the waypoint
  xmlAttr * iterPoint = NULL;

  // this loop was taken from libxmlexample.c
  for( iterPoint = curNode->properties; iterPoint != NULL; iterPoint = iterPoint->next){
    xmlNode * value = iterPoint->children;
    
    char * attrName = (char *) iterPoint->name;
    char * cont  = (char *)(value->content);

    // check if it is a lon or lan attribute, store the data if so 
    if( strcmp(attrName, "lon") == 0 ){
      newWpt->longitude = strtod(cont, NULL);

    } else if( strcmp(attrName, "lat") == 0 ){
      newWpt->latitude = strtod(cont, NULL);
    }
  }

  // search for children of the node
  xmlNode * childIterNode = NULL;

  for( childIterNode = curNode->children; childIterNode != NULL; childIterNode = childIterNode->next ){
    // if the child has a name of name, it is the name, get the contents
    char * contents = NULL;
    if (childIterNode->type == XML_ELEMENT_NODE){
      if( strcmp( (char *)(childIterNode->name), "name") == 0 ){
        // get the contents, realloc, then add the name
        contents = (char *) xmlNodeGetContent( childIterNode );
      
        int length = strlen( contents ) + 1;
        newWpt->name = realloc( newWpt->name, length );

        strcpy( newWpt->name, contents );
      
      } else {
        // malloc space for the data, fill it
        contents = (char *) xmlNodeGetContent( childIterNode );

        GPXData * newData = malloc( sizeof(GPXData) + strlen(contents) + 1 );
        
        strcpy( newData->name, (char *)( childIterNode->name) );
        strcpy( newData->value, contents );
        
        // add the data to the otherData list
        insertFront( newWpt->otherData, newData );
      }
      xmlFree(contents);
    }
  }
}


// * * * * * * * * * * * * * * * * * * * * 
// ********  Route Functions  ************
// * * * * * * * * * * * * * * * * * * * *


List * getRoutesList( xmlNode * headNode ){
  // initialize list
  List * returnList = initializeList( routeToString, deleteRoute, compareWaypoints );

  addRoutes( returnList, headNode );

  return returnList;
}

void addRoutes( List * routeList, xmlNode * headNode ){
  xmlNode * iterator;
  // search for routes

  for( iterator = headNode; iterator != NULL; iterator = iterator->next ){

    if ( iterator->type == XML_ELEMENT_NODE ){
      if( strcmp((char *)(iterator->name), "rte") == 0 ){
        Route * newRoute;

        newRoute = parseRoute( iterator );

        insertFront( routeList, newRoute );
      } else {
        addRoutes( routeList, iterator->children );
      }
    }
  }
}

Route * parseRoute( xmlNode * curNode ){
  // malloc new route - malloc for name, initialize list for otherData and waypoints
  Route * returnRoute = malloc( sizeof(Route) );

  returnRoute->name = malloc( sizeof(char) *2);
  strcpy( returnRoute->name, "" );

  // get the waypoints data out
  returnRoute->waypoints = getWaypointsList( curNode, 1 );

  // get the otherData out
  returnRoute->otherData = initializeList( gpxDataToString, deleteGpxData, compareGpxData );

  xmlNode * childIterNode;
  char * contents;

  for( childIterNode = curNode->children; childIterNode != NULL; childIterNode = childIterNode->next ){
    if (childIterNode->type == XML_ELEMENT_NODE){
      if(  !(strcmp( (char *)(childIterNode->name), "wpt" ) == 0 ) && !(strcmp( (char *)(childIterNode->name) , "rtept") == 0 )  ){
        if( strcmp( (char *)(childIterNode->name), "name") == 0 ){
          // get the contents, realloc, then add the name
          contents = (char *) xmlNodeGetContent( childIterNode );
        
          int length = strlen( contents ) + 1;
          returnRoute->name = realloc( returnRoute->name, length );

          strcpy( returnRoute->name, contents );
      
        } else {
          contents = (char *) xmlNodeGetContent( childIterNode );

          GPXData * newData = malloc( sizeof(GPXData) + strlen(contents) + 1);

          strcpy( newData->name, (char *)(childIterNode->name) );
          strcpy( newData->value, contents );

          insertFront( returnRoute->otherData, newData );
        }
        xmlFree(contents);
      }
    }
  }

  // return route
  return returnRoute;
}

// * * * * * * * * * * * * * * * * * * * * 
// ******  TrackSegment Functions  *******
// * * * * * * * * * * * * * * * * * * * *

List * getTrackSegList( xmlNode * headNode ){
  List * trackSegList = initializeList( trackSegmentToString, deleteTrackSegment, compareTrackSegments );

  addTrackSegments( trackSegList, headNode );

  return trackSegList;
}

// search through and find all the track segments
void addTrackSegments( List * trackSegList, xmlNode * headNode ){

  xmlNode * iterator;

  for( iterator = headNode; iterator != NULL; iterator = iterator->next ){
    if (iterator->type == XML_ELEMENT_NODE){
      if( strcmp((char *)(iterator->name), "trkseg") == 0 ){
        TrackSegment * newTrackSeg;

        newTrackSeg = parseTrackSeg( iterator );

        insertFront( trackSegList, newTrackSeg );
      } else {
        addTrackSegments( trackSegList, iterator->children );
      }
    }
  }

}

TrackSegment * parseTrackSeg( xmlNode * curNode ){
  // track segment is just a list of waypoints
  TrackSegment * returnTrackSeg = malloc( sizeof(TrackSegment) );

  returnTrackSeg->waypoints = getWaypointsList( curNode, 2 );

  return returnTrackSeg;
}



// * * * * * * * * * * * * * * * * * * * * 
// *********  Track Functions  ***********
// * * * * * * * * * * * * * * * * * * * *

List * getTracksList( xmlNode * headNode ){
  List * trackList = initializeList( trackToString, deleteTrack, compareTracks );

  addTracks( trackList, headNode );

  return trackList;
}

void addTracks( List * trackList, xmlNode * headNode ){
  xmlNode * iterator;

  for( iterator = headNode; iterator != NULL; iterator = iterator->next ){
    if (iterator->type == XML_ELEMENT_NODE){
      if( strcmp((char *)(iterator->name), "trk") == 0 ){
        Track * newTrack;

        newTrack = parseTrack( iterator );

        insertFront( trackList, newTrack );
      } else {
        addTracks( trackList, iterator->children );
      }
    }
  }
}

Track * parseTrack( xmlNode * curNode ){
  Track * returnTrack = malloc( sizeof(Track) );

  returnTrack->name = malloc( sizeof(char) );
  strcpy( returnTrack->name, "" );

  // get the tracksegment data out
  returnTrack->segments = getTrackSegList( curNode );

  // get the other data
  returnTrack->otherData = initializeList( gpxDataToString, deleteGpxData, compareGpxData );

  xmlNode * childIterNode;
  char * contents;

  for( childIterNode = curNode->children; childIterNode != NULL; childIterNode = childIterNode->next ){
    if (childIterNode->type == XML_ELEMENT_NODE){
      if( strcmp( (char *)(childIterNode->name), "trkpt" ) != 0 && strcmp( (char *)(childIterNode->name), "trkseg" ) != 0 ){
        if( strcmp( (char *)(childIterNode->name), "name") == 0 ){
          // get the contents, realloc, then add the name
          contents = (char *) xmlNodeGetContent( childIterNode );
        
          returnTrack->name = realloc( returnTrack->name, strlen( contents ) + 1 );

          strcpy( returnTrack->name, contents );
      
        } else {
          contents = (char *) xmlNodeGetContent( childIterNode );

          GPXData * newData = malloc( sizeof(GPXData) + strlen(contents) + 1);

          strcpy( newData->name, (char *)(childIterNode->name) );
          strcpy( newData->value, contents );

          insertFront( returnTrack->otherData, newData );
        }
        xmlFree(contents);
      }
    }
  }
  return returnTrack;
}





// * * * * * * * * * * * * * * * * * * * * 
// *********  Other Functions  ***********
// * * * * * * * * * * * * * * * * * * * *

int compareWaypointList( List * firstList, List * secondList ){
  ListIterator firstIter = createIterator( firstList );
  ListIterator secondIter = createIterator( secondList );

  Waypoint * firstNode = nextElement( &firstIter );
  Waypoint * secondNode = nextElement( &secondIter );

  while( firstNode != NULL && secondNode != NULL ){
    if( compareWaypoints( firstNode, secondNode ) == 0 ){
      return 0;
    }
    firstNode = nextElement( &firstIter );
    secondNode = nextElement( &secondIter );
  }
  if( firstNode == NULL && secondNode == NULL ){
    return 1;
  } else {
    return 0;
  }
}

int compareOtherDataList( List * firstList, List * secondList ){
  ListIterator firstIter;
  ListIterator secondIter;

  GPXData * firstgpx;
  GPXData * secondgpx;

  // set the iterators for the other data list
  firstIter = createIterator( firstList );
  secondIter = createIterator( secondList );

  firstgpx = nextElement( &firstIter );
  secondgpx = nextElement( &secondIter );

  // loop will return 0 if a false comparison is reached
  while( firstgpx != NULL && secondgpx != NULL ){
    if( compareGpxData( firstgpx, secondgpx ) == 0 ){
      return 0;
    }
    firstgpx = nextElement( &firstIter );
    secondgpx = nextElement( &secondIter );
  }
  // check if they are both null because if they are not then theyre not equal
  if( firstgpx == NULL && secondgpx == NULL ){
    return 1;
  } else {
    return 0;
  }
}

int compareTrackSegList( List * firstList, List * secondList ){
  ListIterator firstIter;
  ListIterator secondIter;

  TrackSegment * firstTrackSeg;
  TrackSegment * secondTrackSeg;

  firstIter = createIterator( firstList );
  secondIter = createIterator( secondList );

  firstTrackSeg = nextElement( &firstIter );
  secondTrackSeg = nextElement( &secondIter );

  while( firstTrackSeg != NULL && secondTrackSeg != NULL ){

    if( compareTrackSegments( firstTrackSeg, secondTrackSeg ) == 0 ){
      return 0;
    }
    firstTrackSeg = nextElement( &firstIter );
    secondTrackSeg = nextElement( &secondIter );
  }
  if( firstTrackSeg == NULL && secondTrackSeg == NULL ){
    return 1;
  } else {
    return 0;
  }
}