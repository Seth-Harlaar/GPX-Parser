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

    xmlSchemaValidCtxtPtr ctxt2;

    // validate the doc
    ctxt2 = xmlSchemaNewValidCtxt(schema);
    xmlSchemaSetValidErrors(ctxt2, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    ret = xmlSchemaValidateDoc(ctxt2, doc);
    
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

    xmlSchemaFreeValidCtxt(ctxt2);

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
  xmlNs * newNameSpace;

  // make a new doc
  returnDoc = xmlNewDoc( BAD_CAST "1.0" );

  // make a new node, set its namespace, and set it to root of the doc
  rootNode = xmlNewNode( NULL , BAD_CAST "gpx" );
  
  newNameSpace = xmlNewNs( rootNode, BAD_CAST gpxDoc->namespace, NULL );
  xmlSetNs( rootNode, newNameSpace );

  xmlDocSetRootElement( returnDoc, rootNode );

  // ********* add all the root subdata here, creator, version, ns etc *********
  char * versionString = malloc( 50 );
  sprintf( versionString, "%0.1f", gpxDoc->version );
  xmlNewProp( rootNode, BAD_CAST "version", BAD_CAST versionString );

  free( versionString );

  xmlNewProp( rootNode, BAD_CAST "creator", BAD_CAST gpxDoc->creator );

  // add all the waypoints if there are some
  if( getLength( gpxDoc->waypoints ) != 0 ){
    addWaypointNodeList( rootNode, gpxDoc->waypoints, 0, newNameSpace );
  }

  // add the routes if there are some
  if( getLength( gpxDoc->routes ) != 0 ){
    addRouteNodeList( rootNode, gpxDoc->routes, newNameSpace );
  }

  // add all the tracks if there are some
  if( getLength( gpxDoc->tracks ) != 0 ){
    addTrackNodeList( rootNode, gpxDoc->tracks, newNameSpace );
  }

  return returnDoc;
}




// * * * * * * * * * * * * * * * * * * * * 
// ******* Supports for docToDoc *********
// * * * * * * * * * * * * * * * * * * * *

void addTrackNodeList( xmlNode * parentNode, List * tracks, xmlNs * ns){

  if( parentNode != NULL && tracks != NULL  && ns != NULL  ){
    Track * curTrack;
    TrackSegment * curTrackSeg;

    ListIterator listIter = createIterator( tracks );
    ListIterator trackSegIter;

    xmlNode * newTrackNode;
    xmlNode * newTrackSegNode;

    // loop through the track list
    for( curTrack = nextElement( &listIter ); curTrack != NULL; curTrack = nextElement( & listIter ) ){

      newTrackNode = xmlNewNode( ns, BAD_CAST "trk" );

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
          newTrackSegNode = xmlNewNode( ns, BAD_CAST "trkseg" );
          
          // there should always be waypoints, but check anyway
          if( getLength( curTrackSeg->waypoints ) != 0 ){
            addWaypointNodeList( newTrackSegNode, curTrackSeg->waypoints, 2, ns );
          }
          xmlAddChild( newTrackNode, newTrackSegNode );
        }
      }

      xmlAddChild( parentNode, newTrackNode );
    }
  }
}

void addRouteNodeList( xmlNode * parentNode, List * routes, xmlNs * ns ){

  if( parentNode != NULL && routes != NULL  && ns != NULL ){
    Route * curRoute;
    ListIterator listIter = createIterator( routes );
    
    xmlNode * newRouteNode;

    // for every route in the list 
    for( curRoute = nextElement( &listIter ); curRoute != NULL; curRoute = nextElement( &listIter ) ){

      newRouteNode = xmlNewNode( ns, BAD_CAST "rte" );

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
        addWaypointNodeList( newRouteNode, curRoute->waypoints, 1, ns );
      }
      
      

      xmlAddChild( parentNode, newRouteNode );
    }
  }
}


