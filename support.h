#ifndef SUPPORT_INCLUDED
#define SUPPORT_INCLUDED


#include "provided.h"
#include <cassert>
using namespace std;

inline
bool operator== (const GeoCoord &a, const GeoCoord &b)
{
    if (a.latitude == b.latitude && a.longitude == b.longitude)
        return true;
    return false;
}

inline
bool operator> (const GeoCoord &a, const GeoCoord &b)
{
    if (a.latitude > b.latitude)
        return true;
    else if (a.latitude == b.latitude && a.longitude > b.longitude)
        return true;
    return false;
}

inline
bool operator< (const GeoCoord &a, const GeoCoord &b)
{
    if (a.latitude < b.latitude)
        return true;
    else if (a.latitude == b.latitude && a.longitude < b.longitude)
        return true;
    return false;
}


struct geoCoordWithName
{
    geoCoordWithName(string name, GeoCoord a):m_name(name), m_coord(a){}
    string m_name;
    GeoCoord m_coord;
};

inline string getDirection(const double angle)
{
    if (angle >= 0 && angle <= 22.5)
        return "east";
    else if( angle > 22.5 && angle <= 67.5)
        return "northeast";
    else if(angle > 67.5 && angle <= 112.5)
        return "north";
    else if(angle > 112.5 && angle <= 157.5)
        return "northwest";
    else if(angle > 157.5 && angle <= 202.5)
        return "west";
    else if(angle > 202.5 && angle <= 247.5)
        return "southwest";
    else if(angle > 247.5 && angle <= 292.5)
        return "south";
    else if(angle > 292.5 && angle <= 337.5)
        return "southeast";
    else if(angle > 337.5 && angle < 360)
        return "east";
    else
        assert(false && "wrong angle");
}



#endif // SUPPORT_INCLUDED

