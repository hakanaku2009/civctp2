//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// File name    : ui\aui_ctp2\SetItem.h
// Description  : Handles stuff about selected items.
// Id           : $Id$
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added debug pathing for the city astar. (17-Jan-2008 Martin G�hmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SELECTED_ITEM_H__
#define __SELECTED_ITEM_H__ 1

class SelectedItem;

enum SELECT_TYPE {
SELECT_TYPE_NONE,

SELECT_TYPE_LOCAL_ARMY,
SELECT_TYPE_LOCAL_ARMY_UNLOADING,
SELECT_TYPE_REMOTE_ARMY,
SELECT_TYPE_LOCAL_CITY,
SELECT_TYPE_REMOTE_CITY,
SELECT_TYPE_TRADE_ROUTE,
SELECT_TYPE_GOOD,

SELECT_TYPE_MAX

};

enum SELECT_BUTTON {
	SELECT_BUTTON_LEFT,
	SELECT_BUTTON_LEFT_PATHING,
	SELECT_BUTTON_RIGHT,
	SELECT_BUTTON_RIGHT_PATHING,
	SELECT_BUTTON_MIDDLE,
	SELECT_BUTTON_LEFT_DRAG,
	SELECT_BUTTON_LEFT_DROP,
	SELECT_BUTTON_MAX
};

enum SELECT_MODE {
	SELECT_MODE_RIGHT,
	SELECT_MODE_LEFT,
	SELECT_MODE_CTP2,
	SELECT_MODE_CUSTOM,
	SELECT_MODE_MAX
};

#include "MapPoint.h"
#include "Path.h"
#include "Army.h"
#include "Unit.h"
#include "gstypes.h"

class Unit;
class CivArchive;
class NetUnit;
class ID;
struct aui_MouseEvent;
class OrderRecord;

#define k_SELECTED_ITEM_VERSION_MAJOR 0
#define k_SELECTED_ITEM_VERSION_MINOR 0

class SelectedItem
{
	typedef void (SelectedItem::*ClickFunctionPtr) (const MapPoint &pos,
												const aui_MouseEvent *data,
												bool doubleClick);

	// Serialized
	SELECT_TYPE m_select_state[k_MAX_PLAYERS];
	sint32 m_is_at_start[k_MAX_PLAYERS];
	PLAYER_INDEX m_next_player[k_MAX_PLAYERS];
	sint32 m_remote_owner[k_MAX_PLAYERS];
	MapPoint m_select_pos[k_MAX_PLAYERS];
	PLAYER_INDEX m_current_player;
	Army m_selected_army[k_MAX_PLAYERS];
	Unit m_selected_city[k_MAX_PLAYERS];
	// End of serialied members

	DynamicArray<Army> m_select_cycle;

	bool         m_is_pathing;
	MapPoint     m_cur_mouse_tile;
	Path *       m_good_path, m_bad_path;
	bool         m_is_broken_path;
	DynamicArray<MapPoint> m_waypoints;
	PLAYER_INDEX m_player_on_screen;
	bool         m_auto_unload;
	bool         m_gotClickSinceLastAutoEnd;
	bool         m_selected_something_since_director_select;
	Army         m_force_select_army;
	bool         m_ignoreCitySelect;
	bool         m_isDragging;
	MapPoint     m_startDragPos;
	OrderRecord *m_moveOrder;
	OrderRecord *m_transportOrder;
	uint32       m_clickStartTime;
	bool         m_gotClick;
	bool         m_justNowSelectedArmy;
	bool         m_justGotDoubleClick;

	ClickFunctionPtr m_clickFunc[SELECT_TYPE_MAX][SELECT_TYPE_MAX][SELECT_BUTTON_MAX][SELECT_MODE_MAX];

public:
	friend class NetUnit;

	SelectedItem(const sint32 nPlayers);
	SelectedItem(CivArchive &archive);
	~SelectedItem();

	void Serialize(CivArchive &archive);
	void Init();

	void Deselect(PLAYER_INDEX player);

	void EnterArmyMove(PLAYER_INDEX player, const MapPoint &pos);

	void SetAutoUnload(bool a) { m_auto_unload = a; }
	bool GetAutoUnload() { return(m_auto_unload); }

	void PlaySelectedSound(Unit &unit);
	bool GetTopUnitOrCity(const MapPoint &pos, Unit &top);
	bool GetTopUnit(const MapPoint &pos, Unit &top);

	SELECT_TYPE GetClickedThing(const MapPoint &pos, bool click);
	void RegisterClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick,
					   bool leftDrag, bool leftDrop);

	void KeyboardSelectFirstUnit();
	void SelectFirstUnit(bool setSelect = true);
	void NextUnmovedUnit(bool isFirst = false, bool manualNextUnit = false);
	double UnitsDoneRatio();
	double UnitsBusyRatio();
	void MaybeAutoEndTurn(bool isFirst = false);

	void Refresh();
	void SetSelectCity(const Unit &u, bool all = true, bool isDoubleClick = false);
	void SetSelectUnit(const Unit &u, bool all = true, bool isDoubleClick = false);
	void SetSelectGood(const MapPoint &pos);

	SELECT_TYPE GetState() { return m_select_state[GetVisiblePlayer()]; }

	void GetTopCurItem(PLAYER_INDEX &s_player, ID &s_item,
					   SELECT_TYPE &s_state);

	MapPoint GetCurSelectPos(void) { return m_select_pos[GetVisiblePlayer()]; }

	void ClipCurrentItem();
	void NextItem();

	void ClearWaypoints();
	void NextPlayer();
	sint32 GetPlayerAfterThis(const sint32 p);

	PLAYER_INDEX GetNextHumanPlayer();
	void NextRound();

	void RegisterCreatedUnit(const PLAYER_INDEX owner);
	void RegisterCreatedCity(const PLAYER_INDEX owner);
	void RegisterRemovedArmy(const PLAYER_INDEX owner, const Army &dead_army);
	void RegisterRemovedCity(const PLAYER_INDEX owner, const Unit &dead_city);


	void SetCurPlayer (PLAYER_INDEX p);
	PLAYER_INDEX GetCurPlayer() const { return m_current_player; };
	void RemovePlayer(PLAYER_INDEX p);
	void AddPlayer(PLAYER_INDEX p) ;
	sint32 GetVisiblePlayer() const;
	bool IsPlayerVisible(sint32 player) const { return player == GetVisiblePlayer(); };
	bool IsAutoCenterOn() const;
	void SetAutoCenter(const bool on);

	void AddWaypoint(const MapPoint &p);
	sint32 GetIsPathing() const;
	bool ShouldDrawPath();
	void GetOldMouseTilePos(MapPoint &p) const { p = m_cur_mouse_tile; }
	void SetCurMouseTile(const MapPoint &p) { m_cur_mouse_tile = p; }
	void SetDrawablePathDest(MapPoint &p);
	void ConstructPath(bool &isCircular, double &cost);

	Path *GetGoodPath() { return m_good_path; }
	Path GetBadPath() { return m_bad_path; }

	void SelectTradeRoute(const MapPoint &p);

	bool IsLocalArmy() const;
	bool IsLocalCity() const;

	void Patrol(const MapPoint &pos);
	bool ResumePatrol();
	void ForgetPatrol();
	void ProcessUnitOrders();

	void GroupArmy();
	void UngroupArmy();

	void Settle();
	void Entrench();
	void Detrench();
	void Sleep();
	void Disband();

	void UnloadMode();
	void CancelUnload();

	void Paradrop(const MapPoint &point);

	void SpaceLaunch();

	void EstablishEmbassy(const MapPoint &point);
	void InvestigateCity(const MapPoint &point);
	void NullifyWalls(const MapPoint &point);
	void StealTechnology(const MapPoint &point);
	void InciteRevolution(const MapPoint &point);
	void AssassinateRuler(const MapPoint &point);
	void InvestigateReadiness(const MapPoint &point);

	void Bombard(const MapPoint &point);

	void Franchise(const MapPoint &point);
	void Sue(const MapPoint &point);
	void SueFranchise(const MapPoint &point);
	void Expel(const MapPoint &point);
	void Advertise(const MapPoint &point);
	void PlantNuke(const MapPoint &point);
	void SlaveRaid(const MapPoint &point);
	void EnslaveSettler(const MapPoint &point);
	void UndergroundRailway(const MapPoint &point);
	void InciteUprising(const MapPoint &point);
	void BioInfect(const MapPoint &point);
	void NanoInfect(const MapPoint &point);
	void ConvertCity(const MapPoint &point);
	void ReformCity(const MapPoint &point);
	void IndulgenceSale(const MapPoint &point);
	void Soothsay(const MapPoint &point);

	void Cloak();
	void Uncloak();
	void Rustle(const MapPoint &point);
	void CreatePark(const MapPoint &point);
	void CreateRift(const MapPoint &point);
	void Pillage();
	void Injoin(const MapPoint &point);
	void UseSpaceLadder();
	void Airlift(const MapPoint &dest);
	void Descend();
	void Goto(MapPoint &dest);

	void EnterMovePath(sint32 owner, Army &army,
			const MapPoint &src, const MapPoint &dest);
	void EntrenchArmy(sint32 owner, sint32 index);
	void SleepArmy(sint32 owner, sint32 index);
	void InterceptTrade(void);
	bool GetSelectedCity( Unit &city );
	void UnitCityToggle();

	PLAYER_INDEX GetPlayerOnScreen(void) { return m_player_on_screen; }
	void SetPlayerOnScreen(PLAYER_INDEX player);
	void ClearCycle();
	void DidKeyboardMove();
	void EndUnitTurn();

	void DirectorUnitSelection(uint32 flags);
	void ForceDirectorSelect(const Army &army);
	void RegisterUIClick();
	void RegisterManualEndTurn();
	void UpdateSelectedItem( void );

	bool GetInciteRevolutionCost( const MapPoint &point, sint32 &cost );
	bool GetInciteUprisingCost( const MapPoint &point, sint32 &cost );

	void ArmyMovedCallback(Army &a);

	void SetupClickFunctions();
	void NewRegisterClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick,
						  bool leftDrag, bool leftDrop);

	void ErrorClick             (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);
	void NullClick              (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);

	void SelectArmyClick          (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);
	void SelectArmyCommandClick   (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);
	void SelectCityClick          (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);
	void SelectEnemyCityClick     (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);
	void SelectEnemyArmyClick     (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);
	void SelectGoodClick          (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);
	void SelectTradeRouteClick    (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);

	void DeselectClick            (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);
	void UnloadClick              (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);

	void TerrainContextClick      (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);
	void ArmyContextClick         (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);
	void CityContextClick         (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);
	void EnemyCityContextClick    (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);
	void EnemyArmyContextClick    (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);
	void TradeRouteContextClick   (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);
	void GoodContextClick         (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);

	void SendGoodClick            (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);

	void MoveArmyClick            (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);

	void StartMoveClick           (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);
	void MoveDrag                 (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);
	void MoveDrop                 (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);
	void ActionClick              (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);
	void SelectArmyStartMoveClick (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);
	void CancelPathingClick       (const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick);

	void CheckPreOrderMove(MapPoint &pos, aui_MouseEvent *data);

	bool GetSelectedArmy(Army &a);

	OrderRecord *GetMoveOrder() { return m_moveOrder; }
	OrderRecord *GetTransportOrder() {return m_transportOrder;}
};

bool CanAutoSelect(const Army &army);
bool HasContinuingOrder(const Army &army);

extern uint32 SelectedItem_GetVersion(void);

extern SelectedItem *g_selected_item;

#endif
