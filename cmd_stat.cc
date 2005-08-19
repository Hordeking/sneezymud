//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//      "cmd_stat.cc" - The stat command
//  
//////////////////////////////////////////////////////////////////////////

#include "stdsneezy.h"
#include "disease.h"

extern int eqHpBonus(const TPerson *);
extern int baseHp();
extern float classHpPerLevel(const TPerson *);
extern int ageHpMod(const TPerson *);

void TBeing::statZone(const sstring &zoneNumber)
{
  int zNum,
    cnDesc[3]  = {0, 0, 0},
    cnTitle[3] = {0, 0, 0},
    cnExtra[2] = {0, 0},
    cnFlags[7] = {0, 0, 0, 0, 0, 0, 0},
    rzCount    = 0;
  sstring sb("");
  unsigned long int rNums[2] = {0, 0}, Runner = 0;
  TRoom *curRoomCntr;

  if (zoneNumber.empty()) {
    if (!roomp) {
      vlogf(LOG_BUG, "statZone called by being with no current room.");
      return;
    }

    zNum = roomp->getZoneNum();
  } else {
    zNum = convertTo<int>(zoneNumber);
  }

  if (zNum < 0 || zNum >= (signed int) zone_table.size()) {
    sendTo("Zone number incorrect.\n\r");
    return;
  }

  zoneData &zne = zone_table[zNum];

  Runner = rNums[0] = (zNum ? zone_table[zNum - 1].top + 1 : 0);
  rNums[1] = zne.top;

  sb += "Basic Information:\n\r--------------------\n\r";
  sb += fmt("Zone Num: %3d     Active: %s\n\r") %
    zNum % (zne.enabled ? "Enabled" : "Disabled");

  for (; Runner < (rNums[1] + 1); Runner++)
    if ((curRoomCntr = real_roomp(Runner))) {
      rzCount++;

      if (curRoomCntr->getDescr()) {// Count Descriptions
        cnDesc[0]++;

        if (!strncmp(curRoomCntr->getDescr(), "Empty", 5)) {
          cnDesc[2]++;
        }
      } else {
        cnDesc[1]++;
      }

      if (curRoomCntr->name) {// Count Titles
        sstring tString;

        cnTitle[0]++;
        tString = fmt("%d") % curRoomCntr->number;

        if (tString.find(curRoomCntr->name) != sstring::npos) {
          cnTitle[2]++;
        }
      } else {
        cnTitle[1]++;
      }

      if (curRoomCntr->ex_description)// Count Rooms with extra descriptions
        cnExtra[0]++;
      else
        cnExtra[1]++;

      if (curRoomCntr->isRoomFlag(ROOM_DEATH    ))// Count DEATH_ROOM flags
        cnFlags[0]++;
      if (curRoomCntr->isRoomFlag(ROOM_NO_FLEE  ))// Count NO_FLEE flags
        cnFlags[1]++;
      if (curRoomCntr->isRoomFlag(ROOM_PEACEFUL ))// Count PEACEFUL flags
        cnFlags[2]++;
      if (curRoomCntr->isRoomFlag(ROOM_NO_HEAL  ))// Count NO_HEAL flags
        cnFlags[3]++;
      if (curRoomCntr->isRoomFlag(ROOM_SAVE_ROOM))// Count SAVE_ROOM flags
        cnFlags[4]++;
      if (curRoomCntr->isRoomFlag(ROOM_INDOORS  ))// Count INDOOR flags
        cnFlags[5]++;
      if (curRoomCntr->isRoomFlag(ROOM_PEACEFUL) &&
          !curRoomCntr->isRoomFlag(ROOM_NO_HEAL))// If is Peaceful should ALWAYS be no-heal
        cnFlags[6]++;
    }

  sb += fmt("S-Room: %5lu     E-Room: %5lu     Total:(%lu/%d)\n\r") %
    rNums[0] % rNums[1] % (rNums[1] - rNums[0] + 1) % rzCount;
  sb += "Key Information:\n\r--------------------\n\r";
  sb += fmt("DescrCount: %3d     NoDescr: %3d     InDescr: %3d\n\r") %
    cnDesc[0] % cnDesc[1] % cnDesc[2];
  sb += fmt("TitleCount: %3d     NoTitle: %3d     InTitle: %3d\n\r") %
    cnTitle[0] % cnTitle[1] % cnTitle[2];
  sb += fmt("ExtraCount: %3d     NoExtra: %3d     (Room Counts)\n\r") %
    cnExtra[0] % cnExtra[1];
  sb += "Key Flags:\n\r--------------------\n\r";
  if (cnFlags[0]) {
    sb += fmt("Death-Rooms: %3d\n\r") % cnFlags[0];
  }
  if (cnFlags[1]) {
    sb += fmt("No-Flee    : %3d\n\r") % cnFlags[1];
  }
  if (cnFlags[2]) {
    sb += fmt("Peaceful   : %3d\n\r") % cnFlags[2];
  }
  if (cnFlags[3]) {
    sb += fmt("No-Heal    : %3d\n\r") % cnFlags[3];
  }
  if (cnFlags[6]) {
    sb += fmt("...CRITICAL: +Peaceful !No-Heal: %3d\n\r") % cnFlags[6];
  }
  if (cnFlags[4]) {
    sb += fmt("Save-Room%c : %3d\n\r") %
      (cnFlags[4] > 1 ? 's' : ' ') % cnFlags[4];
  }
  if (cnFlags[5]) {
    sb += fmt("Indoors    : %3d\n\r") % cnFlags[5];
  }

  desc->page_string(sb, SHOWNOW_NO, ALLOWREP_YES);
}

void TBeing::statRoom(TRoom *rmp)
{
  sstring str;
  sstring tmp_str;
  sstring buf2, buf3, buf4;
  extraDescription *e;
  TThing *t;
  int counter = 0, volume;

  if (!limitPowerCheck(CMD_EDIT, rmp->number)) {
    sendTo("You are not allowed to stat this room, sorry.\n\r");
    return;
  }


  str = fmt("%sRoom name:%s %s, %sOf zone:%s %d. %sV-Number:%s %d, %sR-number:%s %d\n\r") %
    cyan() % norm() % rmp->name %
    cyan() % norm() % rmp->getZoneNum() %
    cyan() % norm() % rmp->number %
    cyan() % norm() % in_room;

  str += fmt("%sRoom Coords:%s %d, %d, %d\n\r") %
    cyan() % norm() %
    rmp->getXCoord() % rmp->getYCoord() % rmp->getZCoord();

  str += fmt("%sSector type:%s %s") %
    cyan() % norm() % TerrainInfo[rmp->getSectorType()]->name;

  str += fmt("  %sSpecial procedure:%s ") % cyan() % norm();

  str += fmt("%s") % ((rmp->funct) ? "Exists\n\r" : "No\n\r");

  str += fmt("%sRoom flags:%s ") % cyan() % norm();

  str += sprintbit((long) rmp->getRoomFlags(), room_bits);
  str += "\n\r";

  str += fmt("%sRoom flag bit vector:%s ") % cyan() % norm();

  str += fmt("%d\n\r") % ((unsigned int) rmp->getRoomFlags());

  str += fmt("%sDescription:%s\n\r") % cyan() % norm();
  tmp_str = rmp->getDescr();
  if (tmp_str.empty()) {
    str += "NO DESCRIPTION\n\r";
  } else {
    str += tmp_str.toCRLF();
  }

  str += fmt("%sExtra description keywords(s):%s") % cyan() % norm();
  if (rmp->ex_description) {
    str += "\n\r";
    for (e = rmp->ex_description; e; e = e->next) {
      str += e->keyword;
      str += "\n\r";
    }
    str += "\n\r";
  } else {
    str += " NONE\n\r";
  }

  buf3 = fmt("%d") % rmp->getRoomHeight();
  buf4 = fmt("%d") % rmp->getMoblim();
  str += fmt("%sLight:%s %d   %sRoom Height:%s %s    %sMaximum capacity:%s %s\n\r") %
    cyan() % norm() % rmp->getLight() %
    cyan() % norm() % ((rmp->getRoomHeight() <= 0) ? "Unlimited" : buf3) %
    cyan() % norm() % ((rmp->getMoblim()) ? buf4 : "Infinite");

  if (rmp->isWaterSector() || rmp->isUnderwaterSector()) {
    str += fmt("%sRiver direction:%s %s") % 
      cyan() % norm() %
      ((rmp->getRiverDir() < 0) ? "None" : dirs[rmp->getRiverDir()]);

    str += fmt("   %sRiver speed:%s ") % cyan() % norm();
    if (rmp->getRiverSpeed() >= 1)
      str += fmt("Every %d heartbeat%s\n\r") %
	rmp->getRiverSpeed() % ((rmp->getRiverSpeed() != 1) ? "s." : ".");
    else
      str += fmt("no current.\n\r");

    str += fmt("%sFish caught:%s %i\n\r") %
      cyan() % norm() % rmp->getFished();

  }
  if (rmp->isForestSector())
    str += fmt("Number of logs harvested: %i\n\r")
      % rmp->getLogsHarvested();
  if ((rmp->getTeleTarg() > 0) && (rmp->getTeleTime() > 0)) {
    str += fmt("%sTeleport speed:%s Every %d heartbeats.  %sTo room:%s %d  %sLook?%s %s.\n\r") %
      cyan() % norm() % rmp->getTeleTime() %
      cyan() % norm() % rmp->getTeleTarg() %
      cyan() % norm() % (rmp->getTeleLook() ? "yes" : "no");
  }
  str += fmt("%s------- Chars present -------%s\n\r") % cyan() % norm();
  counter = 0;
  for (t = rmp->getStuff(); t; t = t->nextThing) {
    // canSee prevents seeing invis gods of higher level
    if (dynamic_cast<TBeing *>(t) && canSee(t)) {
      counter++;
      if (counter > 15) {
         str += "Too Many In Room to Stat More\n\r";
         break;
      } else {
        str += fmt("%s%s   (%s)\n\r") %
	  t->getName() % (dynamic_cast<TPerson *>(t) ? "(PC)" : "(NPC)") %
	  t->name;
      }
    }
  }
  str += fmt("%s--------- Born Here ---------%s\n\r") % cyan() % norm();
  counter = 0;
  for (t = rmp->tBornInsideMe; t; t = t->nextBorn) {
    TMonster *tMonster;

    if ((tMonster = dynamic_cast<TMonster *>(t))) {
      counter++;

      if (counter > 5) {
        str += "Too Many Creators Born In Room To Show More\n\r";
        break;
      } else {
        str += fmt("[%6d] %s\n\r") % tMonster->mobVnum() % tMonster->getName();
      }
    }
  }
  str += fmt("%s--------- Contents ---------%s\n\r") % cyan() % norm();
  counter = 0;
  volume = 0;
  buf2="";
  for (t = rmp->getStuff(); t; t = t->nextThing) {
    if (!dynamic_cast<TBeing *>(t)) {
      volume += t->getVolume();
      counter++;
      if (counter > 30) {
        buf2 += "Too Many In Room to Stat More\n\r";
        break;
      } else {
        buf2 += fmt("%s   (%s)\n\r") % t->getName() % t->name;
      }
    }
  }
  str += fmt("%sTotal Volume:%s %s\n\r") %
    cyan() % norm() % volumeDisplay(volume);
  str += buf2;

  str += fmt("%s------- Exits defined -------%s\n\r") % cyan() % norm();
  dirTypeT dir;
  for (dir = MIN_DIR; dir < MAX_DIR; dir++) {
    if (!rmp->dir_option[dir]) {
      continue;
    } else {
      str+=fmt("%sDirection:%s %-10s    %sDoor Type:%s %-12s     %sTo-Room:%s %d\n\r") %
        cyan() % norm() %
	dirs[dir] %
        cyan() % norm() %
        door_types[rmp->dir_option[dir]->door_type] %
        cyan() % norm() %
	rmp->dir_option[dir]->to_room;
      if (rmp->dir_option[dir]->door_type != DOOR_NONE) {
        str += fmt("%sWeight:%s %d      %sExit Flags:%s %s\n\r%sKeywords:%s %s\n\r") %
          cyan() % norm() %
          rmp->dir_option[dir]->weight % 
          cyan() % norm() %
          sprintbit(rmp->dir_option[dir]->condition, exit_bits) %
          cyan() % norm() %
	  rmp->dir_option[dir]->keyword;
        if ((rmp->dir_option[dir]->key > 0) || 
             (rmp->dir_option[dir]->lock_difficulty >= 0)) {
          str += fmt("%sKey Number:%s %d     %sLock Difficulty:%s %d\n\r") %
            cyan() % norm() %
	    rmp->dir_option[dir]->key %
            cyan() % norm() %
            rmp->dir_option[dir]->lock_difficulty;
        }
        if (IS_SET(rmp->dir_option[dir]->condition, EX_TRAPPED)) {
          buf2 = sprinttype(rmp->dir_option[dir]->trap_info, trap_types);
          str += fmt("%sTrap type:%s %s,  %sTrap damage:%s %d (d8)\n\r") % 
            cyan() % norm() %
	    buf2 %
            cyan() % norm() %
            rmp->dir_option[dir]->trap_dam;
        }
      } else if (IS_SET(rmp->dir_option[dir]->condition, EX_SLOPED_UP)) {
        str += fmt("%sSloped:%s Up\n\r") % cyan() % norm();
      } else if (IS_SET(rmp->dir_option[dir]->condition, EX_SLOPED_DOWN)) {
        str += fmt("%sSloped:%s Down\n\r") % cyan() % norm();
      }
      str += fmt("%sDescription:%s\n\r") % cyan() % norm();
      tmp_str = rmp->dir_option[dir]->description;
      if (tmp_str.empty()) {
        str += "UNDEFINED\n\r";
      } else {
        str += tmp_str.toCRLF();
      }
    }
  }
  desc->page_string(str);
  return;
}

