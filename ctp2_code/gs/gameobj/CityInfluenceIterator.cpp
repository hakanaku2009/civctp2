//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : City influence handling.
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
// 
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected wrap detection.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "MapPoint.h"
#include "CityInfluenceIterator.h"

#include "CityRadius.h"
#include "World.h"
#include "Cell.h"
#include "CitySizeRecord.h"

#include "directions.h"
#include "UnitData.h"

#include "network.h"

#include "ConstDB.h"
#include "TiledMap.h"

#if !defined(ACTIVISION_ORIGINAL)
namespace
{

//----------------------------------------------------------------------------
//
// Name       : ClearScratch
//
// Description: Apply SetScratch(0) to a block of tiles around a location.
//
// Parameters : a_Center	: center of the block
//				a_Radius	: radius of the block
//
// Globals    : g_theWorld
//
// Returns    : -
//
// Remark(s)  : The block is a square, tilted diagonally.
//
//----------------------------------------------------------------------------
void ClearScratch(MapPoint const & a_Center, size_t const a_Radius)
{
	for (size_t row = 0; row <= (2 * a_Radius); ++row) 
	{
		OrthogonalPoint	testXY(a_Center);
		testXY.Move(WEST, a_Radius);
		testXY.Move(SOUTHEAST, row);

		for (size_t column = 0; column <= (2 * a_Radius); ++column) 
		{
			if (testXY.IsValid())
			{
				g_theWorld->GetCell(testXY.GetRC())->SetScratch(0);
			}
			testXY.Move(NORTHEAST);
		}
	}
};

}; // namespace

#endif	// ACTIVISION_ORIGINAL

CityInfluenceIterator::CityInfluenceIterator(const MapPoint &center, sint32 size)
#if !defined(ACTIVISION_ORIGINAL)
:	m_testXY(center),
	m_wrappedCur(center)
#endif
{
	Init(center, size);
}

CityInfluenceIterator::CityInfluenceIterator()
#if !defined(ACTIVISION_ORIGINAL)
:	m_testXY(MapPoint()),
	m_wrappedCur()
#endif
{
	m_intRadius = 0;
	m_cur.Set(-1,-1);
	m_cityId = 0;
}

void CityInfluenceIterator::Init(const MapPoint &center, sint32 size)
{
	m_center = center;
	if(size >= g_theCitySizeDB->NumRecords()) {
		size = g_theCitySizeDB->NumRecords() - 1;
	}

	m_cur.Set(-1,-1);
	m_cityId = g_theWorld->GetCell(center)->GetCityOwner().m_id;
	m_intRadius = (sint16)g_theCitySizeDB->Get(size)->GetIntRadius();
}

void CityInfluenceIterator::Start()
{
	m_startX = m_center.x - m_intRadius;
	m_endX = m_center.x + m_intRadius;
	m_cur.x = m_startX;
	m_row = 0;
	m_cur.y = m_center.y + m_row;

#if defined(ACTIVISION_ORIGINAL)	// incorrect wrap computation
	if(!WrapPoint(m_center, m_cur, m_wrappedCur) ||
	   g_theWorld->GetCell(m_wrappedCur)->GetCityOwner().m_id != m_cityId) {
		
		Next();
	}
#else
	m_testXY = OrthogonalPoint(m_center);
	m_testXY.Move(WEST, m_intRadius);
	if (m_testXY.IsValid() &&
		(g_theWorld->GetCell(m_testXY.GetRC())->GetCityOwner().m_id == m_cityId)
	   )
	{
		// Point is on the map, and has not been taken by another city.
		m_wrappedCur = m_testXY.GetRC();
	}
	else
	{
		Next();
	}
#endif
}

bool CityInfluenceIterator::End()
{
	return m_row > (2 * m_intRadius);
}

