// Seth Harlaar -- 1109524

#include "GPXParser.h"
#include "GPXhelpers.h"


// if an error is encountered, NULL will be returned

// * * * * * * * * * * * * * * * * * * * * 
// ******** Waypoint Functions ***********
// * * * * * * * * * * * * * * * * * * * *

// start the list, run the function to add the waypoints
List * getWaypointsList( xmlNode * headNode, int mode ){
  if( headNode == NULL ){
    return NULL;
  }
  // initialize the list
  List * returnList = initializeList( waypointToString, deleteWaypoint, compareWaypoints );

  if( returnList == NULL  ){
    return NULL;
  }
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

        insertBack( wptList, newWpt );
      }  else if( strcmp((char *)(iterator->name), "rtept") == 0 && mode == 1 ){
        Waypoint * newWpt = malloc( sizeof(Waypoint) );

        newWpt->otherData = initializeList(gpxDataToString, deleteGpxData, compareGpxData );

        parseWaypoint( newWpt, iterator );

        insertBack( wptList, newWpt );
        
      } else if( strcmp((char *)(iterator->name), "trkpt") == 0 && mode == 2 ){
        Waypoint * newWpt = malloc( sizeof(Waypoint) );

        newWpt->otherData = initializeList(gpxDataToString, deleteGpxData, compareGpxData );
        
        parseWaypoint( newWpt, iterator );

        insertBack( wptList, newWpt );
      
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
        insertBack( newWpt->otherData, newData );
      }
      xmlFree(contents);
    }
  }
}






// * * * * * * * * * * * * * * * * * * * * 
// ********  Route Functions  ************
// * * * * * * * * * * * * * * * * * * * *

List * getRoutesList( xmlNode * headNode ){
  if( headNode == NULL ){
    return NULL;
  }
  // initialize list
  List * returnList = initializeList( routeToString, deleteRoute, compareWaypoints );

  if( returnList == NULL ){
    return NULL;
  }

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

        insertBack( routeList, newRoute );
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
  returnRoute->waypoints = getWaypointsList( curNode->children, 1 );

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

          insertBack( returnRoute->otherData, newData );
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
  if( headNode == NULL ){
    return NULL;
  }

  List * trackSegList = initializeList( trackSegmentToString, deleteTrackSegment, compareTrackSegments );

  if( trackSegList == NULL ){
    return NULL;
  }

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

        insertBack( trackSegList, newTrackSeg );
      } else {
        addTrackSegments( trackSegList, iterator->children );
      }
    }
  }

}

TrackSegment * parseTrackSeg( xmlNode * curNode ){
  // track segment is just a list of waypoints
  TrackSegment * returnTrackSeg = malloc( sizeof(TrackSegment) );

  returnTrackSeg->waypoints = getWaypointsList( curNode->children, 2 );

  return returnTrackSeg;
}



// * * * * * * * * * * * * * * * * * * * * 
// *********  Track Functions  ***********
// * * * * * * * * * * * * * * * * * * * *