void TBeing::statObj(const TObj *j)
{
  extraDescription *e;
  TThing *t;
  int i;
  sstring str;

  if (!limitPowerCheck(CMD_OEDIT, j->getSnum())) {
    sendTo("You are not allowed to stat that object, sorry.\n\r");
    return;
  }

  str = fmt("Object name: [%s], R-number: [%d], V-number: [%d] Item type: ") %
    j->name % j->number % obj_index[j->getItemIndex()].virt;

  str += ItemInfo[j->itemType()]->name;
  str += "\n\r";

  for (unsigned int zone = 0; zone < zone_table.size(); zone++) {
    if(obj_index[j->getItemIndex()].virt <= zone_table[zone].top){
      str += fmt("Zone: %s\n\r") % zone_table[zone].name;
      break;
    }    
  }

  str += fmt("Short description: %s\n\rLong description:\n\r%s\n\r") %
    ((j->shortDescr) ? j->shortDescr : "None") %
    ((j->getDescr()) ? j->getDescr() : "None");

  if (j->action_description) {
    str += "Action Description: ";
    str += j->action_description;
    str += "\n\r";
  }

  str += fmt("Action pointer: %s\n\r") % (j->act_ptr ? "YES" : "no");

  if (j->ex_description) {
    str += "Extra description keyword(s):\n\r----------\n\r";

    for (e = j->ex_description; e; e = e->next) {
      str += e->keyword;
      str += "\n\r";
    }
    str += "----------\n\r";
  } else {
    str += "Extra description keyword(s): None.\n\r";
  }

  if (j->owners) {
    str += fmt("Owners: [%s]\n\r") % j->owners;
  }

  str += "Can be worn on: ";
  str += sprintbit(j->obj_flags.wear_flags, wear_bits);
  str += "\n\r";

  str += "Set char bits : ";
  str += sprintbit(j->obj_flags.bitvector, affected_bits);
  str += "\n\r";

  str += "Extra flags   : ";
  str += sprintbit(j->getObjStat(), extra_bits);
  str += "\n\r";

  str += fmt("Can be seen   : %d\n\r") % j->canBeSeen;

  str += fmt("Volume: %d, Weight: %.1f, Value: %d, Cost/day: %d\n\r") %
    j->getVolume() % j->getWeight() %
    j->obj_flags.cost % j->rentCost();

  str += fmt("Decay: %d, Max Struct: %d, Struct Left: %d, Depreciation: %d\n\r") %
    j->obj_flags.decay_time % j->getMaxStructPoints() %
    j->getStructPoints() % j->getDepreciation();

  str += fmt("Light: %3d          Material Type: %s\n\r") %
    j->getLight() % material_nums[j->getMaterial()].mat_name;

  if (j->inRoom() != ROOM_NOWHERE)
    str += fmt("In Room: %d\n\r") % j->inRoom();
  else if (j->parent)
    str += fmt("Inside: %s\n\r") % j->parent->getName();
  else if (j->stuckIn)
    str += fmt("Stuck-In: %s (slot=%d)\n\r") %
      j->stuckIn->getName() % j->eq_stuck;
  else if (j->equippedBy)
    str += fmt("Equipped-by: %s (slot=%d)\n\r") %
      j->equippedBy->getName() % j->eq_pos;
  else
    str += "UNKNOWN LOCATION !!!!!!\n\r";

  str += fmt("Carried weight: %.1f   Carried volume: %d\n\r") %
    j->getCarriedWeight() % j->getCarriedVolume();

  str += j->statObjInfo();

  str += fmt("\n\rSpecial procedure: %s   ") % (j->spec ? objSpecials[GET_OBJ_SPE_INDEX(j->spec)].name : "none");

  if (!j->getStuff())
    str += "Contains : Nothing\n\r";
  else {
    str += "Contains :\n\r";
    for (t = j->getStuff(); t; t = t->nextThing) {
      //      str += fname(t->name);
      str += t->shortDescr;
      str += " (";
      str += t->name;
      str += ")";
      str += "\n\r";
    }
  }

  str += "Can affect char:\n\r";
  for (i = 0; i < MAX_OBJ_AFFECT; i++) {
    if (j->affected[i].location == APPLY_SPELL) {
      if (discArray[j->affected[i].modifier]) {
        str += fmt("   Affects:  %s: %s by %ld\n\r") %
          apply_types[j->affected[i].location].name %
          discArray[j->affected[i].modifier]->name %
          j->affected[i].modifier2;
      } else {
        vlogf(LOG_BUG, fmt("BOGUS AFFECT (%d) on %s") %
          j->affected[i].modifier % j->getName());
      }
    } else if (j->affected[i].location == APPLY_DISCIPLINE) {
      if (discNames[j->affected[i].modifier].disc_num) {
        str += fmt("   Affects:  %s: %s by %ld\n\r") %
          apply_types[j->affected[i].location].name %
          discNames[j->affected[i].modifier].name %
          j->affected[i].modifier2;
      } else {
        vlogf(LOG_BUG, fmt("BOGUS AFFECT (%d) on %s") %
          j->affected[i].modifier % j->getName());
      }
    } else if (j->affected[i].location == APPLY_IMMUNITY) {
      str += fmt("   Affects:  %s: %s by %ld\n\r") %
        apply_types[j->affected[i].location].name %
        immunity_names[j->affected[i].modifier] %
        j->affected[i].modifier2;
    } else if (j->affected[i].location != APPLY_NONE) {
      str += fmt("   Affects:  %s by %ld\n\r") %
        apply_types[j->affected[i].location].name %
        j->affected[i].modifier;
    }
  }
  desc->page_string(str);
  return;
}

void TBeing::statObjForDivman(const TObj *j)
{
  TThing *t;
  int i;
  sstring str = "";


  for (unsigned int zone = 0; zone < zone_table.size(); zone++) {
    if(obj_index[j->getItemIndex()].virt <= zone_table[zone].top){
      str += fmt("The item is from %s.\n\r") % zone_table[zone].name;
      break;
    }    
  }

  str += "\n\r";
  str += j->shortDescr;
  str += " is a ";
  str += ItemInfo[j->itemType()]->name;
  str += "\n\r";

  str += "It can be worn on: ";
  str += sprintbit(j->obj_flags.wear_flags, wear_bits);
  str += ".\n\r";

  str += "The item sets the character bits: ";
  str += sprintbit(j->obj_flags.bitvector, affected_bits);
  str += ".\n\r";

  str += "It's extra flags are: ";
  str += sprintbit(j->getObjStat(), extra_bits);
  str += ".\n\r";

  str += j->shortDescr;
  str += fmt(" modifies can be seen by %d.\n\r") % j->canBeSeen;

  str += fmt("It has a volume of %d, it weighs %.1f, and has a value of %d talens.\n\r") %
    j->getVolume() % j->getWeight() % j->obj_flags.cost;

  str += fmt("It will decay in %d, and its structure is %d/%d.\n\r") %
    j->obj_flags.decay_time % j->getStructPoints() %
    j->getMaxStructPoints();

  str += fmt("Light is modified by %3d and %s is made of %s.\n\r") %
    j->getLight() % j->shortDescr % material_nums[j->getMaterial()].mat_name;

  str += j->statObjInfo();

  str += fmt("\n\rIt has %s for a special procedure.\n\r") %
    (j->spec ? objSpecials[GET_OBJ_SPE_INDEX(j->spec)].name : "nothing added");

  if (!j->getStuff())
    str += "It contains nothing...\n\r";
  else {
    str += "The item contains: \n\r";
    for (t = j->getStuff(); t; t = t->nextThing) {
      str += fname(t->name);
      str += "\n\r";
    }
  }

  str += "The item can affect you by:\n\r";
  for (i = 0; i < MAX_OBJ_AFFECT; i++) {
    if (j->affected[i].location == APPLY_SPELL) {
      if (discArray[j->affected[i].modifier]) {
        str += fmt("   Affects:  %s: %s by %ld.\n\r") %
          apply_types[j->affected[i].location].name %
          discArray[j->affected[i].modifier]->name %
          j->affected[i].modifier2;
      } else {
        vlogf(LOG_BUG, fmt("BOGUS AFFECT (%d) on %s") %
          j->affected[i].modifier % j->getName());
      }
    } else if (j->affected[i].location == APPLY_DISCIPLINE) {
     if (discNames[j->affected[i].modifier].disc_num) {
        str += fmt("   Affects:  %s: %s by %ld.\n\r") %
          apply_types[j->affected[i].location].name %
          discNames[j->affected[i].modifier].name %
          j->affected[i].modifier2;
      } else {
        vlogf(LOG_BUG, fmt("BOGUS AFFECT (%d) on %s") %
          j->affected[i].modifier % j->getName());
      }
    } else if (j->affected[i].location == APPLY_IMMUNITY) {
      str += fmt("   Affects:  %s: %s by %ld.\n\r") %
        apply_types[j->affected[i].location].name %
        immunity_names[j->affected[i].modifier] %
        j->affected[i].modifier2;
    } else if (j->affected[i].location != APPLY_NONE) {
      str += fmt("   Affects:  %s by %ld.\n\r") %
        apply_types[j->affected[i].location].name %
        j->affected[i].modifier;
    }
  }
  str += "\n\r";
  str += "The cloud of smoke is quickly dispersed and the air is clear.\n\r";
  desc->page_string(str);
  return;
}