void addWaypointNodeList( xmlNode * parentNode, List * wpts, int mode, xmlNs * ns ){

  char * lat;
  char * lon;
  char * nameString;

  xmlNode * newWaypointNode;

  ListIterator wptIter = createIterator( wpts );
  Waypoint * curWpt;

  if( parentNode != NULL && wpts != NULL  && ns != NULL  ){
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
      
      newWaypointNode = xmlNewNode( ns, BAD_CAST nameString );

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







// * * * * * * * * * * * * * * * * * * * * 
// **** Supports for validateGPXDoc  *****
// * * * * * * * * * * * * * * * * * * * *


bool checkTrackList( List * trackList ){
  if( trackList == NULL ){
    return false;
  }

  bool dataReturn;
  bool wptReturn;

  ListIterator listIter = createIterator( trackList );
  Track * curTrack;

  ListIterator trackSegIter;
  TrackSegment * curTrackSeg;


  // loop through the track list
  for( curTrack = nextElement( &listIter ); curTrack != NULL; curTrack = nextElement( & listIter ) ){

    // check the name node if the name is not just an empty string
    if( curTrack->name == NULL ){
      return false;
    }
         
    // check the other data is there is any
    if( curTrack->otherData == NULL ){
      return false;
    } else {
      if( getLength( curTrack->otherData ) != 0 ){
        dataReturn = checkDataList( curTrack->otherData );
      
        if( !dataReturn ){
          return false;
        }
      }
    }

    // check the track segments if there are any
    if( curTrack->segments == NULL ){
      return false;
    } else {
      if( getLength( curTrack->segments) != 0 ){
        trackSegIter = createIterator( curTrack->segments );
        
        // for each track segment
        for( curTrackSeg = nextElement( &trackSegIter ); curTrackSeg != NULL; curTrackSeg = nextElement( &trackSegIter ) ){
          
          if( curTrackSeg->waypoints == NULL ){
            return false;
          } else {
            // there should always be waypoints, but check anyway
            if( getLength( curTrackSeg->waypoints ) != 0 ){
              wptReturn = checkWaypointList( curTrackSeg->waypoints );

              if( !wptReturn ){
                return false;
              }
            }
          }
        }
      }
    }
  }

  return true;
}


bool checkRouteList( List * routeList ){

  bool dataReturn;
  bool wptReturn;

  ListIterator routeIter;
  Route * route;

  if( routeList == NULL ){
    return false;
  }


  routeIter = createIterator( routeList );

  for( route = nextElement( &routeIter ); route != NULL; route = nextElement( &routeIter ) ){
    // check name
    if( route->name == NULL ){
      return false;
    }

    // check wpts list
    if( route->waypoints == NULL ){
      return false;
    } else {
      if( getLength( route->waypoints ) != 0 ){
        // check wpts
        wptReturn = checkWaypointList( route->waypoints );

        if( !wptReturn ){
          return false;
        }
      }
    }

    // check otherData list
    if( route->otherData == NULL ){
      return false;
    } else {
      if( getLength( route->otherData ) != 0 ){
        // check each otherData item
        dataReturn = checkDataList( route->otherData );

        if( !dataReturn ){
          return false;
        }
      }
    }
  }

  return true;
}


// validate waypoints, checks all waypoints in the list are within constraints of gpxParser.h description
bool checkWaypointList( List * wptList ){
  bool dataReturn;
  
  ListIterator wptIter;
  Waypoint * wpt;

  if( wptList == NULL ){
    return false;
  }

  if( getLength( wptList ) != 0 ){
    wptIter = createIterator( wptList );

    // check values for each waypoint
    for( wpt = nextElement( &wptIter ); wpt != NULL; wpt = nextElement( &wptIter ) ){
      // check name
      if( wpt->name == NULL ){
        return false;
      }

      // check otherData list
      if( wpt->otherData == NULL ){
        return false;
      } else {
        if( getLength( wpt->otherData ) != 0 ){
          // check each otherData item
          dataReturn = checkDataList( wpt->otherData );

          if( !dataReturn ){
            return false;
          }
        }
      }
    }
  } 

  return true;
}

bool checkDataList( List * dataList ){
  ListIterator dataIter;
  GPXData * data;

  if( dataList == NULL ){
    return false;
  }
  if( getLength( dataList ) != 0 ){
    dataIter = createIterator( dataList );

    for( data = nextElement( &dataIter ); data != NULL; data = nextElement( &dataIter ) ){
      if( strcmp( data->name, "") == 0 || strcmp( data->value, "") == 0 ){
        return false;
      }
    }
  }
  return true;
}

// * * * * * * * * * * * * * * * * * * * * 
// *****     A2 Mod 2 Helpers     ********
// * * * * * * * * * * * * * * * * * * * *

float haversine( float latitude1, float longitude1, float latitude2, float longitude2 ){

  float a;
  float c;
  float d;
  float R = 6371000;

  float lat1 = latitude1 * M_PI/180;
  float lat2 = latitude2 * M_PI/180;

  float deltaLats = ( latitude2 - latitude1 )*M_PI/180;
  float deltaLons = ( longitude2 - longitude1 )*M_PI/180;

  a = ( sin( deltaLats/2 )*sin( deltaLats/2 ) ) + ( cos( lat1 )*cos( lat2 )*sin( deltaLons/2 )*sin( deltaLons/2 ) );

  c = 2 * atan2( sqrt(a), sqrt( 1- a ) );

  d = R*c;

  return d;
}

float getLengthWaypoints( Waypoint * wpt1, Waypoint * wpt2 ){
  float length;

  if( wpt1 == NULL || wpt2 == NULL ){
    return 0;
  }


  length = haversine( wpt1->latitude, wpt1->longitude, wpt2->latitude, wpt2->longitude );

  return length;
}

float getLengthWaypointsList( List * wptsList ){
  float totalLength;

  Waypoint * wpt;
  Waypoint * wptNext;
  ListIterator wptIter;
  ListIterator secondIter;

  if( wptsList == NULL ){
    return 0;
  }

  if( getLength( wptsList )  <= 1 ){
    return 0;
  }

  // make two iterators, one for curwpt, and one for the next element
  wptIter = createIterator( wptsList );
  secondIter = createIterator( wptsList );

  // start with 0 length
  totalLength = 0;

  // get the first, and then the second wpt, should always be a second waypoint because length of list is checked before being passed in
  wptNext = nextElement( &secondIter );
  wptNext = nextElement( &secondIter );

  // loop through each waypoint
  for( wpt = nextElement( &wptIter ); wpt != NULL; wpt = nextElement( &wptIter ) ){

    // if there is a wpt after the current wpt
    if( wptNext != NULL ){
      // get length inbetween the wpt and the next one
      totalLength += getLengthWaypoints( wpt, wptNext );
    }
    wptNext = nextElement( & secondIter );
  }

  return totalLength;
}

// checks if Elelen and len are with delta of eachother 
bool compareLength( float EleLen, float len, float delta ){

  if( abs( EleLen - len ) <= delta ){
    return true;
  }
  return false;
}


// * * * * * * * * * * * * * * * * * * * * 
// ****    A3 wrapper functions      *****
// * * * * * * * * * * * * * * * * * * * *

char * gpxFileToJSON( char * fileName ){
  // get the file
  GPXdoc * doc = createValidGPXdoc( fileName, "gpx.xsd" );
  // get the json
  char * returnString = GPXtoJSON( doc );
  // free doc
  deleteGPXdoc( doc );
  // return json
  return returnString;
}


char * gpxRoutesToJSON( char * fileName ){
  // get the file
  GPXdoc * doc = createValidGPXdoc( fileName, "gpx.xsd");

  // get the routes
  char * returnString = routeListToJSON( doc->routes );

  // delete the doc
  deleteGPXdoc( doc );

  // return the routes
  return returnString;
}

char * gpxTracksToJSON( char * fileName ){
  // same as routes but tracks
  GPXdoc * doc = createValidGPXdoc( fileName, "gpx.xsd");

  // get the routes
  char * returnString = trackListToJSON( doc->tracks );

  // delete the doc
  deleteGPXdoc( doc );

  // return the routes
  return returnString;
}

char * otherDataToJSON( GPXData * data ){
  // name and value
  char * returnString = malloc( sizeof( char ) * (256 + strlen(data->value) + 1) );

  char * name = malloc( sizeof( char ) * ( 256 + 1 ) );
  strcpy( name, data->name );

  char * value = malloc( sizeof( char ) * ( strlen( data->value ) + 1 ) );
  strcpy( value, data->value );

  sprintf( returnString, "{\"name\":\"%s\",\"value\":\"%s\"}", name, value );

  free( name );
  free( value );

  return returnString;
}



char * otherDataListToJSON( const List * list ){
 
  char * returnString;

  GPXData * route;
  GPXData * secondRoute;
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
    char * tempRouteString = otherDataToJSON( route );

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


char * getOtherDataJSON( char * fileName, int route, char * componentName ){
  char * returnString;

  ListIterator iter;
  List * otherData = NULL;

  // validate file and make a gpx doc
  GPXdoc * doc = createValidGPXdoc( fileName, "gpx.xsd" );

  // validate the doc
  bool valid  = validateGPXDoc( doc , "gpx.xsd" );

  if( valid ){
    // check if route or track
    // find the component
    if( route == 1 ){
      Route * route;
      iter = createIterator( doc->routes );

      for( route = nextElement( &iter ); route != NULL; route = nextElement( &iter ) ){
        if( strcmp( route->name, componentName ) == 0 ){
          // get the otherData List
          otherData = route->otherData;
        }
      }

    // if its a track
    } else {
      Track * track;
      iter = createIterator( doc->tracks );

      for( track = nextElement( &iter ); track != NULL; track = nextElement( &iter ) ){
        if( strcmp( track->name, componentName ) == 0 ){
          // get the otherData List
          otherData = track->otherData;
        }
      }
    }

    // get the string
    if( otherData != NULL ){
      returnString = otherDataListToJSON( otherData );
    } else {
      printf(" null\n");
      return "not found";
    }

  } else {
    printf(" not valid\n ");
    deleteGPXdoc( doc );
    return "error";
  }

  deleteGPXdoc( doc );
  return returnString;
}


// take a JSON string and save the corresponding gpx file to /uploads
char * makeNewDoc( char * JSON, char * fileName ){
  
  if( JSON == NULL ){
    return "false";
  }
  
  // convert JSON to gpx -> JSONtoGPX()
  GPXdoc * doc = JSONtoGPX( JSON) ;

  // validate -> validate gpxDoc()
  bool valid = validateGPXDoc( doc, "gpx.xsd" );

  if( valid ){
    // save the xmltree to file -> writeGPXDoc()
    bool write = writeGPXdoc( doc, fileName );

    if( write ){
      return "true";
    } else {
      return "false write";
    }

  } else {
    return "false valid";
  }
}



// adding a new route to a file
char * addNewRoute( char * JSON, char * fileName ){
  if( fileName == NULL || JSON == NULL ){
    return "false";
  }
  
  bool success;
  // make a gpxDOc witht he fileName
  GPXdoc * doc = createValidGPXdoc( fileName, "gpx.xsd" );
  // validate
  bool valid = validateGPXDoc( doc, "gpx.xsd" );

  if( valid ){
    // turn the JSON to a route
    Route * newRoute = JSONtoRoute( JSON );

    // add the new route to the gpxDoc
    addRoute( doc, newRoute );

    bool valid = validateGPXDoc( doc, "gpx.xsd" );

    if( valid ){
      success = writeGPXdoc( doc, fileName );
    } else {
      success = false;
    }

    deleteGPXdoc( doc );

    if( valid && success ){
      return "true";
    } else {
      return "false";
    }
  } else {
    deleteGPXdoc( doc );
    return "false";
  }
}



// add a new wpt to a given route in a given file
char * addWptToRoute( char * JSON, char * fileName, char * routeName ){
  // make a new doc from file
  GPXdoc * doc = createValidGPXdoc( fileName, "gpx.xsd" );

  // validate it
  bool valid = validateGPXDoc( doc, "gpx.xsd" );

  if( valid ){
    // find the route
    Route * route = getRoute( doc, routeName );

    if( route == NULL ){
      return "false";
    }

    // make a new wpt from the json
    Waypoint * newWpt = JSONtoWaypoint( JSON );

    if( newWpt == NULL ){
      return "false";
    }

    // add the wpt to the route
    addWaypoint( route, newWpt );

    // revalidate
    valid = validateGPXDoc( doc, "gpx.xsd" );

    if( valid ){
      bool success = writeGPXdoc( doc, fileName );
      deleteGPXdoc( doc );
      if( success ){
        return "true";
      } else {
        return "false";
      }
    
    } else {
      return "false";
    }
  } else {
    return "false";
  }
}


char * renameRoute( char * oldName, char * fileName, char * newName){

  // make a doc
  GPXdoc * doc = createValidGPXdoc( fileName, "gpx.xsd" );

  // validate
  bool valid = validateGPXDoc( doc, "gpx.xsd" );

  if( !valid ){
    return "false";
  }

  // find the route
  Route * route = getRoute( doc, oldName );

  if( route == NULL ){
    return "false";
  }

  // change the name
  route->name = realloc( route->name, strlen( newName ) + 1 );

  strcpy( route->name, newName );

  // revalidate
  valid = validateGPXDoc( doc, "gpx.xsd" );

  if( !valid ){
    return "false";
  }

  // save the doc
  bool success = writeGPXdoc( doc, fileName );

  if( !success ){
    return "false";
  }

  // cleanup and return
  deleteGPXdoc( doc );
  return "true";
}



char * renameTrack( char * oldName, char * fileName, char * newName){

  // make a doc
  GPXdoc * doc = createValidGPXdoc( fileName, "gpx.xsd" );

  // validate
  bool valid = validateGPXDoc( doc, "gpx.xsd" );

  if( !valid ){
    return "false";
  }

  // find the track
  Track * track = getTrack( doc, oldName );

  if( track == NULL ){
    return "false";
  }

  // change the name
  track->name = realloc( track->name, strlen( newName ) + 1 );

  strcpy( track->name, newName );

  // revalidate
  valid = validateGPXDoc( doc, "gpx.xsd" );

  if( !valid ){
    return "false";
  }

  // save the doc
  bool success = writeGPXdoc( doc, fileName );

  if( !success ){
    return "false";
  }

  // cleanup and return
  deleteGPXdoc( doc );
  return "true";
}


char * routesBetweenToJSON( char * fileName, float lat1, float lon1, float lat2, float lon2,  float tol ){
  // make a doc out of the file
  GPXdoc * doc = createValidGPXdoc( fileName, "gpx.xsd" );

  // check valid
  bool valid = validateGPXDoc( doc, "gpx.xsd" );

  if( !valid ){
    return "{\"invalid\":\"true\"}";
  }

  // find routes between two points
  List * routes = getRoutesBetween( doc, lat1, lon1, lat2, lon2, tol );

  if( routes == NULL ){
    return "[]";
  }

  // convert list to JSON
  char * JSON = routeListToJSON( routes );

  if( JSON == NULL ){
    return "{\"invalid\":\"true\"}";
  }

  // return list
  return JSON;
}


char * tracksBetweenToJSON( char * fileName, float lat1, float lon1, float lat2, float lon2,  float tol ){
  // make a doc out of the file
  GPXdoc * doc = createValidGPXdoc( fileName, "gpx.xsd" );

  // check valid
  bool valid = validateGPXDoc( doc, "gpx.xsd" );

  if( !valid ){
    return "{\"invalid\":\"true\"}";
  }

  // find routes between two points
  List * tracks = getTracksBetween( doc, lat1, lon1, lat2, lon2, tol );

  if( tracks == NULL ){
    return "[]";
  }

  // convert list to JSON
  char * JSON = routeListToJSON( tracks );

  if( JSON == NULL ){
    return "{\"invalid\":\"true\"}";
  }

  // return list
  return JSON;
}


char * validateGPXFile( char * fileName ){
  GPXdoc * doc = createGPXdoc( fileName );
  
  if( doc != NULL ){
    bool valid = validateGPXDoc( doc, "gpx.xsd" );

    if( valid ){
      return "true";
    } else {
      return "false";
    }
  } else {
    return "false";
  }
}