void CityInfluenceIterator::Next()
{
#if defined(ACTIVISION_ORIGINAL)
	do {
		m_cur.x++;
		m_cur.y--;
		if(m_cur.x > m_endX) {
			m_row++;
			m_cur.x = m_startX;
			m_cur.y = m_center.y + m_row;
		}
	} while (((!WrapPoint(m_center, m_cur, m_wrappedCur)) ||
			  g_theWorld->GetCell(m_wrappedCur)->GetCityOwner().m_id != m_cityId) &&
			 !End());
#else
	bool	isValid	= false;
	
	while (!isValid && !End())  
	{
		m_cur.x++;
		m_cur.y--;
		m_testXY.Move(NORTHEAST);

		if (m_cur.x > m_endX) 
		{
			m_row++;
			m_cur.x = m_startX;
			m_cur.y = m_center.y + m_row;
			m_testXY.Move(SOUTH);
			m_testXY.Move(SOUTHWEST, m_endX - m_startX);
		}

		isValid = m_testXY.IsValid() && 
			      (g_theWorld->GetCell(m_testXY.GetRC())->GetCityOwner().m_id == m_cityId);
	} 

	if (isValid)
	{
		m_wrappedCur = m_testXY.GetRC();
	}
#endif
}

MapPoint &CityInfluenceIterator::Pos()
{
	return m_wrappedCur;
}

RadiusIterator::RadiusIterator(const MapPoint &center, sint32 size)
#if !defined(ACTIVISION_ORIGINAL)
:	CityInfluenceIterator(center, size)
#endif
{
	Init(center, size, size * size);
}

RadiusIterator::RadiusIterator(const MapPoint &center, sint32 size, sint32 squaredSize)
#if !defined(ACTIVISION_ORIGINAL)
:	CityInfluenceIterator(center, size)
#endif
{
	Init(center, size, squaredSize);
}

RadiusIterator::RadiusIterator()
#if !defined(ACTIVISION_ORIGINAL)
:	CityInfluenceIterator()
#endif
{
#if defined(ACTIVISION_ORIGINAL)
	m_intRadius = 0;
	m_cur.Set(-1,-1);
	m_cityId = 0;
#endif
}


void RadiusIterator::Init(const MapPoint &center, sint32 size, sint32 squaredSize)
{
	m_center = center;
	m_cur.Set(-1,-1);
	m_cityId = g_theWorld->GetCell(center)->GetCityOwner().m_id;
	m_intRadius = sint16(size);
	m_squaredRadius = sint16(squaredSize);
}

void RadiusIterator::Start()
{
	m_startX = m_center.x - m_intRadius;
	m_endX = m_center.x + m_intRadius;
	m_cur.x = m_startX;
	m_row = 0;
	m_cur.y = m_center.y + m_row;
	
#if defined(ACTIVISION_ORIGINAL)	// incorrect wrap computation
	if(!WrapPoint(m_center, m_cur, m_wrappedCur) ||
		MapPoint::GetSquaredDistance(m_cur,m_center) > (m_squaredRadius)) {
		
		Next();
	}
#else
	m_testXY = OrthogonalPoint(m_center);
	m_testXY.Move(WEST, m_intRadius);
	if ( m_testXY.IsValid() &&
		(MapPoint::GetSquaredDistance(m_cur, m_center) <= m_squaredRadius)
	   )
	{
		// Point is on the map, and not too far away.
		m_wrappedCur = m_testXY.GetRC();
	}
	else
	{
		Next();
	}
#endif
}

