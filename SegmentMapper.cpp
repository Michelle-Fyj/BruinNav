#include "provided.h"
#include <vector>
#include "MyMap.h"
#include "support.h"
using namespace std;

class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
    
private:
    MyMap<GeoCoord, vector<StreetSegment>> m_map;
};

SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
    for (int i = 0; i < ml.getNumSegments(); i++)
    {
        StreetSegment tempSegment;
        
        ml.getSegment(i, tempSegment);
        vector<GeoCoord> m_storeGeoCoord;


        m_storeGeoCoord.push_back(tempSegment.segment.start);
        m_storeGeoCoord.push_back(tempSegment.segment.end);
        vector<Attraction>::iterator tempAttr = tempSegment.attractions.begin();
        while(tempAttr != tempSegment.attractions.end())
        {
            m_storeGeoCoord.push_back(tempAttr->geocoordinates);
            tempAttr++;
        }

        for (int j = 0; j < m_storeGeoCoord.size(); j++)
        {
            if (m_map.find(m_storeGeoCoord[j]) == nullptr)
            {
                vector<StreetSegment> segment;
                segment.push_back(tempSegment);
                m_map.associate(m_storeGeoCoord[j], segment);
            }
            else
            {
                vector<StreetSegment>& segment = *m_map.find(m_storeGeoCoord[j]);
                segment.push_back(tempSegment);
            }
        }
    }
}


vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
	vector<StreetSegment> segments;
    segments = *m_map.find(gc);
	return segments;  // This compiles, but may not be correct
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
