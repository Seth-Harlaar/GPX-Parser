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
List * getWaypointsList( xmlNode * headNode );
void addWaypoints( List * wptList, xmlNode * headNode );
void parseWaypoint( Waypoint * newWpt, xmlNode * curNode );
int calcWptLength( List * waypoints );




#endif