void RadiusIterator::Next()
{
#if defined(ACTIVISION_ORIGINAL)
	do {
		m_cur.x++;
		m_cur.y--;
		if(m_cur.x > m_endX) {
			m_row++;
			m_cur.x = m_startX;
			m_cur.y = m_center.y + m_row;
		}
	} while (((!WrapPoint(m_center, m_cur, m_wrappedCur)) ||
		MapPoint::GetSquaredDistance(m_cur,m_center) > (m_squaredRadius)) &&
		!End());
#else
	bool	isValid	= false;
	
	while (!isValid && !End())  
	{
		m_cur.x++;
		m_cur.y--;
		m_testXY.Move(NORTHEAST);

		if (m_cur.x > m_endX) 
		{
			m_row++;
			m_cur.x = m_startX;
			m_cur.y = m_center.y + m_row;
			m_testXY.Move(SOUTH);
			m_testXY.Move(SOUTHWEST, m_endX - m_startX);
		}

		isValid = m_testXY.IsValid() &&
				  (MapPoint::GetSquaredDistance(m_cur, m_center) <= m_squaredRadius);
	} 

	if (isValid)
	{
		m_wrappedCur = m_testXY.GetRC();
	}
#endif
}

SquareIterator::SquareIterator(const MapPoint &center, sint32 size)
#if !defined(ACTIVISION_ORIGINAL)
:	CityInfluenceIterator(center, size)
#endif
{
	Init(center, size);
}

SquareIterator::SquareIterator()
#if !defined(ACTIVISION_ORIGINAL)
:	CityInfluenceIterator()
#endif
{
#if defined(ACTIVISION_ORIGINAL)
	m_intRadius = 0;
	m_cur.Set(-1,-1);
	m_cityId = 0;
#endif
}


void SquareIterator::Init(const MapPoint &center, sint32 size)
{
	m_center = center;
	m_cur.Set(-1,-1);
	m_cityId = g_theWorld->GetCell(center)->GetCityOwner().m_id;
	m_intRadius = sint16(size);
}

void SquareIterator::Start()
{
	m_startX = m_center.x - m_intRadius;
	m_endX = m_center.x + m_intRadius;
	m_cur.x = m_startX;
	m_row = 0;
	m_cur.y = m_center.y + m_row;
	
#if defined(ACTIVISION_ORIGINAL)	// incorrect wrap computation
	if(!WrapPoint(m_center, m_cur, m_wrappedCur)) {
		
		Next();
	}
#else
	m_testXY = OrthogonalPoint(m_center);
	m_testXY.Move(WEST, m_intRadius);
	if (m_testXY.IsValid())
	{
		// Point is on the map
		m_wrappedCur = m_testXY.GetRC();
	}
	else
	{
		Next();
	}
#endif
}

void SquareIterator::Next()
{
#if defined(ACTIVISION_ORIGINAL)
	do {
		m_cur.x++;
		m_cur.y--;
		if(m_cur.x > m_endX) {
			m_row++;
			m_cur.x = m_startX;
			m_cur.y = m_center.y + m_row;
		}
	} while ((!WrapPoint(m_center, m_cur, m_wrappedCur)) && !End());
#else
	bool	isValid	= false;
	
	while (!isValid && !End())  
	{
		m_cur.x++;
		m_cur.y--;
		m_testXY.Move(NORTHEAST);

		if (m_cur.x > m_endX) 
		{
			m_row++;
			m_cur.x = m_startX;
			m_cur.y = m_center.y + m_row;
			m_testXY.Move(SOUTH);
			m_testXY.Move(SOUTHWEST, m_endX - m_startX);
		}

		isValid	= m_testXY.IsValid();
	} 
		
	if (isValid)
	{
		m_wrappedCur = m_testXY.GetRC();
	}
#endif
}

bool ExpandInfluence(Unit &city, const MapPoint &centerPos, MapPoint curPos,
					 const CitySizeRecord *rec);


