//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//////////////////////////////////////////////////////////////////////////


// room.cc

#include "stdsneezy.h"

bool TRoom::isCitySector() const
{
  switch (getSectorType()) {
    case SECT_TROPICAL_CITY:
    case SECT_TEMPERATE_CITY:
    case SECT_ARCTIC_CITY:
      return TRUE;
    default:
      return FALSE;
  }
}

bool TRoom::isFlyingSector() const
{
  switch (getSectorType()) {
    case SECT_MAKE_FLY:
      return TRUE;
    default:
      return FALSE;
  }
}


bool TRoom::isRoadSector() const
{
  switch (getSectorType()) {
    case SECT_TROPICAL_ROAD:
    case SECT_TEMPERATE_ROAD:
    case SECT_ARCTIC_ROAD:
    case SECT_ARCTIC_FOREST_ROAD:
    case SECT_TEMPERATE_FOREST_ROAD:
    case SECT_RAINFOREST_ROAD:
      return TRUE;
    default:
      return FALSE;
  }
}

bool TRoom::isVertSector() const
{
  switch (getSectorType()) {
    case SECT_TROPICAL_CLIMBING:
    case SECT_TEMPERATE_CLIMBING:
    case SECT_ARCTIC_CLIMBING:
      return TRUE;
    default:
      return FALSE;
  }
}

bool TRoom::isUnderwaterSector() const
{
  switch (getSectorType()) {
    case SECT_TROPICAL_UNDERWATER:
    case SECT_TEMPERATE_UNDERWATER:
      return TRUE;
    default:
      return FALSE;
  }
}

bool TRoom::isNatureSector() const
{
  switch (getSectorType()) {
    case SECT_GRASSLANDS:
    case SECT_PLAINS:
    case SECT_SAVANNAH:
    case SECT_TEMPERATE_HILLS:
    case SECT_TROPICAL_HILLS:
    case SECT_VELDT:
    case SECT_JUNGLE:
    case SECT_RAINFOREST:
    case SECT_TEMPERATE_FOREST:
    case SECT_ARCTIC_FOREST:
    case SECT_ARCTIC_FOREST_ROAD:
    case SECT_RAINFOREST_ROAD:
      return TRUE;
    default:
      return FALSE;
  }
}

bool TRoom::isSwampSector() const
{
  switch (getSectorType()) {
    case SECT_ARCTIC_MARSH:
    case SECT_TEMPERATE_SWAMP:
    case SECT_TROPICAL_SWAMP:
      return TRUE;
    default:
      return FALSE;
  }
}

bool TRoom::isBeachSector() const
{
  switch (getSectorType()) {
    case SECT_COLD_BEACH:
    case SECT_TEMPERATE_BEACH:
    case SECT_TROPICAL_BEACH:
      return TRUE;
    default:
      return FALSE;
  }
}

bool TRoom::isHillSector() const
{
  switch (getSectorType()) {
    case SECT_ARCTIC_WASTE:
    case SECT_TEMPERATE_HILLS:
    case SECT_TROPICAL_HILLS:
      return TRUE;
    default:
      return FALSE;
  }
}

bool TRoom::isMountainSector() const
{
  switch (getSectorType()) {
    case SECT_ARCTIC_MOUNTAINS:
    case SECT_TEMPERATE_MOUNTAINS:
    case SECT_TROPICAL_MOUNTAINS:
    case SECT_VOLCANO_LAVA:
      return TRUE;
    default:
      return FALSE;
  }
}

bool TRoom::isForestSector() const
{
  switch (getSectorType()) {
    case SECT_JUNGLE:
    case SECT_RAINFOREST:
    case SECT_TEMPERATE_FOREST:
    case SECT_ARCTIC_FOREST:
    case SECT_ARCTIC_FOREST_ROAD:
    case SECT_TEMPERATE_FOREST_ROAD:
    case SECT_RAINFOREST_ROAD:
    case SECT_DEAD_WOODS:
      return TRUE;
    default:
      return FALSE;
  }
}

bool TRoom::isAirSector() const
{
  switch (getSectorType()) {
    case SECT_TROPICAL_ATMOSPHERE:
    case SECT_TEMPERATE_ATMOSPHERE:
    case SECT_ARCTIC_ATMOSPHERE:
    case SECT_FIRE_ATMOSPHERE:
      return TRUE;
    default:
      return FALSE;
  }
}

bool TRoom::isOceanSector() const
{
  switch (getSectorType()) {
    case SECT_TROPICAL_OCEAN:
    case SECT_TEMPERATE_OCEAN:
    case SECT_ICEFLOW:
      return TRUE;
    default:
      return FALSE;
  }
}

bool TRoom::isRiverSector() const
{
  switch (getSectorType()) {
    case SECT_TROPICAL_RIVER_SURFACE:
    case SECT_TEMPERATE_RIVER_SURFACE:
    case SECT_ARCTIC_RIVER_SURFACE:
      return TRUE;
    default:
      return FALSE;
  }
}

bool TRoom::isIndoorSector() const
{
  switch (getSectorType()) {
    case SECT_TEMPERATE_BUILDING:
    case SECT_TEMPERATE_CAVE:
    case SECT_TROPICAL_BUILDING:
    case SECT_TROPICAL_CAVE:
    case SECT_ARCTIC_BUILDING:
    case SECT_ARCTIC_CAVE:
      return TRUE;
    default:
      return FALSE;
  }
}

bool TRoom::isArcticSector() const
{
  return (getSectorType() >= SECT_SUBARCTIC &&
          getSectorType() < SECT_PLAINS);
}

