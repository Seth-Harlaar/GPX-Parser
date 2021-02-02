#include "../include/GPXParser.h"
#include "../include/GPXhelpers.h"
#include <stdlib.h>



// * * * * * * * * * * * * * * * * * * * * 
// ********  GPXDoc Functions  ***********
// * * * * * * * * * * * * * * * * * * * *

// make the gpx doc
GPXdoc * createGPXdoc(char* fileName){

  // vars 
  GPXdoc * returnDoc = malloc( sizeof(GPXdoc) );
  xmlDoc * doc = NULL;
  xmlNode * headNode = NULL;

  // get doc and check if null
  doc = xmlReadFile(fileName, NULL, 0);

  if( doc == NULL ){
    printf("A GPX document could not be found with the filename given: %s", fileName);
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

  // free the struc itself
  free(doc);
}

// turn the gpx doc into a string
// the code for this function was heavily inspired by the structListDemo.c file
char* GPXdocToString(GPXdoc* doc){
  char * tempString;
  char * tempWptString;
  char * tempRouteString;

  // length will be very big
  int length = 0;

  // for the gpxDoc data
  length += (256 + 8 + 1);
  // add length for creator


  if( getLength( doc->waypoints ) != 0 ){
    tempWptString = toString( doc->waypoints );
  } else {
    tempWptString = malloc(sizeof(char) * 50);
    strcpy( tempWptString, "No Waypoints in this file\n" );
  }
  
  length += strlen( tempWptString );

  if( getLength( doc->routes) != 0 ){
    tempRouteString = toString( doc->routes );
  } else {
    tempRouteString = malloc(sizeof(char) * 50);
    strcpy( tempRouteString, "There are no routes in this file.\n");
  }
  length += strlen( tempRouteString );

  tempString = malloc(sizeof( char ) * length);

  // add the data
  sprintf(tempString, "GPX doc:\n Version: %.1f\n %s\n%s\n", doc->version, tempWptString, tempRouteString );

  // free strings used
  free( tempWptString );
  free( tempRouteString );

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
    strcpy( otherDataString, "\n    |->There is no other data for this waypoint\n");
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
  if( strcmp(firstWpt->name, secondWpt->name) == 0 ){
    if( firstWpt->longitude == secondWpt->longitude ){
      if( firstWpt->latitude == secondWpt->latitude ){

        // check all the other data to see if its the same as well
        // **** remember to check all the data ****
        return 1;
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

  sprintf(returnString, "Route, name: %s\n  Waypoints:\n%s  Other Data:\n%s", route->name, tempWptString, tempOtherDataString );

  free(tempOtherDataString);
  free(tempWptString);

  return returnString;
}

int compareRoutes(const void *first, const void *second){
  // actually add some comparison
  return 1;
}