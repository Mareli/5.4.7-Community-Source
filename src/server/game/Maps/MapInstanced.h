/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRINITY_MAP_INSTANCED_H
#define TRINITY_MAP_INSTANCED_H

#include "Map.h"
#include "C:/Users/Quadral/Documents/WoWCircle/New source/ardor-i90-at-realm/src/server/game/Instances/InstanceSaveMgr.h"
#include "DBCEnums.h"

class RaidEncounter;

class MapInstanced : public Map
{
    friend class MapManager;
    public:
        typedef UNORDERED_MAP< uint32, Map*> InstancedMaps;

        MapInstanced(uint32 id, time_t expiry);
        ~MapInstanced() {}

        // functions overwrite Map versions
        void Update(const uint32);
        void DelayedUpdate(const uint32 diff);
        //void RelocationNotify();
        void UnloadAll();
        bool CanEnter(Player* player);

        Map* CreateInstanceForPlayer(const uint32 mapId, Player* player);
        Map* FindInstanceMap(uint32 instanceId) const
        {
            InstancedMaps::const_iterator i = m_InstancedMaps.find(instanceId);
            return(i == m_InstancedMaps.end() ? NULL : i->second);
        }

        Map* FindRaidEncounterMap(RaidEncounter* encounter)
        {
            for (InstancedMaps::const_iterator itr = m_InstancedMaps.begin(); itr != m_InstancedMaps.end(); ++itr)
            {
                if (itr->second->GetRaidEncounter() == encounter)
                    return itr->second;
            }

            return NULL;
        }

        Map* FindScenarioMap(uint64 owner)
        {
            for (InstancedMaps::const_iterator itr = m_InstancedMaps.begin(); itr != m_InstancedMaps.end(); ++itr)
            {
                if (itr->second->GetScenarioOwner() == owner)
                    return itr->second;
            }

            return NULL;
        }

        bool DestroyInstance(InstancedMaps::iterator &itr);

        void AddGridMapReference(const GridCoord &p)
        {
            ++GridMapReference[p.x_coord][p.y_coord];
            SetUnloadReferenceLock(GridCoord(63-p.x_coord, 63-p.y_coord), true);
        }

        void RemoveGridMapReference(GridCoord const& p)
        {
            --GridMapReference[p.x_coord][p.y_coord];
            if (!GridMapReference[p.x_coord][p.y_coord])
                SetUnloadReferenceLock(GridCoord(63-p.x_coord, 63-p.y_coord), false);
        }

        InstancedMaps &GetInstancedMaps() { return m_InstancedMaps; }
        virtual void InitVisibilityDistance();

    private:
        InstanceMap* CreateInstance(uint32 InstanceId, InstanceSave* save, Difficulty difficulty);
        InstanceMap* CreateInstance(uint32 InstanceId, RaidEncounter* encounter, Difficulty difficulty);
        BattlegroundMap* CreateBattleground(uint32 InstanceId, Battleground* bg);
        InstanceMap* CreateScenarioInstance(uint32 InstanceId, uint64 ownerId, Difficulty difficulty);

        InstancedMaps m_InstancedMaps;

        uint16 GridMapReference[MAX_NUMBER_OF_GRIDS][MAX_NUMBER_OF_GRIDS];
};
#endif
