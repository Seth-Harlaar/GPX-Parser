#include "../include/GPXParser.h"


GPXdoc * createGPXdoc(char* fileName){
  xmlDoc * doc = NULL;

  doc = xmlReadFile(fileName, NULL, 0);

  if( doc == NULL ){
    printf("A GPX document could not be found with the filename given: %s", fileName);
  }

  return doc;
}



char* GPXdocToString(GPXdoc* doc){
  char * z;
  return z;
}


void deleteGPXdoc(GPXdoc* doc){

}


//Total number of waypoints in the GPX file
int getNumWaypoints(const GPXdoc* doc){
  int z;
  return z;
}

//Total number of routes in the GPX file
int getNumRoutes(const GPXdoc* doc){
  int z;
  return z;
}

//Total number of tracks in the GPX file
int getNumTracks(const GPXdoc* doc){
  int z;
  return z;
}

//Total number of segments in all tracks in the document
int getNumSegments(const GPXdoc* doc){
  int z;
  return z;
}

//Total number of GPXData elements in the document
int getNumGPXData(const GPXdoc* doc){
  int z;
  return z;
}


// Function that returns a waypoint with the given name.  If more than one exists, return the first one.  
// Return NULL if the waypoint does not exist
Waypoint* getWaypoint(const GPXdoc* doc, char* name){
  Waypoint * z;
  return z;
}

// Function that returns a track with the given name.  If more than one exists, return the first one. 
// Return NULL if the track does not exist 
Track* getTrack(const GPXdoc* doc, char* name){
  Track * z;
  return z;
}

// Function that returns a route with the given name.  If more than one exists, return the first one.  
// Return NULL if the route does not exist
Route* getRoute(const GPXdoc* doc, char* name){
  Route * z;
  return z;
}



// GPX data
void deleteGpxData( void* data){

}
char* gpxDataToString( void* data){
  char * z;
  return z;
}
int compareGpxData(const void *first, const void *second){
  int z;
  return z;
}

// Waypoints
void deleteWaypoint(void* data){

}
char* waypointToString( void* data){
  char * z;
  return z;
}
int compareWaypoints(const void *first, const void *second){
  int z;
  return z;
}

// Routes
void deleteRoute(void* data){

}
char* routeToString(void* data){
  char * z;
  return z;
}
int compareRoutes(const void *first, const void *second){
  int z;
  return z;
}

// Track segments
void deleteTrackSegment(void* data){

}
char* trackSegmentToString(void* data){
  char * z;
  return z;
}
int compareTrackSegments(const void *first, const void *second){
  int z;
  return z;
}

// Tracks
void deleteTrack(void* data){
  
}
char* trackToString(void* data){
  char * z;
  return z;
}
int compareTracks(const void *first, const void *second){
  int z;
  return z;
}