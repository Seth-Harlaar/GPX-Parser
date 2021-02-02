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
int calcWptsLength( List * waypoints );
int calcWptLength( Waypoint * wpt );

List * getRoutesList( xmlNode * headNode );
void addRoutes( List * routeList, xmlNode * headNode );
Route * parseRoute( xmlNode * curNode );



#endif