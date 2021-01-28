#include "../include/GPXParser.h"
#include "../include/GPXhelpers.h"
#include <stdlib.h>

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
  wayPoints = getWaypointsList( headNode );
  returnDoc->waypoints = wayPoints;
  
  

  // list of routes -- cannot be null, may be empty

  // list of tracks -- cannot be null, may be empty

  xmlFreeDoc(doc);
  xmlCleanupParser();

  return returnDoc;
}

// get rid of the gpx doc
void deleteGPXdoc(GPXdoc* doc){
  // destroy all the data in there
  // free the creator value because its malloced
  
  // free all the lists and everything in them
  
  // free the struc itself
  free(doc);
}

// turn the gpx doc into a string
// the code for this function was heavily inspired by the structListDemo.c file
char* GPXdocToString(GPXdoc* doc){
  // so far the only items included in the string will be the version because that is the only 
  // thing that has been implemented so far, and even that is wrong, or is it
  char * tempString;

  // length will be very big
  int length = 0;

  // for the gpxDoc data
  length += (256 + 8);
  // add length for creator 

  // for waypoints
  if( getLength( doc->waypoints ) != 0 ){
    int wpthLength = calcWptLength( doc->waypoints );
    length += wpthLength;
  }

  tempString = malloc(sizeof( char ) * length);

  // add the doc data
  sprintf(tempString, "GPX doc:\n Version: %.1f\n", doc->version);
  
  // add the waypoint data
  if( getLength( doc->waypoints ) != 0 ){
    char * tempWptString; 
    tempWptString = toString( doc->waypoints );
    sprintf( tempString, "%s", tempWptString );
  }


  return tempString;
}