#if defined(ACTIVISION_ORIGINAL)
void GenerateCityInfluence(const MapPoint &cpos, sint32 size)
{
	MapPoint cur, wrappedCur;
	sint16 row = 0;
	const CitySizeRecord *rec = g_theCitySizeDB->Get(size);

	sint16 intRadius = (sint16)rec->GetIntRadius();

	
	Assert(g_theWorld->GetCell(cpos)->HasCity());

	Unit city = g_theWorld->GetCell(cpos)->GetCity();

	
	
	while(row <= (2 * intRadius)) {
		sint32 i;
		cur.x = cpos.x - intRadius;
		cur.y = cpos.y + row;
		for(i = 0; i <= 2 * intRadius; i++) {
			if(WrapPoint(cpos, cur, wrappedCur)) {
				g_theWorld->GetCell(wrappedCur)->SetScratch(0);
			}
			cur.x++;
			cur.y--;
		}
		row++;
	}

	g_theWorld->GetCell(cpos)->SetScratch(1);

	if(cpos.GetNeighborPosition(NORTHEAST, cur)) ExpandInfluence(city, cpos, cur, rec);
	if(cpos.GetNeighborPosition(SOUTHEAST, cur)) ExpandInfluence(city, cpos, cur, rec);
	if(cpos.GetNeighborPosition(SOUTHWEST, cur)) ExpandInfluence(city, cpos, cur, rec);
	if(cpos.GetNeighborPosition(NORTHWEST, cur)) ExpandInfluence(city, cpos, cur, rec);

	if(g_tiledMap)
		g_tiledMap->RedrawTile(&cpos);
}
#else
//----------------------------------------------------------------------------
//
// Name       : GenerateCityInfluence
//
// Description: -
//
// Parameters : cpos				: location of city
//				size				: size of city
//
// Globals    : g_theWorld
//				g_tiledMap
//				g_theCitySizeDB
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

void GenerateCityInfluence(const MapPoint &cpos, sint32 size)
{
	Assert(g_theWorld->GetCell(cpos)->HasCity());
	Unit					city		= g_theWorld->GetCell(cpos)->GetCity();
	CitySizeRecord const *	rec			= g_theCitySizeDB->Get(size);
	sint32 const			intRadius	= rec->GetIntRadius();
	ClearScratch(cpos, size);
	g_theWorld->GetCell(cpos)->SetScratch(1);

	MapPoint				cur;
	if(cpos.GetNeighborPosition(NORTHEAST, cur)) ExpandInfluence(city, cpos, cur, rec);
	if(cpos.GetNeighborPosition(SOUTHEAST, cur)) ExpandInfluence(city, cpos, cur, rec);
	if(cpos.GetNeighborPosition(SOUTHWEST, cur)) ExpandInfluence(city, cpos, cur, rec);
	if(cpos.GetNeighborPosition(NORTHWEST, cur)) ExpandInfluence(city, cpos, cur, rec);

	if(g_tiledMap)
		g_tiledMap->RedrawTile(&cpos);
}
#endif

bool ExpandInfluence(Unit &city, const MapPoint &centerPos, MapPoint curPos,
					 const CitySizeRecord *rec)
{
	Cell *cell = g_theWorld->GetCell(curPos);

	if(cell->GetScratch() != 0) return false;
	if(cell->GetCityOwner().m_id != 0 && cell->GetCityOwner().m_id != city.m_id) return false;
	if(UnitData::GetDistance(centerPos, curPos, 0) > rec->GetSquaredRadius()) return false;
	
	cell->SetCityOwner(city);
	sint32 newOwner = -1;
	if (city.m_id == 0x0)
		cell->SetOwner(-1);
	else {
		cell->SetOwner(city.GetOwner());
		newOwner = city.GetOwner();
	}
	cell->SetScratch(1);

	if(newOwner >= 0) {
		g_network.Block(newOwner);
		g_network.Enqueue(cell, curPos.x, curPos.y);
		g_network.Unblock(newOwner);
	}

	MapPoint nextPos;

	bool redrawMe = false;

	if(curPos.GetNeighborPosition(NORTHEAST, nextPos)) 
		if(!ExpandInfluence(city, centerPos, nextPos, rec)) {
			redrawMe = true;
		}
	if(curPos.GetNeighborPosition(SOUTHEAST, nextPos))
		if(!ExpandInfluence(city, centerPos, nextPos, rec)) {
			redrawMe = true;
		}
	if(curPos.GetNeighborPosition(SOUTHWEST, nextPos))
		if(!ExpandInfluence(city, centerPos, nextPos, rec)) {
			redrawMe = true;
		}
	if(curPos.GetNeighborPosition(NORTHWEST, nextPos))
		if(!ExpandInfluence(city, centerPos, nextPos, rec)) {
			redrawMe = true;
		}

	if(redrawMe && g_tiledMap) {
		g_tiledMap->RedrawTile(&curPos);
	}

	return true;
}

