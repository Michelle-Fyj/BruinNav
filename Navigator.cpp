#include "provided.h"
#include <string>
#include <vector>
#include "MyMap.h"
#include "support.h"
#include <queue>
#include <cassert>
#include <iostream>

using namespace std;

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;

private:
    AttractionMapper *m_attractionMapper;
    SegmentMapper *m_segmentMapper;
    struct DistanceWithSegment
    {
        DistanceWithSegment(double distance, GeoCoord geoCoord):m_distance(distance), m_geoCoord(geoCoord){}
        double m_distance;
        GeoCoord m_geoCoord;
        bool operator<(DistanceWithSegment const& other) const {
            return m_distance > other.m_distance;

        }
    };
};

NavigatorImpl::NavigatorImpl()
{
    m_attractionMapper = new AttractionMapper;
    m_segmentMapper = new SegmentMapper;
}

NavigatorImpl::~NavigatorImpl()
{
    delete m_attractionMapper;
    delete m_segmentMapper;
}

bool NavigatorImpl::loadMapData(string mapFile)
{
    MapLoader ml;
    if (!ml.load(mapFile)) return false;
    m_attractionMapper->init(ml);
    m_segmentMapper->init(ml);
        return true;
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
    MyMap<GeoCoord, geoCoordWithName> m_map;
    
    
    priority_queue<DistanceWithSegment> m_queueStreet;
    // starting attraction name or address
    
    GeoCoord startLocation;
    if(!m_attractionMapper->getGeoCoord(start, startLocation))   // if start location is not found
            return NAV_BAD_SOURCE;
    m_map.associate(startLocation, geoCoordWithName("", startLocation));
    
    // if attraction is at the middle of a segment
    vector<StreetSegment> startingSegment = m_segmentMapper->getSegments(startLocation);
    vector<StreetSegment>::iterator it = startingSegment.begin();
    while(it != startingSegment.end())
    {
        vector<Attraction> att = it->attractions;
        vector<Attraction>::iterator attit = att.begin();
        while(attit != att.end())
        {
            if(attit->name == start)
            {
                double disStart = distanceEarthKM(startLocation, it->segment.start);
                m_queueStreet.push(DistanceWithSegment(disStart, it->segment.start));
                m_map.associate(it->segment.start, geoCoordWithName(it->streetName, startLocation));
                double disEnd = distanceEarthKM(startLocation, it->segment.end);
                m_queueStreet.push(DistanceWithSegment(disEnd, it->segment.end));
                m_map.associate(it->segment.end, geoCoordWithName(it->streetName, startLocation));
            }
            attit++;
        }
        it++;
    }
    
    // if attraction is at the end points of a segment
    while(it != startingSegment.end())
    {
        if(it->segment.end == startLocation)
        {
            m_queueStreet.push(DistanceWithSegment(0, it->segment.end));
            fprintf(stderr, "1. Associating my map {%s,%s} -> {%s,%s}\n",
                    it->segment.end.latitudeText.c_str(),
                    it->segment.end.longitudeText.c_str(),
                    startLocation.latitudeText.c_str(),
                    startLocation.longitudeText.c_str());
            m_map.associate(it->segment.end, geoCoordWithName("", startLocation));
        }
        if(it->segment.start == startLocation)
        {
            m_queueStreet.push(DistanceWithSegment(0, it->segment.start));
            fprintf(stderr, "2. Associating my map {%s,%s} -> {%s,%s}\n",
                    it->segment.start.latitudeText.c_str(),
                    it->segment.start.longitudeText.c_str(),
                    startLocation.latitudeText.c_str(),
                    startLocation.longitudeText.c_str());

            m_map.associate(it->segment.start, geoCoordWithName("", startLocation));
        }
    }
    
    
    GeoCoord endLocation;
    vector<geoCoordWithName> possibleEndingSegmentPoints;
    if(!m_attractionMapper->getGeoCoord(end, endLocation))      // if end location is not found
            return NAV_BAD_DESTINATION;
    
    
    vector<StreetSegment> endingSegment = m_segmentMapper->getSegments(endLocation);
    vector<StreetSegment>::iterator iterate = endingSegment.begin();
    while(iterate != endingSegment.end())
    {
        vector<Attraction> att = iterate->attractions;
        vector<Attraction>::iterator attit = att.begin();
        while(attit != att.end())
        {
            if(attit->name == end)
            {
                possibleEndingSegmentPoints.push_back(geoCoordWithName(iterate->streetName, iterate->segment.start));
                possibleEndingSegmentPoints.push_back(geoCoordWithName(iterate->streetName, iterate->segment.end));
            }
            attit++;
        }
        iterate++;
    }
    for (auto const& pesp : possibleEndingSegmentPoints) {
        cerr << "possible ending segment latitue: "<<pesp.m_coord.latitude << "; longitude: " << pesp.m_coord.longitude << endl;
    }
    
    MyMap<GeoCoord, bool>visited;
    
    
    string endStreetName;
    
    for(;;)
    {
        if(m_queueStreet.empty())
            return NAV_NO_ROUTE;
        DistanceWithSegment temp = m_queueStreet.top();
        
        cerr << temp.m_distance << endl;
        cerr << "temp geoCoord latitude: " << temp.m_geoCoord.latitude << "; temp geoCoord longitude: " << temp.m_geoCoord.longitude << endl;
        
        m_queueStreet.pop();
        
        visited.associate(temp.m_geoCoord, true);
        
        // if attraction is at the end points of a segment
        if (temp.m_geoCoord == endLocation) {
            endStreetName = m_map.find(temp.m_geoCoord)->m_name;
            break;  // if reach the destination, do something
            cerr<< "end street name is: " << endStreetName << endl;
        }
        
        // if the point is at a street segment with the destination at the middle
        {
            auto p = find_if(possibleEndingSegmentPoints.begin(), possibleEndingSegmentPoints.end(), [&temp] (geoCoordWithName const& gc) { return gc.m_coord == temp.m_geoCoord;});
            if (p != possibleEndingSegmentPoints.end())
                
            {
                fprintf(stderr, "3. Associating my map {%s,%s} -> {%s,%s}\n",
                        endLocation.latitudeText.c_str(),
                        endLocation.longitudeText.c_str(),
                        temp.m_geoCoord.latitudeText.c_str(),
                        temp.m_geoCoord.longitudeText.c_str());

                m_map.associate(endLocation, geoCoordWithName(p->m_name, temp.m_geoCoord));
                endStreetName = p->m_name;
                cerr<< "end street name is: " << endStreetName << endl;
                break;
            }
        }
        
        // if destination is not reached, find all other street segements that contains this geocoord
        vector<StreetSegment> segments = m_segmentMapper->getSegments(temp.m_geoCoord);
        vector<StreetSegment>::iterator it = segments.begin();
        while(it != segments.end())
        {
            GeoCoord a = it->segment.start;
            GeoCoord b = it->segment.end;
            GeoCoord differentGeoCoord;
            if(a == temp.m_geoCoord)
                differentGeoCoord = b;
            else if (b == temp.m_geoCoord)
                differentGeoCoord = a;
            else assert(false);
            
            if(visited.find(differentGeoCoord)) // if we have already visited that point, move to the next segment
            {
                it++;
                continue;
            }
            double dis = temp.m_distance+distanceEarthKM(temp.m_geoCoord, differentGeoCoord);
            DistanceWithSegment distance_with_segment_new(dis, differentGeoCoord);
            m_queueStreet.push(distance_with_segment_new);
            fprintf(stderr, "4. Associating my map {%.6f,%.6f} -> {%.6f,%.6f}\n",
                    differentGeoCoord.latitude,
                    differentGeoCoord.longitude,
                    temp.m_geoCoord.latitude,
                    temp.m_geoCoord.longitude);

            m_map.associate(differentGeoCoord, geoCoordWithName(it->streetName, temp.m_geoCoord));

            it++;
        }
        
    }
    


//    
//    vector<StreetSegment> route;
//    GeoCoord point;
//    GeoCoord connectingPoint = *m_map.find(endLocation);
//    
//    for(int i = 0; i < m_mapLoader->getNumSegments(); i++)
//    {
//        StreetSegment temp;
//        m_mapLoader->getSegment(i, temp);
//        if((temp.segment.start == connectingPoint && temp.segment.end == endLocation) ||
//           (temp.segment.end == connectingPoint && temp.segment.start == endLocation))
//        {
//            route.push_back(temp);
//            point = connectingPoint;
//        }
//    }
//    
//    for(;;)
//    {
//        if(point == startLocation)
//            break;
//        StreetSegment current = route.front();
//        GeoCoord currentConnectingPoint = *m_map.find(currentConnectingPoint);
//        for(int i = 0; i < m_mapLoader->getNumSegments(); i++)
//        {
//            StreetSegment temp;
//            m_mapLoader->getSegment(i, temp);
//            if((temp.segment.start == currentConnectingPoint && temp.segment.end == point) ||
//               (temp.segment.end == currentConnectingPoint && temp.segment.start == point))
//                route.push_back(temp);
//        }
//    }
    
//    reverse(route.begin(), route.end());
    
    directions.clear();
    GeoCoord current = endLocation;
    string currentStreeName = endStreetName;
    GeoCoord previous = endLocation;
    
    while (!(current == startLocation))
    {
        cerr << "New iteration\n\n\ncurrent = " << current.latitudeText << ", " << current.longitudeText << endl;
        
     // get the geoCoord to the next point
        GeoCoord nextCoord = m_map.find(current)->m_coord;
        
        //get the name of the street
        string nextStreetName = m_map.find(current)->m_name;
        cerr << "next street name: " << nextStreetName << endl;
        
        // get the distance
        double currentDistance = distanceEarthMiles(current, nextCoord);
        cerr << "distance: " << currentDistance << endl;
        
        // get direction
        GeoSegment segmentBetweenPoints(nextCoord, current);
        string direction = getDirection(angleOfLine(segmentBetweenPoints));
        cerr << "direction: " << direction << endl;
        
        
        if(currentStreeName !=  nextStreetName)
        {
            GeoCoord secondNextCoord = m_map.find(nextCoord)->m_coord;
            GeoSegment nextSegment(nextCoord, secondNextCoord);
            double turnAngle = angleBetween2Lines(nextSegment, segmentBetweenPoints);
            string turnDirection;
            if (turnAngle< 180)
                turnDirection = "left";
            else if(turnAngle >= 180)
                turnDirection = "right";
            NavSegment turnSegment(turnDirection, nextStreetName);
            directions.push_back(turnSegment);
            cerr << "turning street is called: " << nextStreetName << endl;
            cerr << "with turning angle: "<< turnAngle  << endl;
        }
        
        // proceed
        NavSegment proceedSegment(direction, nextStreetName, currentDistance, segmentBetweenPoints);
        directions.push_back(proceedSegment);
        
        //turn
        

        
        current = nextCoord;
        currentStreeName = nextStreetName;
        cerr << "current = " << current.latitudeText << ", " << current.longitudeText << endl;

    }
    
    
    reverse(directions.begin(), directions.end());
    return NAV_SUCCESS;
}


//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}
