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

// waypoint functions
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


#endif