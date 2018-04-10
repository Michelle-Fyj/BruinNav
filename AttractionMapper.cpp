#include "provided.h"
#include <string>
#include "MyMap.h"
#include <cctype>
using namespace std;

class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
    MyMap<string, GeoCoord> m_map;
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
    for (int i = 0; i < ml.getNumSegments(); i++)
    {
        StreetSegment temp;
        ml.getSegment(i, temp);
        vector<Attraction>::iterator it = temp.attractions.begin();
        while(it != temp.attractions.end()){
            for(int i = 0; i < it->name.size(); i++)
                it->name[i] = tolower(it->name[i]);
            m_map.associate(it->name, it->geocoordinates);
            it++;
        }
    }
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
    for (int i = 0; i < attraction.size(); i++)
        attraction[i] = tolower(attraction[i]);
    if (m_map.find(attraction) != nullptr)
    {
        gc = *m_map.find(attraction);
        return true;
    }
	return false;  // This compiles, but may not be correct
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}
