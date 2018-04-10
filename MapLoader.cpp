#include "provided.h"
#include "MyMap.h"
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
    
private:
    int m_segmentNum;
    vector<StreetSegment> m_mapData;
};

MapLoaderImpl::MapLoaderImpl()
{
    m_segmentNum = 0;
}

MapLoaderImpl::~MapLoaderImpl()
{
}

bool MapLoaderImpl::load(string mapFile)
{
    ifstream inf(mapFile);
    if ( ! inf)
    {
        cerr << "Error: Cannot open data.txt!" << endl;
        return false;
    }
    string s;
    while(getline(inf, s)){
        string startLatitiude, startLongitude, endLatitude, endLongitude;
        getline(inf, startLatitiude, ',');
        getline(inf>>ws, startLongitude, ' ');
        getline(inf, endLatitude, ',');
        getline(inf>>ws, endLongitude);
        
        GeoCoord geoStart(startLatitiude, startLongitude);
        GeoCoord geoEnd(endLatitude, endLongitude);
        StreetSegment *street = new StreetSegment{s, GeoSegment(geoStart, geoEnd), {}};
        
        int attractionNum = 0;
        inf >> attractionNum >> ws;
        
        for(int i = 0; i < attractionNum; i++)
        {
            string attractionName, attractionLat, attractionLon;
            getline(inf, attractionName, '|');
            getline(inf, attractionLat, ',');
            getline(inf, attractionLon);
            Attraction getAttraction{attractionName, {attractionLat, attractionLon}};
            street->attractions.push_back(getAttraction);
        }
        m_mapData.push_back(*street);
        m_segmentNum++;
    }
    return true;
}

size_t MapLoaderImpl::getNumSegments() const
{
	return m_segmentNum; // This compiles, but may not be correct
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
    if(segNum > getNumSegments() - 1)
        return false;
    else
        seg = m_mapData[segNum];
	return true;  // This compiles, but may not be correct
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
   return m_impl->getSegment(segNum, seg);
}
