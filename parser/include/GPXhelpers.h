// Seth Harlaar -- 1109524

#ifndef GPX_HELPERS_H
#define GPX_HELPERS_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlschemastypes.h>
#include "LinkedListAPI.h"


List * getWaypointsList( xmlNode * headNode, int mode );
void addWaypoints( List * wptList, xmlNode * headNode, int mode );
void parseWaypoint( Waypoint * newWpt, xmlNode * curNode );

List * getRoutesList( xmlNode * headNode );
void addRoutes( List * routeList, xmlNode * headNode );
Route * parseRoute( xmlNode * curNode );

List * getTrackSegList( xmlNode * headNode );
void addTrackSegments( List * trackSegList, xmlNode * headNode );
TrackSegment * parseTrackSeg( xmlNode * curNode );

List * getTracksList( xmlNode * headNode );
void addTracks( List * trackList, xmlNode * headNode );
Track * parseTrack( xmlNode * curNode );

// extras
int compareWaypointList( List * firstList, List * secondList );
int compareOtherDataList( List * firstLIst, List * secondList );
int compareTrackSegList( List * firstList, List * secondList );


// A2 helpers
xmlDoc * docToDoc( GPXdoc * gpxDoc );

// supports for docToDoc
void addWaypointNodeList( xmlNode * parentNode, List * wpts, int mode, xmlNs * ns );
void addOtherDataNodeList( xmlNode * parentNode, List * dataList );
void addRouteNodeList( xmlNode * parentNode, List * routes, xmlNs * ns );
void addTrackNodeList( xmlNode * parentNode, List * tracks, xmlNs * ns);

bool validateTree( xmlDoc * doc, char * gpxSchemaFile );


// supports for validateGPXDoc
bool checkDataList( List * dataList );
bool checkWaypointList( List * wptList );
bool checkRouteList( List * routeList );
bool checkTrackList( List * trackList );


// A2 mod 2 helpers
float haversine( float latitude1, float longitude1, float latitude2, float longitude2 );

// length calcs 
float getLengthWaypoints( Waypoint * wpt1, Waypoint * wpt2 );
float getLengthWaypointsList( List * wptsList );

bool compareLength( float EleLen, float len, float delta );


// A3 functions
char * gpxFileToJSON( char * fileName );
char * gpxRoutesToJSON( char * fileName );
char * getOtherDataJSON( char * fileName, int route, char * componentName );
char * makeNewDoc( char * JSON, char * fileName ); 
char * addNewRoute( char * fileName, char * JSON );
char * addWptToRoute( char * fileName, char * JSON, char * routeName );
char * renameRoute( char * oldName, char * fileName, char * newName);
char * renameTrack( char * oldName, char * fileName, char * newName);
char * routesBetweenToJSON( char * fileName, float lat1, float lon1, float lat2, float lon2,  float tol );
char * tracksBetweenToJSON( char * fileName, float lat1, float lon1, float lat2, float lon2,  float tol );
char * validateGPXFile( char * fileName );
char * routeNameToWptJSON( char * fileName, char * routeName );
char * waypointListToJSON( List * wpts );
char * waypointToJSON( Waypoint * wpt );






#endif