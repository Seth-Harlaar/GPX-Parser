#include "../include/GPXParser.h"
#include "../include/GPXhelpers.h"


// * * * * * * * * * * * * * * * * * * * * 
// ******** Waypoint Functions ************
// * * * * * * * * * * * * * * * * * * * * 

void deleteWaypoint( void * data ){
  Waypoint * wpt = data;

  free(wpt->name);
  // free the list of other data
  free(wpt);
}

// return a string displaying the waypoint nicely
char * wayPointToString( void * data ){
  // 50 for name
  // 16 for lon/lat
  // add in the otherData
  int length = 66;
  char * wptString = malloc( sizeof(char) * length );

  Waypoint * wpt = data;

  sprintf(wptString, "  Waypoint, Name: %s\n    Lat: %0.3f, Lon: %0.3f \n", wpt->name, wpt->latitude, wpt->longitude);

  return wptString;
}

int compareWaypoints( const void * fist, const void * second ){
  Waypoint * firstWpt = first;
  Waypoint * secondWpt = second;

  // compare name, lon, lat
  if( strcmp(firstWpt->name, secondWpt->name) == 0 ){
    if( firstWpt->longitude == secondWpt->longitude ){
      if( firstWpt->latitude == secondWpt->latitude ){

        // check all the other data to see if its the same as well


        return 1;
      }
    }
  }
}

// each waypoint must be allocated with its own data, and then put into the linked list
List * getWaypointsList( List * wptList, xmlNode * headNode ){

  // initialize the list
  List * wptList = initializeList();
  // add all the waypoints to the list
  addWaypoints( wptList, headNode );

}

void addWaypoints( List * wptList, xmlNode * headNode ){
  xmlNode * iterator = NULL;

  // this loop taken from the libxmlexample.c file
  for(iterator = headNode; iterator != NULL; iterator = iterator->next){
    // check if its a waypoint
    if (iterator->type == XML_ELEMENT_NODE){
      if( strcmp( (char *)(iterator->name), "wpt" ) == 0 ){
        printf("found a waypoint\n");
        // the element is a waypoint
        // make a list element, parse the waypoint, add the data to the list element
        Waypoint * newWpt = malloc( sizeof(Waypoint) );
        
        parseWaypoint( newWpt, iterator );

        // add the waypoint to the list

      }  else {
        // search the children of the node
        addWaypoints( wptList, iterator->children );
      }

    }    
  }
}


// parse each waypoint and get the data out of them
void parseWaypoint( Waypoint * newWpt, xmlNode * curNode ){
  printf("  Parsing a waypoint\n");
  // fill the waypoint with data
  newWpt->name = malloc( sizeof(char) );
  strcpy(newWpt->name, "");

  // search the attributes of the waypoint
  xmlAttr * iterPoint = NULL;

  // this loop was taken from libxmlexample.c
  printf("    Searching for attributes of waypoint\n");
  for( iterPoint = curNode->properties; iterPoint != NULL; iterPoint = iterPoint->next){
    xmlNode * value = iterPoint->children;
    char * attrName = (char *) iterPoint->name;
    char * cont  = (char *)(value->content);

    // check if it is a lon or lan attribute, store the data if so 
    if( strcmp(attrName, "lon") == 0 ){
      printf("     ->Found lon\n");
      newWpt->longitude = strtod(cont, NULL);
    } else if( strcmp(attrName, "lat") == 0 ){
      printf("     ->Found lat\n");
      newWpt->latitude = strtod(cont, NULL);
    }
  }

  // search for children of the node
  xmlNode * childIterNode = NULL;

  for( childIterNode = curNode->children; childIterNode != NULL; childIterNode = childIterNode->next ){
    // if the child has a name of name, it is the name, get the contents
    if( strcmp( (char *)(childIterNode->name), "name") == 0 ){
      printf("     ->found a name\n");
      char * contents;
      // get the contents, realloc, then add the name
      contents = (char *) xmlNodeGetContent( childIterNode );
      if( contents != NULL ){
        printf("      ->Contents of name: %s\n", contents );
        
        int length = strlen( contents );
        newWpt->name = realloc( newWpt->name, length );

        strcpy( newWpt->name, contents );
      } else {
        printf("contents of name null\n");
      }
    } else {
      // make another list for other data
      newWpt->otherData = initializeList();
      // make space for the new data
      GPXdata * extraData = malloc( sizeof(GPXdata) );
      // get the name
      strcpy( GPXdata->name, (char *)(childIterNode->name));
      // get the data
    }
  }
}


// * * * * * * * * * * * * * * * * * * * * 
// ******** gpxData Functions ************
// * * * * * * * * * * * * * * * * * * * * 
void deleteGpxData( void * data ){
  GPXData * gpxData = data;

  free(gpxData);
}

char * gpxDataToString( void * data ){
  GPXdata * gpxData = data;
  int length = 256 + strlen(gpxData->value) + 50;

  char * returnString = malloc( sizeof(char) * length );

  sprintf(returnString, "      gpxData:\n        Name: %d\n        Value: %s\n ", gpxData->name, gpxData->value);

  return returnString;
}

int compareGpxData( const void * first, const void * second ){
  GPXdata * firstData = first;
  GPXdata * secondData = second;

  if( strcmp(first->name, second->name) == 0 ){
    if( strcmp(fist->value, second->value) == 0 ){
      return 1;
    }
  }
  return 0;
}