bool ExpandBorders(const MapPoint &center, MapPoint curPos, sint32 player, sint32 squaredRadius);


void GenerateBorders(const MapPoint &cpos, sint32 player, sint32 intRadius, sint32 squaredRadius)
{
#if defined(ACTIVISION_ORIGINAL)
	MapPoint cur, wrappedCur;
	sint16 row = 0;
	
	sint16 intRad = (sint16) intRadius;

	
	
	while(row <= (2 * intRad)) {
		sint32 i;
		cur.x = cpos.x - intRad;
		cur.y = cpos.y + row;
		for(i = 0; i <= 2 * intRad; i++) {
			if(WrapPoint(cpos, cur, wrappedCur)) {
				g_theWorld->GetCell(wrappedCur)->SetScratch(0);
			}
			cur.x++;
			cur.y--;
		}
		row++;
	}
#else
	ClearScratch(cpos, intRadius);

	MapPoint cur;
#endif

	Cell *cell = g_theWorld->GetCell(cpos);
	cell->SetOwner(player);
	cell->SetScratch(1);

	g_network.Block(player);
	g_network.Enqueue(cell, cpos.x, cpos.y);
	g_network.Unblock(player);

	if(cpos.GetNeighborPosition(NORTHEAST, cur)) ExpandBorders(cpos, cur, player, squaredRadius);
	if(cpos.GetNeighborPosition(SOUTHEAST, cur)) ExpandBorders(cpos, cur, player, squaredRadius);
	if(cpos.GetNeighborPosition(SOUTHWEST, cur)) ExpandBorders(cpos, cur, player, squaredRadius);
	if(cpos.GetNeighborPosition(NORTHWEST, cur)) ExpandBorders(cpos, cur, player, squaredRadius);

	g_tiledMap->RedrawTile(&cpos);
}

bool ExpandBorders(const MapPoint &center, MapPoint curPos, sint32 player, sint32 squaredRadius)
{
	Cell *cell = g_theWorld->GetCell(curPos);
	if(cell->GetScratch() != 0) return false; 

	cell->SetScratch(1);

	if(cell->GetOwner() >= 0 && cell->GetOwner() != player) return false; 
	if(UnitData::GetDistance(center, curPos, 0) > squaredRadius) return false;

	cell->SetOwner(player);

	if(player >= 0)
		g_network.Block(player);
	g_network.Enqueue(cell, curPos.x, curPos.y);
	if(player >= 0)
		g_network.Unblock(player);

	MapPoint nextPos;

	bool redrawMe = false;

	if(curPos.GetNeighborPosition(NORTHEAST, nextPos)) 
		if(!ExpandBorders(center, nextPos, player, squaredRadius)) {
			redrawMe = true;
		}
	if(curPos.GetNeighborPosition(SOUTHEAST, nextPos))
		if(!ExpandBorders(center, nextPos, player, squaredRadius)) {
			redrawMe = true;
		}
	if(curPos.GetNeighborPosition(SOUTHWEST, nextPos))
		if(!ExpandBorders(center, nextPos, player, squaredRadius)) {
			redrawMe = true;
		}
	if(curPos.GetNeighborPosition(NORTHWEST, nextPos))
		if(!ExpandBorders(center, nextPos, player, squaredRadius)) {
			redrawMe = true;
		}

	if(redrawMe) {
		g_tiledMap->RedrawTile(&curPos);
	}

	return true;
}