bool TRoom::isTropicalSector() const
{
  return ((getSectorType() >= SECT_DESERT &&
           getSectorType() < SECT_ASTRAL_ETHREAL) ||
          getSectorType() == SECT_FIRE ||
          getSectorType() == SECT_FIRE_ATMOSPHERE);
}

bool TRoom::isWierdSector() const
{
  return (getSectorType() == SECT_SOLID_ICE || getSectorType() >= 60);
}

bool TRoom::isFallSector() const
{
  return (isAirSector() || isVertSector() || isFlyingSector());
}

bool TRoom::isWaterSector() const
{
  return (isRiverSector() || isOceanSector());
}

bool TRoom::isWildernessSector() const
{
  return (!isIndoorSector() && !isRoadSector() && !isCitySector() && !isWierdSector());
}

bool TRoom::notRangerLandSector() const
{
  return (isCitySector() || isRoadSector() || isFallSector() || isUnderwaterSector() || isWaterSector() || isIndoorSector());
}

roomDirData * TRoom::exitDir(dirTypeT door) const
{
  return (dir_option[door]);
}

roomDirData * TBeing::exitDir(dirTypeT door) const
{
  return (roomp->dir_option[door]);
}

roomDirData * TObj::exitDir(dirTypeT door) const
{
  return (roomp->dir_option[door]);
}

void room_iterate(TRoom *[], void (*func) (int, TRoom *, sstring &, struct show_room_zone_struct *), sstring &sbdata, void *srzdata)
{
  register int i;
  for (i = 0; i < WORLD_SIZE; i++) {
    TRoom *temp = real_roomp(i);

    if (temp)
      (*func) (i, temp, sbdata, (struct show_room_zone_struct *) srzdata);
  }
}

sectorTypeT TRoom::getSectorType() const
{
  return sectorType;
}

void TRoom::setSectorType(sectorTypeT type)
{
  sectorType = type;
}

dirTypeT TRoom::getRiverDir() const
{
  return riverDir;
}

byte TRoom::getRiverSpeed() const
{
  return riverSpeed;
}

void TRoom::setDescr(const char *tDescription)
{
  descr = tDescription;
}


const char * TRoom::getDescr()
{
  return descr;
}

bool TRoom::putInDb(int vnum)
{
  if (real_roomp(vnum))
    return FALSE;

  room_db[vnum] = this;
  return TRUE;
}

int TRoom::chiMe(TBeing *tLunatic)
{
  TBeing *tSucker;
  int     tRc = 0;
  TThing *tThing,
         *tNextThing;

  if (tLunatic->getSkillValue(SKILL_CHI) < 100 ||
      tLunatic->getDiscipline(DISC_MEDITATION_MONK)->getLearnedness() < 25) {
    tLunatic->sendTo("I'm afraid you don't have the training to do this.\n\r");
    return FALSE;
  }

  if (tLunatic->checkPeaceful("You feel too peaceful to contemplate violence here.\n\r"))
    return FALSE;

  act("You focus your <c>mind<z> and unleash a <r>blast of chi<z> upon your foes!",
      FALSE, tLunatic, NULL, NULL, TO_CHAR);
  act("$n suddenly <r>radiates with power<z> and brings harm to $s enemies!",
      TRUE, tLunatic, NULL, NULL, TO_ROOM);

  for (tThing = getStuff(); tThing; tThing = tNextThing) {
    tNextThing = tThing->nextThing;

    if (!(tSucker = dynamic_cast<TBeing *>(tThing)) || tSucker == tLunatic)
      continue;

    tRc = tSucker->chiMe(tLunatic);

    if (IS_SET_DELETE(tRc, RET_STOP_PARSING)) {
      tLunatic->sendTo("You are forced to stop.\n\r");
      return true;
    }

    if (IS_SET_DELETE(tRc, DELETE_THIS))
      return (DELETE_THIS | RET_STOP_PARSING);

    if (IS_SET_DELETE(tRc, DELETE_VICT)) {
      delete tThing;
      tThing = NULL;
    }
  }

  return true;
}

void TRoom::operator << (TThing &tThing)
{
  // assign birthRoom
  TMonster * tmon = dynamic_cast<TMonster *>(&tThing);
  if (tmon)
    tmon->brtRoom = this->number;

  if (!tBornInsideMe) {
    tBornInsideMe = &tThing;
    tThing.nextBorn = NULL;
    return;
  }

  TThing *tList;

  // creates forward-linked list
  for (tList = tBornInsideMe; tList->nextBorn; tList = tList->nextBorn) {
    if (&tThing == tList) {
      vlogf(LOG_BUG, fmt("Mob already in born list being added again. [%s]") %  tThing.getName());
      return;
    }
  }

  tList->nextBorn = &tThing;
  tThing.nextBorn = NULL;
}

bool TRoom::operator |= (const TThing &tThing)
{
  TThing *tList;

  for (tList = tBornInsideMe; tList; tList = tList->nextBorn)
    if (tList == &tThing)
      return true;

  return false;
}

void TRoom::operator >> (const TThing &tThing)
{
  TThing *tList,
         *tLast = NULL;

  for (tList = tBornInsideMe; tList; tList = tList->nextBorn) {
    if (&tThing == tList) {
      if (tLast)
        tLast->nextBorn = tList->nextBorn;
      else
	tBornInsideMe = tList->nextBorn;

      tList->nextBorn = NULL;

      return;
    }

    tLast = tList;
  }

  vlogf(LOG_BUG, fmt("Attempt to remove mob from born list that isn't in born list! [%s]") %  tThing.getName());
}


int TRoom::getLight()
{
  return ((isRoomFlag(ROOM_ALWAYS_LIT)) ? 18 : TThing::getLight());

}