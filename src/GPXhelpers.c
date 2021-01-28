#include "../include/GPXParser.h"


// get all the waypoints out of the gpx doc



// each waypoint must be allocated with its own data, and then put into the linked list
List * getWaypointsList( List * wptList, xmlNode * headNode ){

  // initialize the list

  // add all the waypoints to the list
  addWaypoints( wptList, headNode );

}

addWaypoints(List * wptList, xmlNode * headNode){
  xmlNode * iterator = NULL;

  // this loop taken from the libxmlexample.c file
  for(iterator = headNode; iterator != NULL; iterator = iterator->next){

    // check if its a waypoint
    // if it is, add it to the list
    if (iterator->type == XML_ELEMENT_NODE){
      if( strcmp(iterator->name, "wpt" ) == 0 ){
        // the element is a waypoint
        // make a list element, parse the waypoint, add the data to the list elemen
        // then add it to the list
        Waypoint * newWaypoint = malloc( sizeof(Waypoint) );
      }
    }

    //explore the children of the list
  }
}

// parse each waypoint and get the data out of them
parseWaypoint( Waypoint * newWpt, xmlNode * curNode ){
  // fill the waypoint with data
  newWpt->name = malloc( sizeof(char) );
  strcpy(newWpt, "");

  // search the attributes of the waypoint
  xmlAtrr * iterPoint = NULL;

  // this loop was taken from libxmlexample.c
  for( iterPoint = curNode->properties; iterPoint != NULL; iterPoint = iterPoint->next){
    xmlNode * value = attr->children;
    char * attrName = (char *) attr->name;
    char * cont  = (char *)(value->content);

    // check if it is a lon or lan attribute, store the data if so 
    if( strcmp(attrName, "lon") == 0 ){
      newWpt->longitude = strtod(cont, NULL);
    } else if( strcmp(attrName, "lan") == 0 ){
      newWpt->latitude = strtod(cont, NULL);
    }
  }

  // search for children of the 
}

// a way point follows this structure:
typedef struct {
  char* name;

  double longitude;

  double latitude;

  List* otherData;
} Waypoint;