void TBeing::statBeing(TBeing *k)
{
  sstring str = "";
  sstring buf2, buf3;
  TFaction *f = NULL;
  const TMonster *km = dynamic_cast<const TMonster *>(k);
  resp *respy;
  followData *fol;
  charList *list;
  struct time_info_data playing_time;
  int objused;
  int i;

  if (!limitPowerCheck(CMD_MEDIT, k->number)) {
    sendTo("You are not allowed to stat this being, sorry.\n\r");
    return;
  }

  switch (k->player.sex) {
    case SEX_NEUTER:
      str = fmt("%sNEUTRAL-SEX%s ") % cyan() % norm();
      break;
    case SEX_MALE:
      str = fmt("%sMALE%s ") % cyan() % norm();
      break;
    case SEX_FEMALE:
      str = fmt("%sFEMALE%s ") % cyan() % norm();
      break;
  }

  bool is_player=dynamic_cast<const TPerson *>(k);

  if (km) {
    str += fmt("%s - Name : %s [M-Num: %d]\n\r     In-Room[%d] Old-Room[%d] Birth-Room[%d] V-Number[%d]\n\r") %
      (is_player ? "PC" : "NPC") %
      k->name % k->number % k->in_room % km->oldRoom % km->brtRoom %
      (k->number >= 0 ? mob_index[km->getMobIndex()].virt : -1);
  } else {
    str += fmt("%s - Name : %s [%s: %d] Room[%d]\n\r") %
      (is_player ? "PC" : "NPC") %
      k->name % (is_player ? "PID  " : "M-Num") %
      (is_player ? k->getPlayerID() : k->number) % k->in_room;
  }

  str += "-----------------------------------------------------------------------------\n\r";
  if (km) {
    str += fmt("%sShort description:%s %s\n\r") %
      cyan() % norm() %
      (km->shortDescr ? km->shortDescr : "NONE");
    str += fmt("%sLong description:%s\n\r%s") %
      cyan() % norm() %
      (km->player.longDescr ? km->player.longDescr : "NONE");
  } else {
    Descriptor *d = k->desc;

    if (d && k->isPc() && k->GetMaxLevel() > MAX_MORT) {
      str += fmt("%sIMM Office  :%s %d\n\r") % cyan() % norm() % d->office;

      if (d->blockastart) {
        str += fmt("%sIMM Block A :%s %d - %d\n\r") % cyan() % norm() % d->blockastart % d->blockaend;
      }

      if (d->blockbstart) {
        str += fmt("%sIMM Block B :%s %d - %d\n\r") % cyan() % norm() % d->blockbstart % d->blockbend;
      }
    }
  }

  buf2 = "";
  for (classIndT ijc = MIN_CLASS_IND; ijc < MAX_CLASSES; ijc++) {
    if (k->hasClass(1<<ijc)) {
      buf2 += classInfo[ijc].name.cap();
      buf2 += " ";
    }
  }

  str += fmt("%sClass       :%s %-28s\n\r") % cyan() % norm() % buf2;

  str += fmt("%sLevel       :%s [") % cyan() % norm();
  for (classIndT i = MIN_CLASS_IND; i < MAX_CLASSES; i++) {
    str += fmt("%c%c:%d ") %
      classInfo[i].name.cap()[0] % classInfo[i].name.cap()[1] %
      k->getLevel(i);
  }
  str += "]\n\r";

  str += fmt("%sRace        :%s %-10s") %
    cyan() % norm() % k->getMyRace()->getSingularName();

  str += fmt("%sHome:%s %-17s") % 
    cyan() % norm() % home_terrains[k->player.hometerrain];

  if (k->desc && k->desc->account) {
    if (IS_SET(k->desc->account->flags, ACCOUNT_IMMORTAL) &&
        !hasWizPower(POWER_VIEW_IMM_ACCOUNTS)) {
      str += "Account : *** Information Concealed ***\n\r";
    } else {
      str += fmt("%sAccount: %s%s\n\r") % purple() %
        k->desc->account->name % norm();
    }
  } else {
    str += "\n\r";
  }

  if (k->isPc()) {
    sstring birth_buf, logon_buf;

    birth_buf = asctime(localtime(&(k->player.time.birth)));
    // Chop off trailing \n from the output of localtime
    birth_buf = birth_buf.substr(0, birth_buf.length() - 1);

    logon_buf = asctime(localtime(&(k->player.time.logon)));
    // Chop off trailing \n from the output of localtime
    logon_buf = logon_buf.substr(0, logon_buf.length() - 1);

    realTimePassed((time(0) - k->player.time.logon) +
                    k->player.time.played, 0, &playing_time);
    str += fmt("%sBirth       : %s%s     %sLogon:%s %s\n\r") %
      cyan() % norm() % birth_buf % cyan() % norm() % logon_buf;
    str += fmt("%sPlaying time:%s %d days, %d hours.      %sBase age:%s %d    %sAge Mod:%s %d\n\r") %
      cyan() % norm() % playing_time.day % playing_time.hours %
      cyan() % norm() % k->getBaseAge() %
      cyan() % norm() % k->age_mod;
    if (!k->desc) {
      str += fmt("%sWARNING%s, player is offline, age will not be accurate.\n\r") % red() % norm();
    }

    str += fmt("%sPlayer age  :%s %d years, %d months, %d days, %d hours\n\r\n\r") %
      cyan() % norm() % k->age()->year % k->age()->month %
      k->age()->day % k->age()->hours;
  }

  buf3 = fmt("[%5.2f]") % k->getExp();
  buf2 = fmt("[%5.2f]") % k->getMaxExp();
  str += fmt("%sDef Rnd:%s [%5d]   %sExp      :%s %-16s %sMax Exp :%s %-13s\n\r") %
    cyan() % norm() % k->defendRound(NULL) %
    cyan() % norm() % buf3 %
    cyan() % norm() % buf2;

  buf2 = fmt("[%5d]") % k->getMoney();
  buf3 = fmt("[%5d]") % k->getBank();
  str += fmt("%sVision :%s [%5d]   %sTalens   :%s %-16s %sBank Bal:%s %-13s\n\r") %
    cyan() % norm() % k->visionBonus %
    cyan() % norm() % buf2 %
    cyan() % norm() % buf3;

  buf2 = fmt("[%5d]") % k->getHitroll();
  buf3 = fmt("[%5d]") % k->getDamroll();
  str += fmt("%sAtt Rnd:%s [%5d]   %sHitroll  :%s %-16s %sDamroll :%s %-13s\n\r") %
    cyan() % norm() % k->attackRound(NULL) %
    cyan() % norm() % buf2 %
    cyan() % norm() % buf3;

  str += fmt("%sPiety  :%s [%5.1f]   %sLifeForce:%s [%5d]\n\r") %
    cyan() % norm() % k->getPiety() %
    cyan() % norm() % k->getLifeforce();

  buf2 = fmt("[%5d]") % k->getHit();
  buf3 = fmt("[%5d]") % k->getMove();
  str += fmt("%sMana   :%s [%5d]   %sHP       :%s %-16s %sMove    :%s %-13s\n\r") %
    cyan() % norm() % k->getMana() %
    cyan() % norm() % buf2 %
    cyan() % norm() % buf3;

  buf2 = fmt("[%5d]") % k->hitLimit();
  buf3 = fmt("[%5d]") % k->moveLimit();
  str += fmt("%sMaxMana:%s [%5d]   %sMax HP   :%s %-16s %sMax Move:%s %-13s\n\r") %
    cyan() % norm() % k->manaLimit() %
    cyan() % norm() % buf2 %
    cyan() % norm() % buf3;

  if(dynamic_cast<TPerson *>(k)){
    buf2 = fmt("[%5d]") % ageHpMod(dynamic_cast<TPerson *>(k));
    buf3 = fmt("[%f]") % k->getConHpModifier();
    str += fmt("%sEq HP  :%s [%5d]   %sAge HP   :%s %-16s %sConHpMod:%s %-13s\n\r") %
      cyan() % norm() % eqHpBonus(dynamic_cast<TPerson *>(k)) %
      cyan() % norm() % buf2 %
      cyan() % norm() % buf3;
  }

  buf2 = fmt("[%5d]") % k->visibility();
  buf3 = fmt("[%5.1f]") % k->getWeight();
  str += fmt("%sHeight :%s [%5d]   %sWt. (lbs):%s %-14s %sVisibility:%s %-13s\n\r") %
    cyan() % norm() % k->getHeight() %
    cyan() % norm() % buf3 %
    cyan() % norm() % buf2;

  buf2 = fmt("[%5d]") % k->specials.conditions[FULL];
  buf3 = fmt("[%5d]") % k->specials.conditions[DRUNK];
  str += fmt("%sThirst :%s [%5d]   %sHunger   :%s %-16s %sDrunk   :%s %-13s\n\r") %
    cyan() % norm() % k->specials.conditions[THIRST] %
    cyan() % norm() % buf2 %
    cyan() % norm() % buf3;

  str += fmt("%sPee    :%s [%5d]   %sPoop     :%s [%5d]\n\r") %
    cyan() % norm() % k->specials.conditions[PEE] %
    cyan() % norm() % k->specials.conditions[POOP];

  buf2 = fmt("[%5d]") % k->getArmor();
  buf3 = fmt("[%5d]") % noise(k);
  str += fmt("%sLight  :%s [%5d]   %sNoise    :%s %-16s %sArmor   :%s %-13s\n\r") %
    cyan() % norm() % k->getLight() %
    cyan() % norm() % buf3 %
    cyan() % norm() % buf2;

  buf2 = fmt("[%5d]") % k->eyeSight(k->roomp);
  buf3 = fmt("[%5d]") % k->getSpellHitroll();
  str += fmt("%sProt.  :%s [%5d]   %sEyesight :%s %-11s %sSpell Hitroll:%s %-13s\n\r") %
    cyan() % norm() % k->getProtection() %
    cyan() % norm() % buf2 %
    cyan() % norm() % buf3;

  if (km && !(polyed == POLY_TYPE_DISGUISE)) {
    str += fmt("%sNumber of attacks :%s %.1f") %
      cyan() % norm() % km->getMult();
    str += fmt("        %sNPC Damage:%s %.1f+%d%%.\n\r") %
      cyan() % norm() % km->getDamLevel() % km->getDamPrecision();
    double bd = km->baseDamage();
    int chg = (int) (bd * km->getDamPrecision() / 100);
    str += fmt("%sNPC Damage range  :%s %d-%d.\n\r") % cyan() % norm() %
      max(1, (int) bd-chg) % max(1, (int) bd+chg);
  } else {
    if (k->hasClass(CLASS_MONK)) {
      str += fmt("%sNumber of attacks:%s %.2f\n\r") %
        cyan() % norm() % k->getMult();
    }

    float fx, fy;
    k->blowCount(false, fx, fy);
    str += fmt("%sPrim attacks:%s %.2f, %sOff attacks:%s %.2f\n\r") %
      cyan() % norm() % fx %
      cyan() % norm() % fy;

    int dam=0;
    int prim_min=9999, prim_max=0;
    int sec_min=9999, sec_max=0;
    for(i=0;i<100;++i){
      dam=k->getWeaponDam(this, k->heldInPrimHand(), HAND_PRIMARY);
      if(dam<prim_min)
	prim_min=dam;
      if(dam>prim_max)
	prim_max=dam;

      dam=k->getWeaponDam(this, k->heldInSecHand(), HAND_SECONDARY);
      if(dam<sec_min)
	sec_min=dam;
      if(dam>sec_max)
	sec_max=dam;
    }

    str += fmt("%sPrim damage:%s %i-%i, %sOff damage:%s %i-%i\n\r") %
      cyan() % norm() % prim_min % prim_max %
      cyan() % norm() % sec_min % sec_max;

    str += fmt("%sApproximate damage per round:%s %i-%i\n\r") %
      cyan() % norm() %
      (int)((fx*(float)prim_min)+((fy*(float)sec_min))) %
      (int)((fx*(float)prim_max)+((fy*(float)sec_max)));
  }
  if (toggleInfo[TOG_TESTCODE5]->toggle && k->newfaction()) {
    if(k->isPc()) {
      str += fmt("%sFaction:%s %s%s,   %sRank :%s %s%s\n\r") %
        cyan() % norm() % k->newfaction()->getName() % norm() %
        cyan() % norm() % k->rank() % norm();
    }
  } else {
    str += fmt("%sFaction:%s %s,   %sFaction Percent:%s %.4f\n\r") %
      cyan() % norm() % FactionInfo[k->getFaction()].faction_name %
      cyan() % norm() % k->getPerc();
#if FACTIONS_IN_USE
    str += fmt("%sPerc_0:%s %.4f   %sPerc_1:%s %.4f   %sPerc_2:%s %.4f   %sPerc_3:%s %.4f\n\r") %
      cyan() % norm() % k->getPercX(FACT_NONE) %
      cyan() % norm() % k->getPercX(FACT_BROTHERHOOD) %
      cyan() % norm() % k->getPercX(FACT_CULT) %
      cyan() % norm() % k->getPercX(FACT_SNAKE);
#endif
  }
//  str += fmt("%sFaction :%s %s\n\r") %
//    cyan() % norm() % FactionInfo[k->getFaction()].faction_name;

  str += "Stats    :";
  str += k->chosenStats.printStatHeader();

  statTypeT ik;

  str += "Race     :";
  for(ik = MIN_STAT; ik<MAX_STATS_USED; ik++) {
    str += fmt(" %3d ") % k->getStat(STAT_RACE, ik);
  }
  str += "\n\r";

  str += "Chosen   :";
  for(ik = MIN_STAT; ik<MAX_STATS_USED; ik++) {
    str += fmt(" %3d ") % k->getStat(STAT_CHOSEN, ik);
  }
  str += "\n\r";

  str += "Age      :";
  for(ik = MIN_STAT; ik<MAX_STATS_USED; ik++) {
    str += fmt(" %3d ") % k->getStat(STAT_AGE, ik);
  }
  str += "\n\r";

  str += "Territory:";
  for(ik = MIN_STAT; ik<MAX_STATS_USED; ik++) {
    str += fmt(" %3d ") % k->getStat(STAT_TERRITORY, ik);
  }
  str += "\n\r";

  str += "Natural  :";
  for(ik=MIN_STAT; ik<MAX_STATS_USED; ik++) {
    str += fmt(" %3d ") % k->getStat(STAT_NATURAL, ik);
  }
  str += "\n\r";
    
  str += "Current  :";
  for(ik = MIN_STAT; ik<MAX_STATS_USED; ik++) {
    str += fmt(" %3d ") % k->getStat(STAT_CURRENT, ik);
  }
  str += "\n\r";

  // only show captive info when needed
  if (k->getCaptiveOf() || k->getCaptive()) {
    str += fmt("%sCaptive Of:%s %s         %sCaptives :%s ") %
       cyan() % norm() %
       (k->getCaptiveOf() ? k->getCaptiveOf()->getName() : "NO ONE") %
       cyan() % norm();
    if (!k->getCaptive())
      str += "NONE\n\r";
    else {
      TBeing *x1;
      for (x1 = k->getCaptive(); x1; x1 = x1->getNextCaptive()) {
        str += x1->getName();
        str += " ";
      }
      str += "\n\r";
    }
  }
  str += fmt("Master is '%s'") %
          ((k->master) ? k->master->getName() : "NOBODY");
  str += "           Followers are:";
  for (fol = k->followers; fol; fol = fol->next) {
    str += fol->follower->getName();
  }
  str += "\n\r";

  if (km) {
    buf2 = sprinttype(km->getPosition(), position_types);
    buf3 = sprinttype((km->default_pos), position_types);
    str += fmt("%sPosition:%s %s  %sFighting:%s %s  %sDefault Position:%s %s\n\r") %
      cyan() % norm() % buf2 %
      cyan() % norm() % (km->fight() ? km->fight()->getName() : "Nobody") %
      cyan() % norm() % buf3;

    str += fmt("%sNPC flags:%s ") % cyan() % norm();
    if (km->specials.act) {
      str += sprintbit(km->specials.act, action_bits);
      str += "\n\r";
    } else {
      str += "None\n\r";
    }
  } else {
    buf2 = sprinttype(k->getPosition(), position_types);
    str += fmt("%sPosition:%s %s  %sFighting:%s %s\n\r") %
          cyan() % norm() % buf2 %
          cyan() % norm() % (k->fight() ? k->fight()->getName() : "Nobody");
  } 
  if (k->desc) {
    str += fmt("\n\r%sFlags (Specials Act):%s ") % cyan() % norm();
    str += sprintbit(k->desc->plr_act, player_bits);
    str += "\n\r";
  }

  str += fmt("%sCarried weight:%s %.1f   %sCarried volume:%s %d\n\r") %
          cyan() % norm() % k->getCarriedWeight() %
          cyan() % norm() % k->getCarriedVolume();

  immuneTypeT ij;
  for (ij = MIN_IMMUNE;ij < MAX_IMMUNES; ij++) {
    if (k->getImmunity(ij) == 0 || immunity_names[ij].empty())
      continue;
    if (k->getImmunity(ij) > 0)
      buf2 = fmt("%d%% resistant to %s.\n\r") %
        k->getImmunity(ij) % immunity_names[ij];
    if (k->getImmunity(ij) < 0)
      buf2 = fmt("%d%% susceptible to %s.\n\r") %
        -k->getImmunity(ij) % immunity_names[ij];
    str += buf2;
  }

  if (!k->isPc()) {
    const TMonster *tmons = dynamic_cast<const TMonster *>(k);
    str += fmt("%sAction pointer:%s %s") % 
      cyan() % norm() %
      ((tmons->act_ptr ? "YES" : "no") );
    str += fmt("    %sSpecial Procedure:%s %s\n\r") %
      cyan() % norm() %
      ((tmons->spec) ? mob_specials[GET_MOB_SPE_INDEX(tmons->spec)].name : "none");
    str += fmt("%sAnger:%s %d/%d     %sMalice:%s %d/%d     %sSuspicion:%s %d/%d   %sGreed:%s %d/%d\n\r") %
      cyan() % norm() %
      tmons->anger() % tmons->defanger() %
      cyan() % norm() %
      tmons->malice() % tmons->defmalice() %
      cyan() % norm() %
      tmons->susp() % tmons->defsusp() %
      cyan() % norm() %
      tmons->greed() % tmons->defgreed();
    str += fmt("%sHates:%s " ) % cyan() % norm();
    if (IS_SET(tmons->hatefield, HATE_CHAR)) {
      if (tmons->hates.clist) {
        for (list = tmons->hates.clist; list; list = list->next) {
          if (list->name) {
            str += list->name;
            str += " ";
          }
        }
      }
    }
    if (IS_SET(tmons->hatefield, HATE_RACE)) {
      if (tmons->hates.race != -1) {
        str += Races[tmons->hates.race]->getSingularName();
        str += "(Race) ";
      }
    }
    if (IS_SET(tmons->hatefield, HATE_SEX)) {
      switch (tmons->hates.sex) {
        case SEX_NEUTER:
          str += "SEX_NEUTER ";
          break;
        case SEX_MALE:
          str += "SEX_MALE ";
          break;
        case SEX_FEMALE:
          str += "SEX_FEMALE ";
          break;
      }
    }
    str += "    ";

    str += fmt("%sFears:%s " ) % cyan() % norm();
    if (IS_SET(tmons->fearfield, FEAR_CHAR)) {
      if (tmons->fears.clist) {
        for (list = tmons->fears.clist; list; list = list->next) {
          if (list->name) {
            str += list->name;
            str += " ";
          }
        }
      }
    }
    if (IS_SET(tmons->fearfield, FEAR_RACE)) {
      if (tmons->fears.race != -1) {
        str += Races[tmons->fears.race]->getSingularName();
        str += "(Race) ";
      }
    }
    if (IS_SET(tmons->fearfield, FEAR_SEX)) {
      switch (tmons->fears.sex) {
        case SEX_NEUTER:
          str += "SEX_NEUTER ";
          break;
        case SEX_MALE:
          str += "SEX_MALE ";
          break;
        case SEX_FEMALE:
          str += "SEX_FEMALE ";
          break;
      }
    }
    if (IS_SET(tmons->fearfield, FEAR_CLASS)) {
       str += fmt("CLASS=%d ") % tmons->fears.Class;
    }
    if (IS_SET(tmons->fearfield, FEAR_VNUM)) {
       str += fmt("VNUM=%d ") % tmons->fears.vnum;
    }
    if (IS_SET(tmons->specials.act, ACT_HUNTING)) {
      str += fmt("\n\r%sHunting:%s %s  %spersist:%s %d  %sorigin:%s %d  %shunt distance:%s %d") %
        cyan() % norm() %
        (tmons->specials.hunting ? tmons->specials.hunting->getName() : "Unknown") %
        cyan() % norm() %
        tmons->persist %
        cyan() % norm() %
        tmons->oldRoom %
        cyan() % norm() %
        tmons->hunt_dist;
    } else if (tmons->specials.hunting) {
      str += fmt("\n\r%sTracking:%s %s  %spersist:%s %d  %sorigin:%s %d  %srange:%s %d") %
        cyan() % norm() %
        tmons->specials.hunting->getName() %
        cyan() % norm() %
        tmons->persist %
        cyan() % norm() %
        tmons->oldRoom %
        cyan() % norm() %
        tmons->hunt_dist;
    }
    str += fmt("\n\r%sAI Target:%s %s  %sRandom:%s %s") %
      cyan() % norm() %
      (tmons->targ() ? tmons->targ()->getName() : "-") %
      cyan() % norm() %
      (tmons->opinion.random ? tmons->opinion.random->getName() : "-");
    str += "\n\r";
  } else {
    // PCs only
    if (k->specials.hunting) {
      str += fmt("%sHunting:%s %s\n\r") %
        cyan() % norm() %
        k->specials.hunting->getName();
    }
    const TPerson *tper = dynamic_cast<const TPerson *>(k);
    if (tper) {
      str += fmt("%sTitle:%s\n\r%s%s\n\r") %
        cyan() % norm() % tper->title % norm();
    }
  }

  str += fmt("%sAffected by:%s ") % cyan() % norm();
  str += sprintbit(k->specials.affectedBy, affected_bits);
  str += "\n\r\n\r";

  str += fmt("%sBody part          Hth Max Flgs  StuckIn%s\n\r") %
    cyan() % norm();
  str += fmt("%s----------------------------------------%s\n\r") %
    cyan() % norm();
  wearSlotT il;
  for (il = MIN_WEAR; il < MAX_WEAR; il++) {
    if (il == HOLD_RIGHT || il == HOLD_LEFT)
      continue;
    if (k->slotChance(il)) {
      buf2 = fmt("[%s]") % k->describeBodySlot(il);
      str += fmt("%-18s %-3d %-3d %-5d %s\n\r") %
        buf2 % k->getCurLimbHealth(il) %
        k->getMaxLimbHealth(il) %
        k->getLimbFlags(il) %
        (k->getStuckIn(il) ? k->getStuckIn(il)->getName() : "None");
    }
  }

  if (km) {
    if (km->resps && km->resps->respList) {
      str += fmt("%sResponse(s):\n\r------------%s\n\r") % cyan() % norm();
      for (respy = km->resps->respList; respy; respy = respy->next) {
        if (respy->cmd < MAX_CMD_LIST) {
          str += fmt("%s %s\n\r") % commandArray[respy->cmd]->name %
            respy->args;
        } else if (respy->cmd == CMD_RESP_ROOM_ENTER) {
          str += "roomenter\n\r";
        } else if (respy->cmd == CMD_RESP_PACKAGE) {
          str += fmt("package %s\n\r") % respy->args;
        } else {
          str += fmt("%d %s\n\r") % respy->cmd % respy->args;
        }
      }
      str += fmt("%s------------%s\n\r") % cyan() % norm();

      if (km->resps->respMemory) {
        str += fmt("%sResponse Memory:\n\r----------------\n\r%s") %
          cyan() % norm();

        for (RespMemory *rMem = km->resps->respMemory; rMem; rMem = rMem->next)
          if (rMem->cmd < MAX_CMD_LIST) {
            str += fmt("%s %s %s\n\r") %
              (rMem->name ? rMem->name : "Unknown") %
              commandArray[rMem->cmd]->name %
              (rMem->args ? rMem->args : "");
	  } else if (rMem->cmd == CMD_RESP_ROOM_ENTER) {
            str += fmt("%s %s %s\n\r") %
              (rMem->name ? rMem->name : "Unknown") %
              "roomenter" %
              (rMem->args ? rMem->args : "");
          } else {
            str += fmt("%s %d %s\n\r") %
              (rMem->name ? rMem->name : "Unknown") %
              rMem->cmd %
              (rMem->args ? rMem->args : "");
          }

        str += fmt("----------------\n\r%s") % cyan() % norm();
      }
    } else
      str += fmt("%sResponse(s):%s None.\n\r") % cyan() % norm();
  }

  str += fmt("\n\r%sAffecting Spells:\n\r-----------------%s\n\r") %
    cyan() % norm();
  affectedData *aff, *af2;
  for (aff = k->affected; aff; aff = af2) {
    // technically, shouldn't need to save next, but apparently
    // some operations below "might" cause aff to be deleted
    // not sure which ones though - Bat 4/28/98
    af2 = aff->next;

    switch (aff->type) {
      case SKILL_TRACK:
      case SKILL_SEEKWATER:
      case SPELL_GUST:
      case SPELL_DUST_STORM:
      case SPELL_TORNADO:
      case SKILL_QUIV_PALM:
      case SKILL_SHOULDER_THROW:
      case SPELL_CALL_LIGHTNING_DEIKHAN:
      case SPELL_CALL_LIGHTNING:
      case SPELL_LIGHTNING_BREATH:
      case SPELL_GUSHER:
      case SPELL_AQUALUNG:
      case SPELL_AQUATIC_BLAST:
      case SPELL_CARDIAC_STRESS:
      case SPELL_ICY_GRIP:
      case SPELL_ARCTIC_BLAST:
      case SPELL_ICE_STORM:
      case SPELL_FROST_BREATH:
      case SPELL_WATERY_GRAVE:
      case SPELL_TSUNAMI:
      case SPELL_CHLORINE_BREATH:
      case SPELL_DUST_BREATH:
      case SPELL_POISON_DEIKHAN:
      case SPELL_POISON:
      case SPELL_ACID_BREATH:
      case SPELL_ACID_BLAST:
      case SKILL_BODYSLAM:
      case SKILL_SPIN:
      case SKILL_CHARGE:
      case SKILL_SMITE:
      case SPELL_METEOR_SWARM:
      case SPELL_EARTHQUAKE_DEIKHAN:
      case SPELL_EARTHQUAKE:
      case SPELL_PILLAR_SALT:
      case SPELL_FIREBALL:
      case SPELL_HANDS_OF_FLAME:
      case SPELL_INFERNO:
      case SPELL_HELLFIRE:
      case SPELL_RAIN_BRIMSTONE_DEIKHAN:
      case SPELL_RAIN_BRIMSTONE:
      case SPELL_FLAMESTRIKE:
      case SPELL_FIRE_BREATH:
      case SPELL_SPONTANEOUS_COMBUST:
      case SPELL_FLAMING_SWORD:
      case SPELL_FLARE:
      case SPELL_MYSTIC_DARTS:
      case SPELL_STUNNING_ARROW:
      case SPELL_COLOR_SPRAY:
      case SPELL_SAND_BLAST:
      case SPELL_PEBBLE_SPRAY:
      case SPELL_LAVA_STREAM:
      case SPELL_DEATH_MIST:
      case SPELL_FLATULENCE:
      case SPELL_SLING_SHOT:
      case SPELL_GRANITE_FISTS:
      case SPELL_STICKS_TO_SNAKES:
      case SPELL_DISTORT: // shaman
      case SPELL_DEATHWAVE: // shaman
      case SPELL_SOUL_TWIST: // shaman
      case SPELL_SQUISH: // shaman
      case SPELL_ENERGY_DRAIN:
      case SPELL_LICH_TOUCH: // shaman
      case SPELL_SYNOSTODWEOMER:
      case SPELL_HARM_DEIKHAN:
      case SPELL_HARM:
      case SPELL_HARM_LIGHT_DEIKHAN:
      case SPELL_HARM_SERIOUS_DEIKHAN:
      case SPELL_HARM_CRITICAL_DEIKHAN:
      case SPELL_HARM_LIGHT:
      case SPELL_HARM_SERIOUS:
      case SPELL_HARM_CRITICAL:
      case SPELL_WITHER_LIMB:
      case SPELL_BLEED:
      case SKILL_KICK_DEIKHAN:
      case SKILL_KICK_THIEF:
      case SKILL_KICK_MONK:
      case SKILL_KICK:
      case SKILL_SPRINGLEAP:
      case SKILL_DEATHSTROKE:
      case SKILL_BASH_DEIKHAN:
      case SKILL_BASH:
      case SPELL_BONE_BREAKER:
      case SPELL_PARALYZE:
      case SPELL_PARALYZE_LIMB:
      case SPELL_INFECT_DEIKHAN:
      case SPELL_INFECT:
      case SKILL_CHOP:
      case SPELL_DISEASE:
      case SPELL_SUFFOCATE:
      case SKILL_GARROTTE:
      case SKILL_STABBING:
      case SKILL_BACKSTAB:
      case SKILL_THROATSLIT:
      case SKILL_HEADBUTT:
      case SKILL_STOMP:
      case SKILL_BRAWL_AVOIDANCE:
      case SPELL_BLAST_OF_FURY:
      case SKILL_CHI:
      case SKILL_TRIP:
      case SPELL_FUMBLE:
      case SPELL_BLINDNESS:
      case SPELL_GARMULS_TAIL:
      case SPELL_SORCERERS_GLOBE:
      case SPELL_FAERIE_FIRE:
      case SPELL_STUPIDITY:
      case SPELL_ILLUMINATE:
      case SPELL_DETECT_MAGIC:
      case SPELL_MATERIALIZE:
      case SPELL_BLOOD_BOIL:
      case SPELL_DJALLA:
      case SPELL_LEGBA:
      case SPELL_PROTECTION_FROM_EARTH:
      case SPELL_PROTECTION_FROM_AIR:
      case SPELL_PROTECTION_FROM_FIRE:
      case SPELL_PROTECTION_FROM_WATER:
      case SPELL_PROTECTION_FROM_ELEMENTS:
      case SPELL_INFRAVISION:
      case SPELL_IDENTIFY:
      case SPELL_POWERSTONE:
      case SPELL_FAERIE_FOG:
      case SPELL_TELEPORT:
      case SPELL_KNOT:
      case SPELL_SENSE_LIFE:
      case SPELL_SENSE_LIFE_SHAMAN: // shaman
      case SPELL_CALM:
      case SPELL_ACCELERATE:
      case SPELL_CHEVAL: // shaman
      case SPELL_CELERITE:
      case SPELL_LEVITATE:
      case SPELL_FEATHERY_DESCENT:
      case SPELL_STEALTH:
      case SPELL_GILLS_OF_FLESH:
      case SPELL_TELEPATHY:
      case SPELL_ROMBLER: // shaman
      case SPELL_INTIMIDATE: //shaman
      case SPELL_CLEANSE: // shaman
      case SPELL_FEAR:
      case SPELL_SLUMBER:
      case SPELL_CONJURE_EARTH:
      case SPELL_CONJURE_AIR:
      case SPELL_CONJURE_FIRE:
      case SPELL_CONJURE_WATER:
      case SPELL_DISPEL_MAGIC:
      case SPELL_CHASE_SPIRIT: // shaman
      case SPELL_ENHANCE_WEAPON:
      case SPELL_GALVANIZE:
      case SPELL_DETECT_INVISIBLE:
      case SPELL_DETECT_SHADOW: // shaman
      case SPELL_DISPEL_INVISIBLE:
      case SPELL_FARLOOK:
      case SPELL_FALCON_WINGS:
      case SPELL_INVISIBILITY:
      case SPELL_ENSORCER:
      case SPELL_EYES_OF_FERTUMAN:
      case SPELL_COPY:
      case SPELL_HASTE:
      case SPELL_IMMOBILIZE:
      case SPELL_FLY:
      case SPELL_ANTIGRAVITY:
      case SPELL_DIVINATION:
      case SPELL_SHATTER:
      case SKILL_SCRIBE:
      case SPELL_SPONTANEOUS_GENERATION:
      case SPELL_STONE_SKIN:
      case SPELL_TRAIL_SEEK:
      case SPELL_FLAMING_FLESH:
      case SPELL_ATOMIZE:
      case SPELL_ANIMATE:
      case SPELL_BIND:
      case SPELL_ENLIVEN:
      case SPELL_TRUE_SIGHT:
      case SPELL_CLOUD_OF_CONCEALMENT:
      case SPELL_POLYMORPH:
      case SPELL_SILENCE:
      case SPELL_BREATH_OF_SARAHAGE:
      case SPELL_PLASMA_MIRROR:
      case SPELL_THORNFLESH:
      case SPELL_ETHER_GATE:
      case SPELL_HEAL_LIGHT:
      case SPELL_HEALING_GRASP:
      case SPELL_CREATE_FOOD:
      case SPELL_CREATE_WATER:
      case SPELL_ARMOR:
      case SPELL_BLESS:
      case SPELL_CLOT:
      case SPELL_HEAL_SERIOUS:
      case SPELL_STERILIZE:
      case SPELL_EXPEL:
      case SPELL_CURE_DISEASE:
      case SPELL_CURSE:
      case SPELL_REMOVE_CURSE:
      case SPELL_CURE_POISON:
      case SPELL_HEAL_CRITICAL:
      case SPELL_SALVE:
      case SPELL_REFRESH:
      case SPELL_NUMB:
      case SPELL_PLAGUE_LOCUSTS:
      case SPELL_CURE_BLINDNESS:
      case SPELL_SUMMON:
      case SPELL_HEAL:
      case SPELL_WORD_OF_RECALL:
      case SPELL_SANCTUARY:
      case SPELL_RELIVE:
      case SPELL_CURE_PARALYSIS:
      case SPELL_SECOND_WIND:
      case SPELL_HEROES_FEAST:
      case SPELL_ASTRAL_WALK:
      case SPELL_PORTAL:
      case SPELL_HEAL_FULL:
      case SPELL_HEAL_CRITICAL_SPRAY:
      case SPELL_HEAL_SPRAY:
      case SPELL_HEAL_FULL_SPRAY:
      case SPELL_RESTORE_LIMB:
      case SPELL_KNIT_BONE:
      case SKILL_RESCUE:
      case SKILL_BLACKSMITHING:
      case SKILL_REPAIR_MAGE:
      case SKILL_REPAIR_MONK:
      case SKILL_REPAIR_CLERIC:
      case SKILL_REPAIR_DEIKHAN:
      case SKILL_REPAIR_SHAMAN:
      case SKILL_REPAIR_THIEF:
      case SKILL_BLACKSMITHING_ADVANCED:
      case SKILL_MEND:
      case SKILL_DISARM:
      case SKILL_DUAL_WIELD:
      case SKILL_POWERMOVE:
      case SKILL_PARRY_WARRIOR:
      case SKILL_BERSERK:
      case SKILL_SWITCH_OPP:
      case SKILL_KNEESTRIKE:
      case SKILL_SHOVE:
      case SKILL_RETREAT:
      case SKILL_GRAPPLE:
      case SKILL_DOORBASH:
      case SKILL_TRANCE_OF_BLADES:
      case SKILL_WEAPON_RETENTION:
      case SKILL_CLOSE_QUARTERS_FIGHTING:
      case SKILL_HIKING:
      case SKILL_FORAGE:
      case SKILL_TRANSFORM_LIMB:
      case SKILL_BEAST_SOOTHER:
      case SPELL_ROOT_CONTROL:
      case SKILL_BEFRIEND_BEAST:
      case SKILL_TRANSFIX:
      case SKILL_SKIN:
      case SKILL_BUTCHER:
      case SPELL_LIVING_VINES:
      case SKILL_BEAST_SUMMON:
      case SKILL_BARKSKIN:
      case SPELL_ENTHRALL_SPECTRE:
      case SPELL_ENTHRALL_GHAST:
      case SPELL_ENTHRALL_GHOUL:
      case SPELL_ENTHRALL_DEMON:
      case SPELL_CREATE_WOOD_GOLEM:
      case SPELL_CREATE_ROCK_GOLEM:
      case SPELL_CREATE_IRON_GOLEM:
      case SPELL_CREATE_DIAMOND_GOLEM:
      case SPELL_STORMY_SKIES:
      case SPELL_TREE_WALK:
      case SKILL_BEAST_CHARM:
      case SPELL_SHAPESHIFT:
      case SPELL_CHRISM:
      case SPELL_CLARITY:
      case SKILL_CONCEALMENT:
      case SKILL_APPLY_HERBS:
      case SKILL_DIVINATION:
      case SKILL_ENCAMP:
      case SPELL_HEAL_LIGHT_DEIKHAN:
      case SKILL_CHIVALRY:
      case SPELL_ARMOR_DEIKHAN:
      case SPELL_BLESS_DEIKHAN:
      case SPELL_EXPEL_DEIKHAN:
      case SPELL_CLOT_DEIKHAN:
      case SPELL_STERILIZE_DEIKHAN:
      case SPELL_REMOVE_CURSE_DEIKHAN:
      case SPELL_CURSE_DEIKHAN:
      case SKILL_RESCUE_DEIKHAN:
      case SPELL_CURE_DISEASE_DEIKHAN:
      case SPELL_CREATE_FOOD_DEIKHAN:
      case SPELL_HEAL_SERIOUS_DEIKHAN:
      case SPELL_CURE_POISON_DEIKHAN:
      case SKILL_DISARM_DEIKHAN:
      case SPELL_HEAL_CRITICAL_DEIKHAN:
      case SKILL_SWITCH_DEIKHAN:
      case SKILL_RETREAT_DEIKHAN:
      case SKILL_SHOVE_DEIKHAN:
      case SKILL_RIDE:
      case SKILL_ALCOHOLISM:
      case SKILL_FISHING:
      case SKILL_LOGGING:
      case SKILL_CALM_MOUNT:
      case SKILL_TRAIN_MOUNT:
      case SKILL_ADVANCED_RIDING:
      case SKILL_RIDE_DOMESTIC:
      case SKILL_RIDE_NONDOMESTIC:
      case SKILL_RIDE_WINGED:
      case SPELL_CREATE_WATER_DEIKHAN:
      case SKILL_RIDE_EXOTIC:
      case SPELL_HEROES_FEAST_DEIKHAN:
      case SPELL_REFRESH_DEIKHAN:
      case SPELL_SALVE_DEIKHAN:
      case SKILL_LAY_HANDS:
      case SPELL_NUMB_DEIKHAN:
      case SKILL_YOGINSA:
      case SKILL_CINTAI:
      case SKILL_OOMLAT:
      case SKILL_ADVANCED_KICKING:
      case SKILL_DISARM_MONK:
      case SKILL_GROUNDFIGHTING:
      case SKILL_DUFALI:
      case SKILL_RETREAT_MONK:
      case SKILL_SNOFALTE:
      case SKILL_COUNTER_MOVE:
      case SKILL_SWITCH_MONK:
      case SKILL_JIRIN:
      case SKILL_KUBO:
      case SKILL_CATFALL:
      case SKILL_CATLEAP:
      case SKILL_WOHLIN:
      case SKILL_VOPLAT:
      case SKILL_BLINDFIGHTING:
      case SKILL_CRIT_HIT:
      case SKILL_FEIGN_DEATH:
      case SKILL_BLUR:
      case SKILL_CHAIN_ATTACK:
      case SKILL_HURL:
      case SKILL_SWINDLE:
      case SKILL_SNEAK:
      case SKILL_RETREAT_THIEF:
      case SKILL_PICK_LOCK:
      case SKILL_SEARCH:
      case SKILL_SPY:
      case SKILL_SWITCH_THIEF:
      case SKILL_STEAL:
      case SKILL_DETECT_TRAP:
      case SKILL_SUBTERFUGE:
      case SKILL_DISARM_TRAP:
      case SKILL_CUDGEL:
      case SKILL_HIDE:
      case SKILL_POISON_WEAPON:
      case SKILL_DISGUISE:
      case SKILL_DODGE_THIEF:
      case SKILL_SET_TRAP_CONT:
      case SKILL_SET_TRAP_DOOR:
      case SKILL_SET_TRAP_MINE:
      case SKILL_SET_TRAP_GREN:
      case SKILL_SET_TRAP_ARROW:
      case SKILL_DUAL_WIELD_THIEF:
      case SKILL_DISARM_THIEF:
      case SKILL_COUNTER_STEAL:
      case SPELL_DANCING_BONES:
      case SPELL_CONTROL_UNDEAD:
      case SPELL_RESURRECTION:
      case SPELL_VOODOO:
      case SKILL_BREW:
      case SPELL_VAMPIRIC_TOUCH:
      case SPELL_LIFE_LEECH:
      case SKILL_TURN:
      case SKILL_SIGN:
      case SKILL_SWIM:
      case SKILL_CONS_UNDEAD:
      case SKILL_CONS_VEGGIE:
      case SKILL_CONS_DEMON:
      case SKILL_CONS_ANIMAL:
      case SKILL_CONS_REPTILE:
      case SKILL_CONS_PEOPLE:
      case SKILL_CONS_GIANT:
      case SKILL_CONS_OTHER:
      case SKILL_READ_MAGIC:
      case SKILL_BANDAGE:
      case SKILL_CLIMB:
      case SKILL_FAST_HEAL:
      case SKILL_EVALUATE:
      case SKILL_TACTICS:
      case SKILL_DISSECT:
      case SKILL_DEFENSE:
      case SKILL_ADVANCED_DEFENSE:
      case SKILL_OFFENSE:
      case SKILL_WHITTLE:
      case SKILL_WIZARDRY:
      case SKILL_RITUALISM:
      case SKILL_MEDITATE:
      case SKILL_DEVOTION:
      case SKILL_PENANCE:
      case SKILL_SLASH_PROF:
      case SKILL_PIERCE_PROF:
      case SKILL_BLUNT_PROF:
      case SKILL_BAREHAND_PROF:
      case SKILL_SLASH_SPEC:
      case SKILL_BLUNT_SPEC:
      case SKILL_PIERCE_SPEC:
      case SKILL_BAREHAND_SPEC:
      case SKILL_RANGED_SPEC:
      case SKILL_RANGED_PROF:
      case SKILL_FAST_LOAD:
      case SKILL_SHARPEN:
      case SKILL_DULL:
      case SKILL_ATTUNE:
      case SKILL_STAVECHARGE:
      case SKILL_SACRIFICE:
      case SPELL_SHIELD_OF_MISTS:
      case SPELL_SHADOW_WALK:
      case SPELL_RAZE:
      case SPELL_HYPNOSIS:
      case SKILL_PSITELEPATHY:
      case SKILL_TELE_SIGHT:
      case SKILL_TELE_VISION:
      case SKILL_MIND_FOCUS:
      case SKILL_PSI_BLAST:
      case SKILL_MIND_THRUST:
      case SKILL_PSYCHIC_CRUSH:
      case SKILL_KINETIC_WAVE:
      case SKILL_MIND_PRESERVATION:
      case SKILL_TELEKINESIS:
      case SKILL_PSIDRAIN:
      case SKILL_MANA:
      case SKILL_IRON_FIST:
      case SKILL_IRON_FLESH:
      case SKILL_IRON_SKIN:
      case SKILL_IRON_BONES:
      case SKILL_IRON_MUSCLES:
      case SKILL_IRON_LEGS:
      case SKILL_IRON_WILL:
      case SKILL_PLANT:
      case SPELL_EMBALM:
#if 1
      case SPELL_EARTHMAW:
      case SPELL_CREEPING_DOOM:
      case SPELL_FERAL_WRATH:
      case SPELL_SKY_SPIRIT:
#endif
        if (!discArray[aff->type]) {
          vlogf(LOG_BUG, fmt("BOGUS AFFECT (%d) on %s") %
            aff->type % k->getName());
          k->affectRemove(aff);
          break;
        }

        str += fmt("Spell : '%s'\n\r") % discArray[aff->type]->name;
        if (aff->location == APPLY_IMMUNITY) {
          str += fmt("     Modifies %s to %s by %ld points\n\r") %
            apply_types[aff->location].name %
            immunity_names[aff->modifier] %
            aff->modifier2;
        } else if (aff->location == APPLY_SPELL) {
          str += fmt("     Modifies %s (%s) by %ld points\n\r") %
            apply_types[aff->location].name %
            (discArray[aff->modifier] ? discArray[aff->modifier]->name : "BOGUS") %
            aff->modifier2;
        } else if (aff->location == APPLY_DISCIPLINE) {
          str += fmt("     Modifies %s (%s) by %ld points\n\r" ) %
            apply_types[aff->location].name %
            (discNames[aff->modifier].disc_num ? discNames[aff->modifier].properName : "BOGUS") %
            aff->modifier2;
        } else {
          str += fmt("     Modifies %s by %ld points\n\r") %
            apply_types[aff->location].name % aff->modifier;
        }
        str += fmt("     Expires in %6d updates, Bits set: %s\n\r\n\r") %
          aff->duration % sprintbit(aff->bitvector, affected_bits);
        break;


      case AFFECT_DISEASE:
        str += fmt("Disease: '%s'\n\r") % DiseaseInfo[affToDisease(*aff)].name;
        str += fmt("     Expires in %d updates.  Status = %d.\n\r") %
          aff->duration % aff->level;
        break;

      case AFFECT_DUMMY:
        str += "Dummy Affect: \n\r";
        str += fmt("     Expires in %d updates.  Status = %d.\n\r") %
          aff->duration % aff->level;
        break;

      case AFFECT_WAS_INDOORS:
        str += "Was indoors (immune to frostbite): \n\r";
        str += fmt("     Expires in %d updates.  Status = %d.\n\r") %
          aff->duration % aff->level;
        break;

      case AFFECT_FREE_DEATHS:
        str += "Free Deaths: \n\r";
        str += fmt("     Remaining %ld.  Status = %d.\n\r") %
          aff->modifier % aff->level;
        break;

      case AFFECT_HORSEOWNED:
        str += "Horse-owned: \n\r";
        str += fmt("     Expires in %d updates.\n\r") %
          aff->duration;
        break;

      case AFFECT_PLAYERKILL:
        str += "Player-Killer: \n\r";
        str += fmt("     Expires in %d updates.\n\r") %
          aff->duration;
        break;

      case AFFECT_PLAYERLOOT:
        str += "Player-Looter: \n\r";
        str += fmt("     Expires in %d updates.\n\r") %
          aff->duration;
        break;

      case AFFECT_TEST_FIGHT_MOB:
        str += "Test Fight Mob: \n\r";
        str += fmt("     Remaining %ld.  Status = %d.\n\r") %
          aff->modifier % aff->level;
        break;

      case AFFECT_SKILL_ATTEMPT:
        str += "Skill Attempt: \n\r";
        str += fmt("     Expires in %d updates.  Skill = %d.\n\r") %
          aff->duration % (int) aff->bitvector; 
        break;

      case AFFECT_NEWBIE:
        str += "Got Newbie Equipment: \n\r";
        str += fmt("     Expires in %d updates.  Status = %d.\n\r") %
          aff->duration % aff->level;
        break;

      case AFFECT_DRUNK:
        str += "Drunken slumber: \n\r";
        str += fmt("     Expires in %d updates.  Status = %d.\n\r") %
          aff->duration % aff->level;
        break;

      case AFFECT_DRUG:
        str += fmt("%s: \n\r") % drugTypes[aff->modifier2].name;
        str += fmt("     Modifies %s by %ld points\n\r") %
          apply_types[aff->location].name % aff->modifier;
        str += fmt("     Expires in %d updates.  Status = %d.\n\r") %
          aff->duration % aff->level;
        str += fmt("renew %i\n\r") % aff->renew;
        break;

      case AFFECT_COMBAT:
        str += fmt("Combat: '%s'\n\r") % 
          (aff->be ? aff->be->getName() : "No aff->be!");
        str += fmt("     Expires in %d updates.  Status = %d.\n\r") %
          aff->duration % aff->level;
        break;

      case AFFECT_PET:
        str += fmt("pet of: '%s'\n\r") % aff->be->getName();
        str += fmt("     Expires in %d updates.  Status = %d.\n\r") %
          aff->duration % aff->level;
        break;

      case AFFECT_CHARM:
        str += fmt("charm of: '%s'\n\r") % aff->be->getName();
        str += fmt("     Expires in %d updates.  Status = %d.\n\r") %
          aff->duration % aff->level;
        break;

      case AFFECT_THRALL:
        str += fmt("thrall of: '%s'\n\r") % aff->be->getName();
        str += fmt("     Expires in %d updates.  Status = %d.\n\r") %
          aff->duration % aff->level;
        break;

      case AFFECT_ORPHAN_PET:
        str += fmt("orphan pet of: '%s'\n\r") % aff->be->getName();
        str += fmt("     Expires in %d updates.  Status = %d.\n\r") %
          aff->duration % aff->level;
        break;

      case AFFECT_TRANSFORMED_ARMS:
      case AFFECT_TRANSFORMED_HANDS:
      case AFFECT_TRANSFORMED_LEGS:
      case AFFECT_TRANSFORMED_HEAD:
      case AFFECT_TRANSFORMED_NECK:
        str += "Spell : 'Transformed Limb'\n\r";
        str += fmt("     Modifies %s by %ld points\n\r") %
          apply_types[aff->location].name % aff->modifier;
        str += fmt("     Expires in %6d updates, Bits set: %s\n\r") %
          aff->duration % sprintbit(aff->bitvector, affected_bits);
        break;

      case AFFECT_GROWTH_POTION:
        str += "Spell : 'Growth'\n\r";
        str += fmt("     Modifies %s by %ld points\n\r") %
          apply_types[aff->location].name % aff->modifier;
        str += fmt("     Expires in %6d updates, Bits set: %s\n\r") %
          aff->duration % sprintbit(aff->bitvector, affected_bits);
        break;
	
      case AFFECT_WARY:
	str += "State: Wary\n\r";
	str += "     Decreases chance of multiple cudgels\n\r";
	break;

      case AFFECT_DEFECTED:
	str += "Player recently defected from a faction.\n\r";
	str += fmt("     Expires in %6d updates.\n\r") % aff->duration;
	break;
	
      case AFFECT_OFFER:
	f = get_faction_by_ID(aff->modifier);
	if (!f) {
	  vlogf(LOG_FACT, "char had faction offer from non-existant faction in cmd_stat");
	  break;
	}
	str += fmt("Received offer to join %s (%d).\n\r") %
          f->getName() % f->ID;
	str += fmt("     Expires in %6d updates.\n\r") % aff->duration;
	break;
	
      case AFFECT_OBJECT_USED:
        objused = aff->modifier;

	str += fmt("Used magical object: %s\n\r") %
          obj_index[objused].short_desc;
        str += fmt("     Expires in %6d updates.\n\r") % aff->duration;
        break;

      case AFFECT_BITTEN_BY_VAMPIRE:
	str += "Bitten by vampire.\n\r";
	str += fmt("Expires in %6d updates.\n\r") % aff->duration;
	break;

      case LAST_ODDBALL_AFFECT:
      case LAST_TRANSFORMED_LIMB:
      case LAST_BREATH_WEAPON:
      case DAMAGE_GUST:
      case DAMAGE_TRAP_TNT:
      case DAMAGE_ELECTRIC:
      case DAMAGE_TRAP_FROST:
      case DAMAGE_FROST:
      case DAMAGE_DROWN:
      case DAMAGE_WHIRLPOOL:
      case DAMAGE_HEMORRAGE:
      case DAMAGE_IMPALE:
      case DAMAGE_TRAP_POISON:
      case DAMAGE_ACID:
      case DAMAGE_TRAP_ACID:
      case DAMAGE_COLLISION:
      case DAMAGE_FALL:
      case DAMAGE_TRAP_BLUNT:
      case DAMAGE_TRAP_FIRE:
      case DAMAGE_FIRE:
      case DAMAGE_DISRUPTION:
      case DAMAGE_DRAIN:
      case DAMAGE_TRAP_ENERGY:
      case DAMAGE_KICK_HEAD:
      case DAMAGE_KICK_SHIN:
      case DAMAGE_KICK_SIDE:
      case DAMAGE_KICK_SOLAR:
      case DAMAGE_TRAP_DISEASE:
      case DAMAGE_SUFFOCATION:
      case DAMAGE_TRAP_SLASH:
      case DAMAGE_ARROWS:
      case DAMAGE_TRAP_PIERCE:
      case DAMAGE_DISEMBOWLED_HR:
      case DAMAGE_DISEMBOWLED_VR:
      case DAMAGE_EATTEN:
      case DAMAGE_HACKED:
      case DAMAGE_KNEESTRIKE_FOOT:
      case DAMAGE_HEADBUTT_FOOT:
      case DAMAGE_KNEESTRIKE_SHIN:
      case DAMAGE_KNEESTRIKE_KNEE:
      case DAMAGE_KNEESTRIKE_THIGH:
      case DAMAGE_HEADBUTT_LEG:
      case DAMAGE_KNEESTRIKE_SOLAR:
      case DAMAGE_HEADBUTT_BODY:
      case DAMAGE_KNEESTRIKE_CROTCH:      
      case DAMAGE_HEADBUTT_CROTCH:
      case DAMAGE_HEADBUTT_THROAT:
      case DAMAGE_KNEESTRIKE_CHIN:
      case DAMAGE_HEADBUTT_JAW:
      case DAMAGE_KNEESTRIKE_FACE:
      case DAMAGE_CAVED_SKULL:
      case DAMAGE_RIPPED_OUT_HEART:
      case DAMAGE_HEADBUTT_SKULL:
      case DAMAGE_STARVATION:
      case DAMAGE_STOMACH_WOUND:
      case DAMAGE_RAMMED:
      case DAMAGE_BEHEADED:
      case DAMAGE_NORMAL:
      case DAMAGE_TRAP_SLEEP:
      case DAMAGE_TRAP_TELEPORT:
      case MAX_SKILL:
      case TYPE_WATER:
      case TYPE_AIR:
      case TYPE_EARTH:
      case TYPE_FIRE:
      case TYPE_KICK:
      case TYPE_CLAW:
      case TYPE_SLASH:
      case TYPE_CLEAVE:
      case TYPE_SLICE:
      case TYPE_BEAR_CLAW:
      case TYPE_MAUL:
      case TYPE_SMASH:
      case TYPE_WHIP:
      case TYPE_CRUSH:
      case TYPE_BLUDGEON:
      case TYPE_SMITE:
      case TYPE_HIT:
      case TYPE_FLAIL:
      case TYPE_PUMMEL:
      case TYPE_THRASH:
      case TYPE_THUMP:
      case TYPE_WALLOP:
      case TYPE_BATTER:
      case TYPE_BEAT:
      case TYPE_STRIKE:
      case TYPE_POUND:
      case TYPE_CLUB:
      case TYPE_PIERCE:
      case TYPE_STAB:
      case TYPE_STING:
      case TYPE_THRUST:
      case TYPE_SPEAR:
      case TYPE_BEAK:
      case TYPE_BITE:
      case TYPE_SHOOT:
      case TYPE_UNDEFINED:
      case TYPE_MAX_HIT:
      case ABSOLUTE_MAX_SKILL:
        vlogf(LOG_BUG, fmt("BOGUS AFFECT (%d) on %s") % aff->type % k->getName());
        k->affectRemove(aff);
        break;
    }
  }
  if (k->task) {
    str += fmt("Player is busy '%s'.\n\r") % tasks[k->task->task].name;
    str += fmt("Time left:    %6d updates     Orignal argument:  %s\n\r") %
      k->task->timeLeft % k->task->orig_arg;
    str += fmt("Was in room:  %6d             Status/Flags:      %6d/%6d\n\r") %
      k->task->wasInRoom % k->task->status % k->task->flags;
  }
  for (i = 1; i < MAX_TOG_INDEX;i++) {
    if (k->hasQuestBit(i))  {
      str += fmt("%sToggle Set:%s (%d) %s\n\r") %
        cyan() % norm() %
        i % TogIndex[i].name;
    }
  }
#if 0
  // spams too much, use "powers xxx" instead
  wizPowerT ipow;
  for (ipow = MIN_POWER_INDEX; ipow < MAX_POWER_INDEX;ipow++) {
    if (k->hasWizPower(ipow))  {
      str += fmt("Wiz-Power Set: (%d) %s\n\r") % mapWizPowerToFile(ipow) % getWizPowerName(ipow);
    }
  }
#endif
  if (k->desc) {
    str += fmt("%sClient:%s %s\n\r") %
      cyan() % norm() %
      (k->desc->m_bIsClient ? "Yes" : "No");
  }
  
  if (km) {
    if (km->sounds) {
      str += fmt("%sLocal Sound:%s\n\r%s") %
        cyan() % norm() %
        km->sounds;
    }
    if (km->distantSnds) {
      str += fmt("%sDistant Sound:%s\n\r%s") %
        cyan() % norm() %
        km->distantSnds;
    }
  }
  desc->page_string(str);
  return;
}

