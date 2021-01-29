#include "../include/GPXParser.h"
#include "../include/GPXhelpers.h"



// * * * * * * * * * * * * * * * * * * * * 
// ******** Waypoint Functions ***********
// * * * * * * * * * * * * * * * * * * * *

// ******** Project Functions ************
void deleteWaypoint( void * data ){
  Waypoint * wpt = data;

  free(wpt->name);
  freeList(wpt->otherData);
  free(wpt);
}

// return a string displaying the waypoint nicely
char * waypointToString( void * data ){
  Waypoint * wpt = data;

  // ? for name, 16 for lon/lat, add in the otherData
  char * otherDataString = toString( wpt->otherData );
  
  int length = strlen(otherDataString) + 16 + strlen(wpt->name) + 50;

  char * wptString = malloc( sizeof(char) * length );

  sprintf(wptString, "  Waypoint, Name: %s\n  |->Lat: %0.3f, Lon: %0.3f \n%s\n", wpt->name, wpt->latitude, wpt->longitude, otherDataString);

  free(otherDataString);

  return wptString;
}

int compareWaypoints( const void * first, const void * second ){

  const Waypoint * firstWpt = first;
  const Waypoint * secondWpt = second;

  // compare name, lon, lat
  if( strcmp(firstWpt->name, secondWpt->name) == 0 ){
    if( firstWpt->longitude == secondWpt->longitude ){
      if( firstWpt->latitude == secondWpt->latitude ){

        // check all the other data to see if its the same as well
        return 1;
      }
    }
  }
  return 0;
}

// ******** Helper Functions ************

// each waypoint must be allocated with its own data, and then put into the linked list
List * getWaypointsList( xmlNode * headNode ){

  // initialize the list
  List * returnList = initializeList( waypointToString, deleteWaypoint, compareWaypoints );
  // add all the waypoints to the list
  addWaypoints( returnList, headNode );

  return returnList;
}

void addWaypoints( List * wptList, xmlNode * headNode ){
  xmlNode * iterator = NULL;

  // this loop taken from the libxmlexample.c file
  for(iterator = headNode; iterator != NULL; iterator = iterator->next){
    // check if its a waypoint
    if (iterator->type == XML_ELEMENT_NODE){
      if( strcmp( (char *)(iterator->name), "wpt" ) == 0 ){
        //printf("found a waypoint\n");
        // the element is a waypoint
        // make a list element, parse the waypoint, add the data to the list element
        Waypoint * newWpt = malloc( sizeof(Waypoint) );
        
        newWpt->otherData = initializeList(gpxDataToString, deleteGpxData, compareGpxData );

        parseWaypoint( newWpt, iterator );

        // add the waypoint to the list
        insertFront( wptList, newWpt );
      }  else {
        // search the children of the node
        addWaypoints( wptList, iterator->children );
      }
    }    
  }
}


// parse each waypoint and get the data out of them
void parseWaypoint( Waypoint * newWpt, xmlNode * curNode ){
  // fill the waypoint with data
  newWpt->name = malloc( sizeof(char) );
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


// * * * * * * * * * * * * * * * * * * * * 
// ******** gpxData Functions ************
// * * * * * * * * * * * * * * * * * * * * 

// project functions

void deleteGpxData( void * data ){
  GPXData * gpxData = data;

  free(gpxData);
}

char * gpxDataToString( void * data ){
  GPXData * gpxData = data;
  int length = 50 + 256 + strlen(gpxData->value) + 1;

  char * returnString = malloc( sizeof(char) * length );

  sprintf(returnString, "    |->gpxData, Name: %s, Value: %s ", gpxData->name, gpxData->value);

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