List * getTracksList( xmlNode * headNode ){
  if( headNode == NULL ){
    return NULL;
  }

  List * trackList = initializeList( trackToString, deleteTrack, compareTracks );

  if( trackList == NULL ){
    return NULL;
  }

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

        insertBack( trackList, newTrack );
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
  returnTrack->segments = getTrackSegList( curNode->children );

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

          insertBack( returnTrack->otherData, newData );
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













// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// *************************                        Assignment 2 Helpers                         *************************
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

// validates a libxml tree
bool validateTree( xmlDoc * doc, char * gpxSchemaFile ){
  bool returnValue;
  
  int ret;

  GPXdoc * returnDoc;

  if( doc == NULL ){
    returnValue = false;

  } else {

    // checks the validity of the file -- the code for this functionality was taken from one of the links provided by the professor in the 
    // assignment description. Also found here: http://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;

    xmlLineNumbersDefault(1);

    ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

    xmlSchemaValidCtxtPtr ctxt;

    // validate the doc
    ctxt = xmlSchemaNewValidCtxt(schema);
    xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    ret = xmlSchemaValidateDoc(ctxt, doc);
    
    // check result of validation test
    if (ret == 0){
      // pass
      returnValue = true;
    
    } else if (ret > 0) {
      // fail
      returnValue = false;

    } else {
      // error
      returnValue = false;
    }

    xmlSchemaFreeValidCtxt(ctxt);
    xmlFreeDoc(doc);

    // free the resource
    if(schema != NULL)
    xmlSchemaFree(schema);

    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();
  }
  
  return returnValue;

}

// converts a GPXdoc to an xmlDoc
xmlDoc * docToDoc( GPXdoc * gpxDoc ){
  
  if( gpxDoc == NULL ){
    return NULL;
  }

  xmlDoc * returnDoc = NULL;
  xmlNode * rootNode = NULL;

  returnDoc = xmlNewDoc( BAD_CAST "1.0" );

  rootNode = xmlNewNode( NULL, BAD_CAST "gpx" );
  xmlDocSetRootElement( returnDoc, rootNode );

  // ********* add all the root subdata here, creator, version, ns etc *********


  // add all the waypoints if there are some
  if( getLength( gpxDoc->waypoints ) != 0 ){
    addWaypointNodeList( rootNode, gpxDoc->waypoints, 0 );
  }

  // add the routes if there are some
  if( getLength( gpxDoc->routes ) != 0 ){
    addRouteNodeList( rootNode, gpxDoc->routes );
  }

  // add all the tracks if there are some
  if( getLength( gpxDoc->tracks ) != 0 ){
    addTrackNodeList( rootNode, gpxDoc->tracks );
  }

  return returnDoc;
}

void addTrackNodeList( xmlNode * parentNode, List * tracks){

  if( parentNode != NULL && tracks != NULL ){
    Track * curTrack;
    TrackSegment * curTrackSeg;

    ListIterator listIter = createIterator( tracks );
    ListIterator trackSegIter;

    xmlNode * newTrackNode;
    xmlNode * newTrackSegNode;

    // loop through the track list
    for( curTrack = nextElement( &listIter ); curTrack != NULL; curTrack = nextElement( & listIter ) ){

      newTrackNode = xmlNewNode( NULL, BAD_CAST "trk" );

      // add the name node if the name is not just an empty string
      if( strcmp( curTrack->name, "" ) != 0 ){
        xmlNewChild( newTrackNode, NULL, BAD_CAST "name", BAD_CAST curTrack->name );
      }      

      // add the other data is there is any
      if( getLength( curTrack->otherData ) != 0 ){
        addOtherDataNodeList( newTrackNode, curTrack->otherData );
      }

      // add the track segments if there are any
      if( getLength( curTrack->segments) != 0 ){
        
        trackSegIter = createIterator( curTrack->segments );
        
        // for each track segment
        for( curTrackSeg = nextElement( &trackSegIter ); curTrackSeg != NULL; curTrackSeg = nextElement( &trackSegIter ) ){
          newTrackSegNode = xmlNewNode( NULL, BAD_CAST "trk" );
          
          // there should always be waypoints, but check anyway
          if( getLength( curTrackSeg->waypoints ) != 0 ){
            addWaypointNodeList( newTrackSegNode, curTrackSeg->waypoints, 2 );
          }
          xmlAddChild( newTrackNode, newTrackSegNode );
        }
      }

      xmlAddChild( parentNode, newTrackNode );
    }
  }
}

void addRouteNodeList( xmlNode * parentNode, List * routes ){
  
  if( parentNode != NULL && routes != NULL ){
    Route * curRoute;
    ListIterator listIter = createIterator( routes );
    
    xmlNode * newRouteNode;

    // for every route in the list 
    for( curRoute = nextElement( &listIter ); curRoute != NULL; curRoute = nextElement( &listIter ) ){

      newRouteNode = xmlNewNode( NULL, BAD_CAST "route" );

      // add the name node if its not an empty string
      if( strcmp( curRoute->name, "") != 0 ){
        xmlNewChild( newRouteNode, NULL, BAD_CAST "name", BAD_CAST curRoute->name );
      }
      
      // add the list of other data
      if( getLength( curRoute->otherData ) != 0 ){
        addOtherDataNodeList( newRouteNode, curRoute->otherData );
      }

      // add the list of waypoints if there are any
      if( getLength( curRoute->waypoints ) != 0 ){
        addWaypointNodeList( newRouteNode, curRoute->waypoints, 1 );
      }

      xmlAddChild( parentNode, newRouteNode );
    }
  }
}


void addWaypointNodeList( xmlNode * parentNode, List * wpts, int mode ){

  char * lat;
  char * lon;
  char * nameString;

  xmlNode * newWaypointNode;

  ListIterator wptIter = createIterator( wpts );
  Waypoint * curWpt;

  if( parentNode != NULL && wpts != NULL ){
    nameString = malloc( sizeof( char ) * 10 );
    if( mode == 0 ){
      sprintf( nameString, "wpt" );
    } else if( mode == 1 ) {
      sprintf( nameString, "rtept" );
    } else {
      sprintf( nameString, "trkpt" );
    }


    // for each wpt in the list make a new node and add it to the parent
    for( curWpt = nextElement( &wptIter ); curWpt != NULL; curWpt = nextElement( &wptIter ) ){
      

      newWaypointNode = xmlNewNode( NULL, BAD_CAST nameString );

      // get the lat and lon
      lat = malloc( 50 );
      lon = malloc( 50 );

      sprintf( lat, "%f", curWpt->latitude );
      sprintf( lon, "%f", curWpt->longitude );

      // add them as properties
      xmlNewProp( newWaypointNode, BAD_CAST "lat", BAD_CAST lat );
      xmlNewProp( newWaypointNode, BAD_CAST "lon", BAD_CAST lon );

      // add the other data for the waypoint
      if( getLength( curWpt->otherData ) != 0 ){
        addOtherDataNodeList( newWaypointNode, curWpt->otherData );
      }

      // add an extra node for the name
      if( strcmp( curWpt->name, "") != 0 ){
        xmlNewChild( newWaypointNode, NULL, BAD_CAST "name", BAD_CAST curWpt->name );
      }

      xmlAddChild( parentNode, newWaypointNode );

      free( lat );
      free( lon );
    }
    free( nameString );
  }
}

void addOtherDataNodeList( xmlNode * parentNode, List * dataList ){

  // if the args arent null, add every otherData item to the parent node provided
  if( parentNode != NULL && dataList != NULL ){
    GPXData * curData;

    ListIterator dataIter = createIterator( dataList );

    for( curData = nextElement( &dataIter ); curData != NULL; curData = nextElement( &dataIter ) ){
      xmlNewChild( parentNode, NULL, BAD_CAST curData->name, BAD_CAST curData->value );
    }
  }
}