void TBeing::doStat(const sstring &)
{
  return;
}

void TPerson::doStat(const sstring &argument)
{
  sstring arg1, buf, buf2, skbuf, namebuf;
  sstring tmp_arg;
  TBeing *k = NULL;
  TObj *j = NULL;
  int count, parm = 0;
  int foundNum = FALSE;

  if (!hasWizPower(POWER_STAT)) {
    sendTo("Sorry, you lack the power to stat things.\n\r");
    return;
  }

  if (!isImmortal())
    return;

  if (!desc)
    return;

  tmp_arg = argument;
  tmp_arg = one_argument(tmp_arg, buf);
  tmp_arg = one_argument(tmp_arg, skbuf);
  tmp_arg = one_argument(tmp_arg, namebuf);
  arg1 = argument;

  if (arg1.empty()) {
    sendTo("Stats on who or what?\n\r");
    return;
  } else if (is_abbrev(skbuf, "skill")) {
    if (!hasWizPower(POWER_STAT_SKILL)) {
      sendTo("You can not type stat <argument> <skill>.\n\r");
      return;
    }
    if (namebuf.empty()) {
      sendTo("Syntax: stat <char name> <skill> <value>\n\r");
      return;
    }
    count = 1;
    if (!(k = get_char_room(buf, in_room))) {
      if (!(k = get_pc_world(this, buf, EXACT_NO))) {
        if (!(k = get_char_vis_world(this, buf, &count, EXACT_YES))) {
          if (!(k = get_char_vis_world(this, buf, &count, EXACT_NO))) {
            sendTo("Syntax: stat <char name> <skill> <value>\n\r");
            return;
          }
        }
      }
    }

    spellNumT snt;
    if ((parm = convertTo<int>(namebuf))) {
      snt = spellNumT(parm);
    } else {
      foundNum = FALSE;
      for (snt = MIN_SPELL; snt < MAX_SKILL; snt++) {
        if (hideThisSpell(snt)) {
          continue;
        }
        if (is_exact_name(namebuf, discArray[snt]->name)) {
          if (!(k->getSkill(snt))) {
            continue;
          }
          foundNum = TRUE;
          break;
        }
      }
      if (!foundNum) {
        for (snt = MIN_SPELL; snt < MAX_SKILL; snt++) {
          if (hideThisSpell(snt)) {
            continue;
          }
          buf2 = discArray[snt]->name;
          // kludge since chivalry < chi  in discarray
          if ((namebuf == "chi") && (buf2 != "chi")) {
            continue;
          }
          // kludge since stealth < steal in discarray
          if ((namebuf == "steal") && (buf2 != "steal")) {
            continue;
          }
          // kludge since paralyze limb < paralyze in discarray
          if ((namebuf == "paralyze") && (buf2 != "paralyze")) {
            continue;
          }
          if (isname(namebuf, discArray[snt]->name)) {
            if (!(k->getSkill(snt))) {
              continue;
            }
            break;
          }
        } 
      }
    }
    if ((snt < MIN_SPELL) || (snt >= MAX_SKILL)) {
      sendTo(fmt("Not a good skill number (%d) or the being doesnt have the skill!\n\r") % snt);
      sendTo("Syntax: stat <char name> <skill> <value>\n\r");
      return;
    }

    if (!k->doesKnowSkill(snt)) {
      if (discArray[snt]) {
        sendTo(COLOR_MOBS, fmt("%s doesnt appear to know that skill (%s).\n\r") % k->getName() % (discArray[snt]->name ? discArray[snt]->name : "unknown"));
      } else {
        sendTo(COLOR_MOBS, fmt("%s doesnt appear to know that skill.\n\r") % k->getName());
      }
      return;
    }
    CSkill *sk = k->getSkill(snt);
    if (!sk) {
      if (discArray[snt]) {
        sendTo(COLOR_MOBS, fmt("%s doesnt appear to have that skill (%s).\n\r") % k->getName() % (discArray[snt]->name ? discArray[snt]->name : "unknown"));
      } else {
        sendTo(COLOR_MOBS, fmt("%s doesnt appear to know that skill.\n\r") % k->getName());
      }
      return;
    }
    sendTo(COLOR_MOBS, fmt("%s's %s Raw (stored) Learning: Current (%d) Natural (%d).\n\r") % k->getName() % discArray[snt]->name % k->getRawSkillValue(snt) % k->getRawNatSkillValue(snt));
    sendTo(COLOR_MOBS, fmt("%s's %s Actual (used) Learning: Current (%d) Natural (%d) Max (%d).\n\r") % k->getName() % discArray[snt]->name % k->getSkillValue(snt) % k->getNatSkillValue(snt) % k->getMaxSkillValue(snt));

    time_t ct = sk->lastUsed;
    char *tmstr = (char *) asctime(localtime(&ct));
    *(tmstr + strlen(tmstr) - 1) = '\0';
    sendTo(COLOR_MOBS, fmt("%s's %s Last Increased: %s\n\r") % k->getName() % discArray[snt]->name % tmstr);

    return;
  } else if (is_abbrev(skbuf, "discipline")) {
    if (!hasWizPower(POWER_STAT_SKILL)) {
      sendTo("You can not type stat <argument> <discipline>.\n\r");
      return;
    }

    count = 1;

    if (!(k = get_char_room(buf, in_room))) {
      if (!(k = get_pc_world(this, buf, EXACT_NO))) {
        if (!(k = get_char_vis_world(this, buf, &count, EXACT_NO))) {
          sendTo("Syntax: stat <char name> <discipline> <value>\n\r");
          return;
        }
      }
    }

    CDiscipline *cd;
    
    if (namebuf.empty() && !k->isPc() && !k->desc) {
      sendTo(COLOR_MOBS, fmt("%s has the following disciplines:\n\r\n\r") % k->getName());
      discNumT dnt;
      for (dnt = MIN_DISC; dnt < MAX_DISCS; dnt++) {
        if (!(cd = k->getDiscipline(dnt))) {
          break;
        }
        sendTo(COLOR_MOBS, fmt("Discpline %20.20s : Current (%d) Natural (%d).\n\r") % discNames[dnt].name % cd->getLearnedness()  % cd->getNatLearnedness());
      }
      return;
    } else if (namebuf.empty()) {
      sendTo("Syntax: stat <char name> <discipline> <value>\n\r");
      return;
    }

    discNumT dnt = mapFileToDisc(convertTo<int>(namebuf));
    if (dnt == DISC_NONE) {
      sendTo("Not a good discipline!\n\r");
      return;
    }

    if (!k->discs) {
      sendTo(COLOR_MOBS, fmt("%s does not have disciplines allocated yet!\n\r") % k->getName());
      return;
    }

    if (!(cd = k->getDiscipline(dnt))) {
       sendTo(COLOR_MOBS, fmt("%s doesnt appear to have that disipline.\n\r") % k->getName());
       return;
    }
    sendTo(COLOR_MOBS, fmt("%s's %s Used Learning: Current (%d) Natural (%d).\n\r") % k->getName() % discNames[dnt].name % cd->getLearnedness() % cd->getNatLearnedness());
    return;
  } else if (is_abbrev(skbuf, "donebasic")) {
    if (!hasWizPower(POWER_STAT_SKILL)) {
      sendTo("You can not type stat <argument> <donebasic>.\n\r");
      return;
    }
    if (namebuf.empty()) {
      sendTo("Syntax: stat <char name> <donebasic>\n\r");
      return;
    }
    count = 1;
    if (!(k = get_char_room(buf, in_room))) {
      if (!(k = get_pc_world(this, buf, EXACT_NO))) {
        if (!(k = get_char_vis_world(this, buf, &count, EXACT_NO))) {
          sendTo("Syntax: stat <char name> <donebasic>\n\r");
          return;
        }
      }
    }
    for (count = 0; count < MAX_CLASSES; count++) {
      sendTo(fmt("%-25.25s  :  %d\n\r") % classInfo[count].name.cap() % k->player.doneBasic[count]);
    }
    return;
  } else if (arg1 == "room") {
    statRoom(roomp);
    return;
  } else if (buf == "zone") {
    statZone(skbuf);
    return;
  } else {
    count = 1;

    if (((j = get_obj_vis_accessible(this, arg1)) ||
          (j = get_obj_vis(this, arg1.c_str(), &count, EXACT_NO))) &&
          ((k = get_char_room(arg1, in_room)) == NULL) &&
          ((k = get_pc_world(this, arg1, EXACT_NO)) == NULL)) {
      if (!hasWizPower(POWER_STAT_OBJECT)) {
        sendTo("Sorry, you lack the power to stat objects.\n\r");
        return;
      }
      statObj(j);
      return;
    }

    if ((k = get_char_room(arg1, in_room)) || 
        (k = get_char_vis_world(this, arg1, &count, EXACT_NO))) {
      if (!hasWizPower(POWER_STAT_MOBILES)) {
        sendTo("Sorry, you lack the power to stat mobiles.\n\r");
        return;
      }
      statBeing(k);
      return;
    }

    if (is_number(arg1)) {
      TObj         *tObj=NULL;
      TMonster     *tMonster=NULL;
      unsigned int  tValue;

      if (hasWizPower(POWER_STAT_OBJECT) &&
          ((tValue = real_object(convertTo<int>(arg1))) < obj_index.size()) &&
          tValue >= 0 && (tObj = read_object(tValue, REAL))) {
        statObj(tObj);
        delete tObj;
        tObj = NULL;

        return;
      }

      if (hasWizPower(POWER_STAT_MOBILES) &&
          ((tValue = real_mobile(convertTo<int>(arg1))) < mob_index.size()) &&
          tValue >= 0 && (tMonster = read_mobile(tValue, REAL))) {
        statBeing(tMonster);
        delete tMonster;
        tMonster = NULL;

        return;
      }
    }

    sendTo("No mobile or object by that name in The World.\n\r");
  }
}
