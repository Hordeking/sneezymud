//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//    "show.cc" - Functions related to showing something
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <dirent.h>

#include "stdsneezy.h"
#include "disc_looting.h"
#include "combat.h"
#include "obj_component.h"
#include "cmd_dissect.h"
#include "disc_alchemy.h"
#include "obj_table.h"
#include "obj_note.h"
#include "obj_trap.h"
#include "obj_window.h"
#include "obj_potion.h"
#include "obj_scroll.h"
#include "obj_staff.h"
#include "database.h"

void TThing::showMe(TBeing *ch) const
{
  ch->sendTo("You see nothing special.\n\r");
}

void TNote::showMe(TBeing *ch) const
{
  sstring sb;
  const char HEADER_TXT_NOTE[] = "There is something written upon it:\n\r\n\r";

  if (action_description) {
    sb += HEADER_TXT_NOTE;
    sb += action_description;

    if (ch->desc)
      if (!ch->desc->m_bIsClient) {
        ch->desc->page_string(sb);
      } else {
        processStringForClient(sb);
        ch->desc->clientf(fmt("%d") % CLIENT_NOTE);
        ch->sendTo(fmt("%s") % sb);  // tmpbuf may have "%" in it, do it this way
        ch->desc->clientf(fmt("%d") % CLIENT_NOTE_END);
      }
  } else
    ch->sendTo("It's blank.\n\r");
  return;
}

//   Obviously, all this is really hokey for a procedure that gets used so
//   often.  This probably needs to get redone eventually.  I am happy to
//   say that none of this is my mess.  - SG 
void TObj::show_me_to_char(TBeing *ch, showModeT mode) const
{
  sstring buffer, buf, capbuf;

  if (mode == SHOW_MODE_DESC_PLUS && getDescr()) {
    if (roomp && roomp->isWaterSector() && 
        !isObjStat(ITEM_HOVER) &&
        !isObjStat(ITEM_FLOAT) &&
        !objVnum() == GENERIC_FLARE ) {
      buffer = fmt("%s is floating here.") % getName();
      buffer=buffer.cap();
    } else {
      capbuf = fmt("%s") % addNameToBuf(ch, ch->desc, this, getDescr(), COLOR_OBJECTS);
      sstring cStrbuf = capbuf;
      while (cStrbuf.find("$$g") != sstring::npos)
        cStrbuf.replace(cStrbuf.find("$$g"), 3,
                        (roomp ? roomp->describeGround().c_str() : "TELL A GOD"));
      while (cStrbuf.find("$g") != sstring::npos)
        cStrbuf.replace(cStrbuf.find("$g"), 2,
                        (roomp ? roomp->describeGround().c_str() : "TELL A GOD"));
      capbuf=cStrbuf.cap();
      buffer=colorString(ch, ch->desc, capbuf, NULL, COLOR_OBJECTS, TRUE);
    }
  } else if ((mode == SHOW_MODE_SHORT_PLUS || 
              mode == SHOW_MODE_SHORT_PLUS_INV ||
              mode == SHOW_MODE_SHORT) && getName()) {
    buffer=sstring(getName()).cap();
  } else if (mode == SHOW_MODE_TYPE) {
    showMe(ch);
    return;
  } else 
    buffer[0] = '\0';// we need this before we start doing strcat
  
  // this is an item-type-specific modifier
  buffer += showModifier(mode, ch);

  if (isObjStat(ITEM_INVISIBLE))
    buffer += " (invisible)";
  if (isObjStat(ITEM_MAGIC) && ch->isAffected(AFF_DETECT_MAGIC)){
    buf = fmt(" %s(blue aura)%s") % ch->cyan() % ch->norm();
    buffer += buf;
  }
  if (isObjStat(ITEM_GLOW)){
    buf = fmt(" %s(glowing)%s") % ch->orange() % ch->norm();
    buffer += buf;
  }
  if (isObjStat(ITEM_SHADOWY))
    buffer += " (shadowy)";
  if (isObjStat(ITEM_HOVER))
    buffer += " (hovering)";
  if (isObjStat(ITEM_HUM))
    buffer += " (humming)";
  if (isObjStat(ITEM_BURNING)){
    buf = fmt(" %s(burning)%s") % ch->red() % ch->norm();
    buffer += buf;
  }
  if (isObjStat(ITEM_CHARRED)){
    buf = fmt(" %s(charred)%s") % ch->blackBold() % ch->norm();
    buffer += buf;
  }


  if (parent && dynamic_cast<TObj *>(parent)) {
    buf = fmt(" (in %s)") % sstring(parent->getName()).uncap();
    buffer += buf;
  }
  if (riding) {
    buf = fmt(" (on %s)") % sstring(ch->objs(riding)).uncap();
    buffer += buf;
  }
  if (dynamic_cast<const TTable *>(this) && rider) {
    buffer += "\n\r";
    ch->sendTo(buffer);
    list_thing_on_heap(rider, ch, false);
    return;
  }
  if (!buffer.empty())
    buffer += "\n\r";
  buf = fmt("%s") % colorString(ch, ch->desc, buffer, NULL, COLOR_OBJECTS, TRUE);
//  ch->sendTo(COLOR_OBJECTS, buffer);
//  COSMO_COLOR
  ch->sendTo(buf);
}

void TObj::show_me_mult_to_char(TBeing *ch, showModeT mode, unsigned int num) const
{
  sstring buffer, tmp, capbuf;

  // uses page string (needs desc), so don't bother unless PC
  if (!ch->desc)
    return;

  if (mode == SHOW_MODE_DESC_PLUS && getDescr()) {
    capbuf = fmt("%s") % addNameToBuf(ch, ch->desc, this, getDescr(), COLOR_OBJECTS);
    sstring cStrbuf = capbuf;
    while (cStrbuf.find("$$g") != sstring::npos)
      cStrbuf.replace(cStrbuf.find("$$g"), 3, 
                      (roomp ? roomp->describeGround().c_str() : "TELL A GOD"));
    while (cStrbuf.find("$g") != sstring::npos)
      cStrbuf.replace(cStrbuf.find("$g"), 2,
                      (roomp ? roomp->describeGround().c_str() : "TELL A GOD"));
    capbuf=cStrbuf;
    buffer=capbuf;
  } else if (getName() && (mode == SHOW_MODE_SHORT_PLUS ||
          mode == SHOW_MODE_SHORT_PLUS_INV || mode == SHOW_MODE_SHORT)) {
    buffer=getName();
  } else if (mode == SHOW_MODE_TYPE) {
    showMe(ch);
  }
  if (mode == SHOW_MODE_DESC_PLUS ||
      mode == SHOW_MODE_SHORT_PLUS ||
      mode == SHOW_MODE_SHORT_PLUS_INV ||
      mode == SHOW_MODE_TYPE ||
      mode == SHOW_MODE_PLUS) {
    buffer += showModifier(mode, ch);

    if (isObjStat(ITEM_INVISIBLE)) 
      buffer += " (invisible)";
    if (isObjStat(ITEM_MAGIC) && ch->isAffected(AFF_DETECT_MAGIC)) {
      tmp = fmt(" %s(glowing blue)%s") % ch->cyan() % ch->norm();
      buffer += tmp;
    }
    
    if (isObjStat(ITEM_GLOW)) {
      tmp = fmt(" %s(glowing)%s") % ch->orange() % ch->norm();
      buffer += tmp;
    }
    
    if (isObjStat(ITEM_SHADOWY))
      buffer += " (shadowy)";
    if (isObjStat(ITEM_HOVER))
      buffer += " (hovering)";

    if (isObjStat(ITEM_HUM)) 
      buffer += " (humming)";
    if (isObjStat(ITEM_BURNING)){
      tmp = fmt(" %s(burning)%s") % ch->red() % ch->norm();
      buffer += tmp;
    }
    if (isObjStat(ITEM_CHARRED)){
      tmp = fmt(" %s(charred)%s") % ch->blackBold() % ch->norm();
      buffer += tmp;
    }
    
  }
  if (num > 1) {
    tmp = fmt(" [%d]") % num;
    buffer += tmp;
  }
  if (parent && dynamic_cast<TObj *>(parent)) {    
    tmp = fmt(" (in %s)") % sstring(parent->getName()).uncap();
    buffer += tmp;
  }
  if (riding) {
    tmp = fmt(" (on %s)") % sstring(ch->objs(riding)).uncap();
    buffer += tmp;
  }
  buffer += "\n\r";
  buffer=buffer.cap();
  buffer = fmt("%s") % colorString(ch, ch->desc, buffer, NULL, COLOR_OBJECTS, TRUE);
  ch->desc->page_string(buffer);
}

void TTrap::listMeExcessive(TBeing *ch) const
{
  if ((getTrapCharges() > 0)) {
    if (ch->doesKnowSkill(SKILL_DETECT_TRAP)) {
      if (detectTrapObj(ch, this))
        ch->showTo(this, SHOW_MODE_DESC_PLUS);
    }
  } else {
    // used up trap
    ch->showTo(this, SHOW_MODE_DESC_PLUS);
  }
}

void TThing::listMeExcessive(TBeing *ch) const
{
  ch->showTo(this, SHOW_MODE_DESC_PLUS);
}

void TTrap::listMe(TBeing *ch, unsigned int num) const
{
  if ( getTrapCharges() > 0) {
    if (ch->doesKnowSkill(SKILL_DETECT_TRAP)) {
      if (detectTrapObj(ch, this)) {
        if (num > 1) 
          ch->showMultTo(this, SHOW_MODE_DESC_PLUS, num);
        else 
          ch->showTo(this, SHOW_MODE_DESC_PLUS);
      }
    }
  } else {
    // used up traps
    if (num > 1) 
      ch->showMultTo(this, SHOW_MODE_DESC_PLUS, num);
    else 
      ch->showTo(this, SHOW_MODE_DESC_PLUS);
  }
}

void TThing::listMe(TBeing *ch, unsigned int num) const
{
  if (num > 1) 
    ch->showMultTo(this, SHOW_MODE_DESC_PLUS, num);
  else 
    ch->showTo(this, SHOW_MODE_DESC_PLUS);
}

// returns true if should not be listed
bool TBeing::listThingRoomMe(const TBeing *ch) const
{
  if (ch == this) {
    return true;
  } else if (rider) {
    // a horse
    // in general, don't display
    // if I can detect the horse, but not the rider, this would be bad
    if (!ch->isAffected(AFF_SENSE_LIFE) &&
        !ch->canSee(this, INFRA_YES) &&
        !ch->canSee(this, INFRA_NO)) {
      // horse is concealed
      return true;
    } else {
      TThing *t2 = NULL;
      for (t2 = rider; t2; t2 = t2->nextRider) {
        // if one of the riders is vis, skip showing the horse
        if (ch == dynamic_cast<TBeing *>(t2) ||
            ch->isAffected(AFF_SENSE_LIFE) ||
            ch->canSee(t2, INFRA_YES) ||
            ch->canSee(t2)) {
          return true;
        }
      } 
    }
  } else if (!ch->isAffected(AFF_SENSE_LIFE) &&
      !ch->canSee(this, INFRA_YES) &&
      !ch->canSee(this)) {
    return true;
  }
  return false;
}

// returns true if should not be listed
bool TObj::listThingRoomMe(const TBeing *ch) const
{
  // list tables, but not chairs
  if (!ch->canSee(this) || dynamic_cast<TBeing *>(rider))
    return true;
  return false;
}

// returns true if should not be listed
bool TWindow::listThingRoomMe(const TBeing *ch) const
{
  return true;
}

// returns true if should not be listed
bool TThing::listThingRoomMe(const TBeing *ch) const
{
  if (!ch->canSee(this))
    return true;
  return false;
}

void list_thing_in_room(const TThing *list, TBeing *ch)
{
  const TThing *t;
  unsigned int k;
  vector <const TThing *> cond_ptr;
  vector <int> cond_tot;

  for (t = list; t; t = t->nextThing) {
    if (t->listThingRoomMe(ch))
      continue;
    if(t->riding){
      t->listMeExcessive(ch);
    } else {
      bool found = FALSE;
      for (k = 0; (k < cond_ptr.size() && !found); k++) {
	if(dynamic_cast<const TObj *>(t) || dynamic_cast<const TMonster *>(t)){
          if (cond_ptr.size() > 0) {
            if (t->isSimilar(cond_ptr[k])) {
              cond_tot[k] += 1;
              found = TRUE;
            }
          }
        }
      }
      if (!found) {
	cond_ptr.push_back(t);
	cond_tot.push_back(1);
      }
    }
  }
  if (cond_ptr.size()>0) {
    for (k = 0; k < cond_ptr.size(); k++) {
      cond_ptr[k]->listMe(ch, cond_tot[k]);
    }
  }
}

void list_in_heap(const TThing *list, TBeing *ch, bool show_all, int perc)
{
  const TThing *i;
  vector<const TThing *>cond_ptr(0);
  vector<unsigned int>cond_tot(0);
  unsigned int k;

  for (i = list; i; i = i->nextThing) {
    if (number(0,99) > perc)
      continue;
    if (ch->canSee(i)) {
      for (k = 0; k < cond_ptr.size(); k++) {
        if (i->isSimilar(cond_ptr[k])) {
          cond_tot[k] += 1;
          break;
        }
      }
      if (k >= cond_ptr.size()) {
        cond_ptr.push_back(i);
        cond_tot.push_back(1);
      }
    }
    if (show_all && i->getStuff())
      list_in_heap(i->getStuff(), ch, true, 100);
  } // for loop

  if (cond_ptr.empty())
    ch->sendTo("Nothing.\n\r");

//  int Num_Inventory = 1;
  for (k = 0; k < cond_ptr.size(); k++) {
    if (cond_tot[k] > 1) {
//      Num_Inventory += cond_tot[k] - 1;
      ch->showMultTo(cond_ptr[k], SHOW_MODE_SHORT_PLUS_INV, cond_tot[k]);
    } else
      ch->showTo(cond_ptr[k], SHOW_MODE_SHORT_PLUS_INV);
  }
}

void list_thing_on_heap(const TThing *list, TBeing *ch, bool show_all)
{
  const TThing *i, *cond_ptr[50];
  int k, cond_top;
  unsigned int cond_tot[50];
  bool found = FALSE;

  int Num_Inventory = 1;
  cond_top = 0;

  for (i = list; i; i = i->nextRider) {
    if (ch->canSee(i)) {
      if (cond_top < 50) {
        found = FALSE;
        for (k = 0; (k < cond_top && !found); k++) {
          if (cond_top > 0) {
            if (i->isSimilar(cond_ptr[k])) {
              cond_tot[k] += 1;
              found = TRUE;
            }
          }
        }
        if (!found) {
          cond_ptr[cond_top] = i;
          cond_tot[cond_top] = 1;
          cond_top += 1;
        }
      } else
        ch->showTo(i, SHOW_MODE_SHORT_PLUS_INV);
    }
    if (show_all)
      list_thing_on_heap(i->rider, ch, true);
  } // for loop
  if (cond_top) {
    for (k = 0; k < cond_top; k++) {
      if (cond_tot[k] > 1) {
        Num_Inventory += cond_tot[k] - 1;
        ch->showMultTo(cond_ptr[k], SHOW_MODE_SHORT_PLUS_INV, cond_tot[k]);
      } else
        ch->showTo(cond_ptr[k], SHOW_MODE_SHORT_PLUS_INV);
    }
  }
}

static sstring displayShowApprox(const TBeing *looked, const TBeing *looker, spellNumT tSkill, float tDiff)
{
  // This function is still experimental.  Don't use it in the main world yet.
  if (strcmp(looker->getName(), "Lapsos") != 0 || !looker->isImmortal())
    return ("");

  if (!looker->doesKnowSkill(tSkill))
    return ("");

  affectedData *tAff;
  char          tBuf[256];

  // -49, ..., 49
  float tDeffer = (float) (looked->GetMaxLevel() - looker->GetMaxLevel());

  // (-1, ..., 1), ..., (-49, ..., 49)
  if (tDeffer > 0)
    tDeffer = (float) ::number((int) tDeffer, (int) -(tDeffer));
  else
    tDeffer = 0;

  // Assuming 2.0 tDiff:
  // 0, (-6, ..., 6), ..., (-102, ..., 102), ..., 100
  tDeffer = max((float) 100.0, min((float) 0.0, (tDiff * (tDeffer + ::number(-2, 2)))));
  // 0, ..., 2
  tDeffer += (float) ((100 - looker->getSkillValue(tSkill)) / 100);
  // -.20, ..., .00, ..., .20
  tDeffer = ((float) ::number((int) tDeffer, (int) -(tDeffer))) / 10;

  for (tAff = looked->affected; tAff; tAff = tAff->next)
    if (tAff->type == tSkill) {
      tDeffer = (float) (tAff->duration + (tDeffer * tAff->duration));

      sprintf(tBuf, "Approx Time Left: %s\n\r",
              describeDuration(looker, (int) tDeffer).c_str());
      return (tBuf);
    }

  return ("");
}

static void describeSpellEffects(const TBeing *me, const TBeing *ch, bool verbose)
{
  char   bufspell[1024];
  char   bufpray[1024];
  int    totspell = 0;
  int    totpray  = 0;
  sstring tStSpell(""),
         tStPray("");

  // these are the less common or more urgent affects that we want noticed
  if (me->getCaptiveOf()) {
    char buf[256];
    sprintf(buf, ".....$n is held captive by %s.",me->getCaptiveOf()->getName());
    act(buf, FALSE, me, 0, ch, TO_VICT);
  }

  if (me->isAffected(AFF_MUNCHING_CORPSE))
    act(".....$n is munching on a corpse!", FALSE, me, 0, ch, TO_VICT);

  if (me->isFlying()) {
    if (me->roomp && me->roomp->isUnderwaterSector())
      act(".....$n is swimming around.", FALSE, me, 0, ch, TO_VICT);
    else
      act(".....$n is flying around.", FALSE, me, 0, ch, TO_VICT);
  }
  if (me->isLevitating())
    act(".....$n is hovering above the $g!", FALSE, me, 0, ch, TO_VICT);
#if 1
  if (me->affectedBySpell(SPELL_FERAL_WRATH))
    act(".....$n has a feral madness about $m.", FALSE, me, 0, ch, TO_VICT);
#endif
  if (me->isAffected(AFF_BLIND) && !me->affectedBySpell(SPELL_TRUE_SIGHT) &&
      me->getPosition() > POSITION_SITTING && me->getPosition() <= POSITION_STANDING)
    act(".....$n blindly stumbles around!", FALSE, me, 0, ch, TO_VICT);

  if (me->isAffected(AFF_WEB)) {
    if (me->affectedBySpell(SPELL_LIVING_VINES))
      act(".....$n is entangled in a mass of vines!", FALSE, me, 0, ch, TO_VICT);
    else
      act(".....$n is enshrouded in magical webs!", FALSE, me, 0, ch, TO_VICT);
  }

  if (me->isAffected(AFF_SANCTUARY)) {
    act(".....$n glows with a bright light!", FALSE, me, 0, ch, TO_VICT);
    //act(displayShowApprox(me, ch, SPELL_SANCTUARY, 2.0).c_str(),
    //    FALSE, me, 0, ch, TO_VICT);
  }

  if(me->affectedBySpell(AFFECT_PLAYERKILL)){
   colorAct(COLOR_ROOMS, 
	    ".....$n is covered in <r>blood<1> from a recent murder!",
	    FALSE, me, 0, ch, TO_VICT);
  }

  // more common effects that we wish to suppress
  bufspell[0] = '\0';
  bufpray[0]  = '\0';

  if (me->affectedBySpell(SPELL_FAERIE_FIRE)) {
    sprintf(bufspell, ".....$n is surrounded by a pink aura!\n\r");
    tStSpell += bufspell;
    tStSpell += displayShowApprox(me, ch, SPELL_FAERIE_FIRE, 1.0);
    ++totspell;
  }

  if (me->affectedBySpell(SPELL_STUPIDITY)) {
    sprintf(bufspell, ".....$n is surrounded by a fog of stupidity!\n\r");
    tStSpell += bufspell;
    tStSpell += displayShowApprox(me, ch, SPELL_STUPIDITY, 1.0);
    ++totspell;
  }

  if (me->affectedBySpell(SPELL_FLAMING_FLESH)) {
    sprintf(bufspell, ".....$n has a ring of fire about $m!\n\r");
    tStSpell += bufspell;
    tStSpell += displayShowApprox(me, ch, SPELL_FLAMING_FLESH, 1.3);
    ++totspell;
  }

  if (me->affectedBySpell(SPELL_STONE_SKIN)) {
    sprintf(bufspell, ".....$n's skin has a gritty, rock-like look!\n\r");
    tStSpell += bufspell;
    tStSpell += displayShowApprox(me, ch, SPELL_STONE_SKIN, 1.3);
    ++totspell;
  }

  if (me->affectedBySpell(SPELL_SORCERERS_GLOBE)) {
    sprintf(bufspell, ".....pulsating swirls of magic encircle $n.\n\r");
    tStSpell += bufspell;
    tStSpell += displayShowApprox(me, ch, SPELL_SORCERERS_GLOBE, .5);
    ++totspell;
  }

  if (me->affectedBySpell(SPELL_ARMOR)) {
    sprintf(bufpray, ".....$n is surrounded by a radiant hue.\n\r");
    tStPray += bufpray;
    tStPray += displayShowApprox(me, ch, SPELL_ARMOR, .6);
    ++totpray;
  }

  if (me->affectedBySpell(SPELL_BLESS)) {
    sprintf(bufpray, ".....$n emits a brilliant aura.\n\r");
    tStPray += bufpray;
    tStPray += displayShowApprox(me, ch, SPELL_BLESS, .2);
    ++totpray;
  }

  if (me->affectedBySpell(SPELL_CURSE)) {
    sprintf(bufpray, ".....$n is surrounded by a dark, forboding red aura!\n\r");
    tStPray += bufpray;
    tStPray += displayShowApprox(me, ch, SPELL_CURSE, 1.0);
    ++totpray;
  }

  if (me->affectedBySpell(SKILL_BARKSKIN)) {
    sprintf(bufspell, ".....$n seems coated in a thin veneer.\n\r");
    tStSpell += bufspell;
    tStSpell += displayShowApprox(me, ch, SKILL_BARKSKIN, 1.2);
    ++totspell;
  }

  if (me->affectedBySpell(SPELL_PLASMA_MIRROR)) {
    sprintf(bufspell, ".....$n has great swirls of plasma surrounding $m.\n\r");
    tStSpell += bufspell;
    tStSpell += displayShowApprox(me, ch, SPELL_PLASMA_MIRROR, 2.0);
    ++totspell;
  }

  if (me->affectedBySpell(SPELL_THORNFLESH)) {
    sprintf(bufspell, ".....$n has thorns all over $s body.\n\r");
    tStSpell += bufspell;
    tStSpell += displayShowApprox(me, ch, SPELL_THORNFLESH, 2.0);
    ++totspell;
  }
  if (me->affectedBySpell(SPELL_SHIELD_OF_MISTS)) {
    sprintf(bufspell, ".....$n is enveloped by a thick green mist.\n\r");
    tStSpell += bufspell;
    tStSpell += displayShowApprox(me, ch, SPELL_THORNFLESH, 2.0);
    ++totspell;
  }

  strcpy(bufspell, tStSpell.c_str());
  strcpy(bufpray, tStPray.c_str());

  if (totspell > 0) {
    if (verbose || totspell == 1) {
      bufspell[strlen(bufspell) - 2] = '\0'; // strip off the trailing \n\r, act() re-adds it
      act(bufspell, FALSE, me, 0, ch, TO_VICT);
    } else {
      bufspell[0] = '\0';
      sprintf(bufspell, ".....$n is surrounded by ");
      if (totspell < 3)
	strcat(bufspell, "a couple of");
      else if (totspell < 6)
	strcat(bufspell, "a few");
      else if (totspell < 9)
	strcat(bufspell, "several");
      else
	strcat(bufspell, "a great many");
      strcat(bufspell, " effects.");
      act(bufspell, FALSE, me, 0, ch, TO_VICT);
    }
  }

  if (totpray > 0) {
    if (verbose || totpray == 1) {
      bufpray[strlen(bufpray) - 2] = '\0'; // strip off the trailing \n\r, act() re-adds it
      act(bufpray, FALSE, me, 0, ch, TO_VICT);
    } else {
      bufpray[0] = '\0';
      sprintf(bufpray, ".....$n is surrounded by ");
      if (totpray < 3)
	strcat(bufpray, "a couple of");
      else if (totpray < 6)
	strcat(bufpray, "a few");
      else if (totpray < 9)
	strcat(bufpray, "several");
      else
	strcat(bufpray, "a great many");

      strcat(bufpray, " spiritual effects.");
      act(bufpray, FALSE, me, 0, ch, TO_VICT);
    }
  }
}

void TBeing::show_me_to_char(TBeing *ch, showModeT mode) const
{
  sstring buffer, buf, capbuf;
  int found, percent;
  TThing *t;

  if (mode == SHOW_MODE_DESC_PLUS) {
    if (!ch->canSee(this)) {
      if (ch->canSee(this, INFRA_YES)) {
        buffer = fmt("A blob of heat is here in the shape of %s %s.\n\r") %
          (getMyRace()->getSingularName().startsVowel() ? "an" : "a") %
          getMyRace()->getSingularName();
        ch->sendTo(buffer);
      } else if (ch->isAffected(AFF_SENSE_LIFE) && (GetMaxLevel() < 51))
        ch->sendTo("You sense a hidden life form in the room.\n\r");

      return;
    }
    const TMonster *tm = dynamic_cast<const TMonster *>(this);
    if (riding || spelltask || fight() ||
        (roomp->isWaterSector() && !isAffected(AFF_WATERBREATH)) ||
        !(getLongDesc()) ||
        (tm && tm->getPosition() != tm->default_pos)) {
      // A player char or a mobile without long descr, or not in default pos. 
      if (hasColorStrings(NULL, getName(), 2)) {
        if (dynamic_cast<const TPerson *>(this))
          buffer = colorString(ch, ch->desc, getName(), NULL, COLOR_MOBS, FALSE);
        else 
          buffer = colorString(ch, ch->desc, sstring(getName()).cap(), NULL, COLOR_MOBS, FALSE);
      } else 
        buffer = fmt("%s%s%s") % ch->cyan() % sstring(getName()).cap() % ch->norm();
      
      if (isAffected(AFF_INVISIBLE) || getInvisLevel() > MAX_MORT)
        buffer +=" (invisible)";
      if (isAffected(AFF_SHADOW_WALK))
        buffer +=" (shadow)";
      if (isPet(PETTYPE_THRALL))
        buffer +=" (thrall)";
      if (isPet(PETTYPE_CHARM))
        buffer +=" (charm)";
      if (isPet(PETTYPE_PET))
        buffer +=" (pet)";
      if ((ch->isImmortal() || affectedBySpell(AFFECT_PLAYERKILL)) && 
	  isLinkdead())
        buffer +=" (link-dead)";
      if (getTimer() >= 10)
        buffer +=" (AFK)";
      if (desc && desc->connected)
        buffer +=" (editing)";
      if (ch->isImmortal() && isDiurnal())
        buffer +=" (diurnal)";
      if (ch->isImmortal() && isNocturnal())
        buffer +=" (nocturnal)";
      if (spelltask) {
        if (((discArray[(spelltask->spell)])->typ) == SPELL_MAGE) 
          buffer +=" is here, casting a spell.";
        else if (((discArray[(spelltask->spell)])->typ) == SPELL_CLERIC) 
          buffer +=" is here, reciting a prayer.";
        else if (((discArray[(spelltask->spell)])->typ) == SPELL_SHAMAN) 
          buffer +=" is here, invoking a ritual.";
      } else if (fight()) {
        buffer +=" is here, fighting ";
        if (fight() == ch)
          buffer +="YOU!";
        else {
          if (sameRoom(*fight())) {
            buffer += ch->pers(fight());
            buffer +=".";
          } else
            buffer +="someone who has already left.";
        }
      } else {
        TBeing *tbr;
        switch (getPosition()) {
          case POSITION_STUNNED:
            buffer +=" is lying here, stunned.";
            break;
          case POSITION_INCAP:
            buffer +=" is lying here, incapacitated.";
            break;
          case POSITION_MORTALLYW:
            buffer +=" is lying here, mortally wounded.";
            break;
          case POSITION_DEAD:
            buffer +=" is lying here, dead.";
            break;
          case POSITION_MOUNTED:
            tbr = dynamic_cast<TBeing *>(riding);
            if (tbr &&
                dynamic_cast<const TBeing *>(riding->horseMaster()) == this) {
              buffer +=" is here, riding ";
              buffer += ch->pers(riding);
              if (tbr->isAffected(AFF_INVISIBLE))
                buffer +=" (invisible)";
              if (tbr->isAffected(AFF_SHADOW_WALK))
                buffer +=" (shadow)";
              buffer +=".";
            } else if (tbr && tbr->horseMaster()) {
              if (ch == tbr->horseMaster())
                buffer +=" is here, also riding your ";
              else
                buffer += fmt(" is here, also riding on %s's ") %
                  ch->pers(tbr->horseMaster());
              buffer += ch->persfname(riding);
              if (tbr->isAffected(AFF_INVISIBLE))
                buffer +=" (invisible)";
              if (tbr->isAffected(AFF_SHADOW_WALK))
                buffer +=" (shadow)";
              buffer +=".";
            } else
              buffer +=" is standing here.";

            break;
          case POSITION_FLYING:
            if (roomp && roomp->isUnderwaterSector()) 
              buffer +=" is swimming about.";
	    else
	      buffer +=" is flying about.";
            break;
          case POSITION_STANDING:
            if(roomp->isWaterSector())
              buffer +=" is floating here.";
            else if (isCombatMode(ATTACK_BERSERK))
              buffer += fmt(" is standing here with a crazy glint in %s eye.") % hshr();
            else
              buffer +=" is standing here.";
            break;
          case POSITION_CRAWLING:
            buffer +=" is crawling here.";
            break;
          case POSITION_SITTING:
            if (riding) {
              buffer +=" is sitting on ";
              if (riding->getName())
                buffer += ch->objs(riding);
              else
                buffer +="A bad object";

              buffer +=".";
            } else
              buffer +=" is sitting here.";
            break;
          case POSITION_RESTING:
            if (roomp->isWaterSector())
              buffer +="is resting here in the water.";
            else if (riding) {
              buffer +=" is resting on ";
              if (riding->getName())
                buffer += ch->objs(riding);
              else
                buffer +="A bad object";

              buffer +=".";
            } else
              buffer +=" is resting here.";
            break;
          case POSITION_SLEEPING:
            if(roomp->isWaterSector())
              buffer +=" is sleeping here in the water.";
            else if (riding) {
              buffer +=" is sleeping on ";
              if (riding->getName())
                buffer += ch->objs(riding);
              else
                buffer +="A bad object";

              buffer +=".";
            } else
              buffer +=" is sleeping here.";
            break;
          default:
            buffer +=" is floating here.";
            break;
        }
      }
      buffer +="\n\r";
      buffer = colorString(ch,ch->desc,buffer,NULL,COLOR_MOBS, TRUE);
      ch->sendTo(buffer);
    } else {        // npc with long 
      if (isAffected(AFF_INVISIBLE))
        buffer = "*";
      else
        buffer = "";

      capbuf = addNameToBuf(ch, ch->desc, this, getLongDesc(), COLOR_MOBS);
      // let's concat the name of a loser god that didn't put it in their
      // long desc
      if (isPc() && (capbuf.find(getName()) == sstring::npos) && ch->isImmortal() &&
          (hasWizPower(POWER_WIZARD) || ch->GetMaxLevel() > GetMaxLevel()) &&
          ch->isPc())
        capbuf += fmt(" (%s)") % getName();
      while (capbuf.find("$$g") != sstring::npos)
        capbuf.replace(capbuf.find("$$g"), 3,
                      (roomp ? roomp->describeGround() : "TELL A GOD"));
      while (capbuf.find("$g") != sstring::npos)
        capbuf.replace(capbuf.find("$g"), 2,
                      (roomp ? roomp->describeGround() : "TELL A GOD"));
      buffer += capbuf.cap();
      buffer.assign(buffer, 0, buffer.find_last_not_of("\n\r "));
   
      if (isPet(PETTYPE_THRALL))
        buffer += " (thrall)";
      if (isPet(PETTYPE_CHARM))
        buffer += " (charm)";
      if (isPet(PETTYPE_PET))
        buffer += " (pet)";
      if (ch->isImmortal() && isLinkdead())
        buffer += " (link-dead)";
      if (getTimer() >= 10)
        buffer += " (AFK)";
      if (desc && desc->connected)
        buffer += " (editing)";
      if (ch->isImmortal() && isDiurnal())
        buffer += " (diurnal)";
      if (ch->isImmortal() && isNocturnal())
        buffer += " (nocturnal)";

      buffer += "\n\r";
      buffer = colorString(ch,ch->desc,buffer,NULL,COLOR_MOBS, TRUE);
      ch->sendTo(buffer);
    }

    if (task && task->task != TASK_SIT && 
        task->task != TASK_REST && 
        task->task != TASK_SLEEP)
      ch->sendTo(COLOR_MOBS,fmt(".....%s is busy %s.\n\r") % getName() % tasks[task->task].name);
    if (checkSlots() && getPosition() == POSITION_SITTING)
      act(".....$n is sitting at the slot machine!", FALSE, this, 0, ch, TO_VICT);
    if(!ch->isPlayerAction(PLR_BRIEF))
      describeSpellEffects(this, ch, FALSE);
  } else if (mode == SHOW_MODE_SHORT_PLUS) {
    if (getDescr()) {
      capbuf = addNameToBuf(ch, ch->desc, this, getDescr(), COLOR_MOBS);
      while (capbuf.find("$$g") != sstring::npos)
        capbuf.replace(capbuf.find("$$g"), 3,
                        (roomp ? roomp->describeGround() : "TELL A GOD"));
      while (capbuf.find("$g") != sstring::npos)
        capbuf.replace(capbuf.find("$g"), 2,
                        (roomp ? roomp->describeGround() : "TELL A GOD"));
      capbuf = capbuf.cap();
      ch->sendTo(COLOR_MOBS, fmt("%s") % capbuf);
    } else 
      act("You see nothing special about $m.", FALSE, this, 0, ch, TO_VICT);
    
    describeSpellEffects(this, ch, TRUE);

    if (isPc() || (getRace() <= RACE_OGRE)) {
      buffer = fmt("$n is of the %s race.") % sstring(race->proper_name).uncap();
      buffer = colorString(ch,ch->desc,buffer,NULL,COLOR_MOBS, TRUE);
      act(buffer, FALSE, this, 0, ch, TO_VICT);
    }
    if (riding && dynamic_cast<TObj *>(riding)) {
      buffer = fmt("$n is %s on %s.") % 
        sstring(position_types[getPosition()]).uncap() % 
        riding->getName();
      buffer = colorString(ch,ch->desc,buffer,NULL,COLOR_MOBS, TRUE);
      act(buffer, FALSE, this, 0, ch, TO_VICT);
    }
    if (riding && dynamic_cast<TBeing *>(riding)) {
      buffer = fmt("$n is mounted on $p.");
      act(buffer, FALSE, this, riding, ch, TO_VICT);
    }
    if (rider && dynamic_cast<TBeing *>(rider)) {
      buffer = fmt("$n is ridden by $p.");
      act(buffer, FALSE, this, horseMaster(), ch, TO_VICT);
      for (t = rider; t; t = t->nextRider) {
        if (t == horseMaster())
          continue;
        if (!dynamic_cast<TBeing *>(t))
          continue;
        buffer = fmt("$n is also being ridden by $p.");
        act(buffer, FALSE, this, t, ch, TO_VICT);
      }
    }

    if (hitLimit() > 0)
      percent = (100 * getHit()) / hitLimit();
    else
      percent = -1;        // How could MAX_HIT be < 1?? 

    if (percent >= 100)
      buffer = "$n looks healthy.";
    else if (percent >= 90)
      buffer = "$n looks slightly scratched.";
    else if (percent >= 75)
      buffer = "$n has some small wounds and bruises.";
    else if (percent >= 50)
      buffer = "$n has many wounds.";
    else if (percent >= 30)
      buffer = "$n has large wounds and scratches.";
    else if (percent >= 15)
      buffer = "$n looks very hurt.";
    else if (percent >= 0)
      buffer = "$n is in awful condition.";
    else
      buffer = "$n has many large wounds and is near death.";
    act(buffer, FALSE, this, 0, ch, TO_VICT);

    if (!isPc()) {
      percent = dynamic_cast<const TMonster *>(this)->anger();
      if (percent >= 95)
        buffer = "$n is raving mad!";
      else if (percent >= 80)
        buffer = "$n is infuriated.";
      else if (percent >= 65)
        buffer = "$n is fuming.";
      else if (percent >= 50)
        buffer = "$n is really angry.";
      else if (percent >= 35)
        buffer = "$n seems mad.";
      else if (percent >= 20)
        buffer = "$n seems irritated.";
      else if (percent >= 5)
        buffer = "$n is displeased.";
      else 
        buffer = "$n seems peaceful.";
      act(buffer, FALSE,this, 0,ch,TO_VICT);
    }
    if (curStats.get(STAT_STR) > 190)
      buffer = "$e is unhumanly muscular, ";
    else if (curStats.get(STAT_STR) > 170)
      buffer = "$e is extremely brawny, ";
    else if (curStats.get(STAT_STR) > 150)
      buffer = "$e is brawny, ";
    else if (curStats.get(STAT_STR) > 130)
      buffer = "$e is muscular, ";
    else if (curStats.get(STAT_STR) > 110)
      buffer = "$e is fairly muscular, ";
    else if (curStats.get(STAT_STR) > 90)
      buffer = "$e has an average build, ";
    else if (curStats.get(STAT_STR) > 70)
      buffer = "$e is somewhat frail, ";
    else if (curStats.get(STAT_STR) > 50)
      buffer = "$e is frail, ";
    else if (curStats.get(STAT_STR) > 30)
      buffer = "$e is feeble, ";
    else
      buffer = "$e is extremely feeble, ";

    if (curStats.get(STAT_AGI) > 190)
      buffer += "astoundingly agile, and ";
    else if (curStats.get(STAT_AGI) > 160)
      buffer += "amazingly agile, and ";
    else if (curStats.get(STAT_AGI) > 140)
      buffer += "agile, and ";
    else if (curStats.get(STAT_AGI) > 120)
      buffer += "graceful, and ";
    else if (curStats.get(STAT_AGI) > 80)
      buffer += "able-bodied, and ";
    else if (curStats.get(STAT_AGI) > 60)
      buffer += "clumsy, and ";
    else if (curStats.get(STAT_AGI) > 30)
      buffer += "awkward and clumsy, and ";
    else
      buffer += "a bumbling clutz, and ";
   
    if (curStats.get(STAT_CHA) > 190)
      buffer += "has godlike grace.";
    else if (curStats.get(STAT_CHA) > 160)
      buffer += "is exceptionally well-favored.";
    else if (curStats.get(STAT_CHA) > 140)
      buffer += "is well-favored.";
    else if (curStats.get(STAT_CHA) > 120)
      buffer += "is attractive.";
    else if (curStats.get(STAT_CHA) > 100)
      buffer += "is comely.";
    else if (curStats.get(STAT_CHA) > 80)
      buffer += "is average looking.";
    else if (curStats.get(STAT_CHA) > 60)
      buffer += "is dumpy.";
    else if (curStats.get(STAT_CHA) > 40)
      buffer += "is ill-favored.";
    else
      buffer += "is extremely ill-favored.";

    if (isHumanoid())
      act(buffer, TRUE,this, 0,ch,TO_VICT);

    buffer = fmt("$e is about %d'%d\" tall and weighs around %d pounds.") %
      (getHeight()/INCHES_PER_FOOT) % (getHeight()%INCHES_PER_FOOT) %
      ((int) getWeight());
    act(buffer, TRUE,this,0,ch,TO_VICT);

    act("\n\r",FALSE,this,0,ch,TO_VICT);
    found = FALSE;
    wearSlotT ij;
    for (ij = MIN_WEAR; ij < MAX_WEAR; ij++) {
      if (equipment[ij]) {
        if (ch->canSee(equipment[ij]))
          found = TRUE;
      }
      if (isLimbFlags(ij, PART_TRANSFORMED))
          found = TRUE;
    }
    TDatabase db(DB_SNEEZY);
    sstring tattoos[MAX_WEAR];

    db.query("select location, tattoo from tattoos where name='%s' order by location",getName());
    while(db.fetchRow()){
      tattoos[convertTo<int>(db["location"])]=db["tattoo"];
      found=true;
    }

    if (found && ch->GetMaxLevel() != GOD_LEVEL1) {
      act("$n is using:", FALSE, this, 0, ch, TO_VICT);
      for (ij = MIN_WEAR; ij < MAX_WEAR; ij++) {
        if (isLimbFlags(ij, PART_TRANSFORMED)) {
          if (shouldDescTransLimb(ij)) {
            buf = fmt("<%s>") % describeTransEquipSlot(ij);
            ch->sendTo(fmt("%s\n\r") % buf);
          }
        }
        if (equipment[ij] && ch->canSee(equipment[ij])) {
          if (!equipment[ij]->shouldntBeShown(ij)) {
            buf = fmt("<%s>") % describeEquipmentSlot(ij);
            ch->sendTo(fmt("%-26s") % buf);
            ch->showTo(equipment[ij], SHOW_MODE_SHORT_PLUS);
          }
        } else if(tattoos[ij]!=""){
	  sstring slot = describeEquipmentSlot(ij);
	  buf = fmt("<%s>") % (slot.find("Worn") != sstring::npos ? slot.replace(slot.find("Worn"),4,"Tattooed") : slot);


	  //	  sprintf(buf, "<%s>", describeEquipmentSlot(ij).c_str());
	  ch->sendTo(fmt("%-26s") %buf);
	  ch->sendTo(COLOR_BASIC, tattoos[ij]);
	  ch->sendTo("\n\r");
	}
      }
    }

    ch->describeLimbDamage(this);

    found = FALSE;
    if ((ch != this) && !ch->isImmortal() && ch->affectedBySpell(SKILL_SPY)) {
      int value = getMoney();
      int skill = ch->getSkillValue(SKILL_SPY);
      ch->sendTo("\n\rYou attempt to peek at the inventory:\n\r");
      if (ch->isAffected(AFF_SCRYING)) {
        list_in_heap(getStuff(), ch, FALSE, skill);
      }

      // randomize wealth report
      // this is anywhere from 0* to 2* as much at 0 learning, to exact at 100%
      if (!ch->isAffected(AFF_SCRYING))
        skill = 0;
      value *= ::number(skill/10, (20-skill/10));
      value /= 10;
      ch->sendTo(COLOR_MOBS,fmt("\n\rYou estimate %s has %d talens.\n\r") % getName() % value);

    } else if (ch->isImmortal()) {
      ch->sendTo("Inventory:\n\r");

      for (t = getStuff(); t; t = t->nextThing) {
        ch->showTo(t, SHOW_MODE_SHORT_PLUS);
        found = TRUE;
      }
      if (!found)
        ch->sendTo("Nothing.\n\r");
    }
  } else if (mode == SHOW_MODE_SHORT_PLUS_INV) {
    act("$N is carrying:", FALSE, ch, 0, this, TO_CHAR);
    list_in_heap(getStuff(), ch, false, 100);
  }
}

void TBeing::show_me_mult_to_char(TBeing *ch, showModeT, unsigned int num) const
{
  char buffer[MAX_STRING_LENGTH];
  char tmp[10];
  char capbuf[256];

  if (!ch->canSee(this)) {
    if (ch->canSee(this, INFRA_YES)) {
      if (num == 1)
        sprintf(buffer,"A blob of heat is here in the shape of %s %s.\n\r",
          getMyRace()->getSingularName().startsVowel() ? "an" : "a",
          getMyRace()->getSingularName().c_str());
      else
        sprintf(buffer,"A few blobs of heat are here in the shape of %s.\n\r",
          getMyRace()->getPluralName().c_str());
      ch->sendTo(buffer);
    } else if (ch->isAffected(AFF_SENSE_LIFE) && (GetMaxLevel() < 51))
      if (num == 1)
        ch->sendTo("You sense a hidden life form in the room.\n\r");
      else
        ch->sendTo("You sense hidden life forms in the room.\n\r");
    return;
  }
  const TMonster *tm = dynamic_cast<const TMonster *>(this);
  if (riding || spelltask || fight() ||
        (roomp->isWaterSector() && !isAffected(AFF_WATERBREATH)) ||
        !(getLongDesc()) ||
        (tm && tm->getPosition() != tm->default_pos)) {
    // A player char or a mobile without long descr, or not in default pos. 
    strcpy(buffer, getName());
    strcpy(buffer, sstring(buffer).cap().c_str());
    if (isAffected(AFF_INVISIBLE))
      strcat(buffer, " (invisible)");
    if (isAffected(AFF_SHADOW_WALK))
      strcat(buffer, " (shadow)");
    if (isPet(PETTYPE_THRALL))
      strcat(buffer, " (thrall)");
    if (isPet(PETTYPE_CHARM))
      strcat(buffer, " (charm)");
    if (isPet(PETTYPE_PET))
      strcat(buffer, " (pet)");
    if (ch->isImmortal() && isLinkdead())
      strcat(buffer, " (link-dead)");
    if (getTimer() >= 10)
      strcat(buffer, " (AFK)");
    if (desc && desc->connected)
      strcat(buffer, " (editing)");
    if (ch->isImmortal() && isDiurnal())
      strcat(buffer, " (diurnal)");
    if (ch->isImmortal() && isNocturnal())
      strcat(buffer, " (nocturnal)");

    if (fight()) {
      strcat(buffer, " is here, fighting ");
      if (fight() == ch)
        strcat(buffer, " YOU!");
      else {
        if (sameRoom(*fight())) {
          strcat(buffer, ch->pers(fight()));
          strcat(buffer, ".");
        } else
          strcat(buffer, "someone who has already left.");
      }
    } else {
      switch (getPosition()) {
        case POSITION_STUNNED:
          strcat(buffer, " is lying here, stunned.");
          break;
        case POSITION_INCAP:
          strcat(buffer, " is lying here, incapacitated.");
          break;
        case POSITION_MORTALLYW:
          strcat(buffer, " is lying here, mortally wounded.");
          break;
        case POSITION_DEAD:
          strcat(buffer, " is lying here, dead.");
          break;
        case POSITION_FLYING:
          if (roomp && roomp->isUnderwaterSector()) 
            strcat(buffer, " is swimming about.");
	  else
	    strcat(buffer, " is flying about.");
          break;
        case POSITION_STANDING:
          strcat(buffer, " is standing here.");
          break;
        case POSITION_CRAWLING:
          strcat(buffer, " is crawling here.");
          break;
        case POSITION_SITTING:
          if (riding) {
            strcat(buffer, " is sitting here on ");
            if (riding->getName())
              strcat(buffer,ch->objs(riding));
            else
              strcat(buffer, "A bad object");

            strcat(buffer,".");
          } else
            strcat(buffer, " is sitting here.");
          break;
        case POSITION_RESTING:
          if (riding) {
            strcat(buffer, " is resting here on ");
            if (riding->getName())
              strcat(buffer,ch->objs(riding));
            else
              strcat(buffer, "A bad object");

            strcat(buffer,".");
          } else
            strcat(buffer, " is resting here.");
          break;
        case POSITION_SLEEPING:
          if (riding) {
            strcat(buffer, " is sleeping here on ");
            if (riding->getName())
// COSMO MARKER --test
              if (dynamic_cast<TBeing *>(riding)) {
                strcat(buffer,ch->objs(riding));
              } else {
                if (IS_SET(ch->desc->plr_color, PLR_COLOR_OBJECTS)) {
                  sprintf(capbuf, "%s", colorString(ch, ch->desc, ch->objs(riding), NULL, COLOR_OBJECTS, FALSE).c_str());

                  sprintf(capbuf, "%s", ch->objs(riding));
                  strcat(buffer, capbuf);
//                  strcat(buffer,ch->objs(riding));
                }
              }
            else
              strcat(buffer, "A bad object");

            strcat(buffer,".");
          } else
            strcat(buffer, " is sleeping here.");
          break;
        default:
          strcat(buffer, " is floating here.");
          break;
      }
    }
    if (num > 1) {
      sprintf(tmp, " [%d]", num);
      strcat(buffer, tmp);
    }
    strcat(buffer, "\n\r");
// COSMO MARKER -- test
    ch->sendTo(COLOR_MOBS, sstring(buffer).cap().c_str());
  } else {  // npc with long 
    if (isAffected(AFF_INVISIBLE)) 
      strcpy(buffer, "*");
    else 
      *buffer = '\0';
    
    sprintf(capbuf, "%s", addNameToBuf(ch, ch->desc, this, getLongDesc(), COLOR_MOBS).c_str());
    sstring cStrbuf = capbuf;
    while (cStrbuf.find("$$g") != sstring::npos)
      cStrbuf.replace(cStrbuf.find("$$g"), 3,
                      (roomp ? roomp->describeGround().c_str() : "TELL A GOD"));
    while (cStrbuf.find("$g") != sstring::npos)
      cStrbuf.replace(cStrbuf.find("$g"), 2,
                      (roomp ? roomp->describeGround().c_str() : "TELL A GOD"));
    strcpy(capbuf, cStrbuf.c_str());
    strcpy(capbuf, sstring(capbuf).cap().c_str());
    strcat(buffer, capbuf);

    if (num > 1) {
      while ((buffer[strlen(buffer) - 1] == '\r') ||
             (buffer[strlen(buffer) - 1] == '\n') ||
             (buffer[strlen(buffer) - 1] == ' ')) {
        buffer[strlen(buffer) - 1] = '\0';
      }
      if (isPet(PETTYPE_THRALL))
        strcat(buffer, " (thrall)");
      if (isPet(PETTYPE_CHARM))
        strcat(buffer, " (charm)");
      if (isPet(PETTYPE_PET))
        strcat(buffer, " (pet)");
      if (ch->isImmortal() && isLinkdead())
        strcat(buffer, " (link-dead)");
      if (getTimer() >= 10)
        strcat(buffer, " (AFK)");
      if (desc && desc->connected)
        strcat(buffer, " (editing)");
      if (ch->isImmortal() && isDiurnal())
        strcat(buffer, " (diurnal)");
      if (ch->isImmortal() && isNocturnal())
        strcat(buffer, " (nocturnal)");

      sprintf(tmp, " [%d]\n\r", num);
      strcat(buffer, tmp);
    }
      sprintf(buffer,"%s",colorString(ch,ch->desc,buffer,NULL,COLOR_MOBS, TRUE).c_str());
      ch->sendTo(buffer);

//    ch->sendTo(COLOR_MOBS, buffer);
  }
#if 1
  // these no longer show up, just seen on look-at which isn't part of 
  // show mult
  describeSpellEffects(this, ch, FALSE);
#endif
}

void TBeing::doGlance(const char *argument)
{
  char buffer[1024];
  int percent;
  TBeing *i;

  for (; isspace(*argument); argument++);

  if (*argument) {
    if ((i = get_char_room_vis(this, argument))) {
      if (i->hitLimit() > 0)
        percent = (100 * i->getHit()) / i->hitLimit();
      else
        percent = -1;                // How could MAX_HIT be < 1?? 

      if (percent >= 100)
        strcpy(buffer, "$n looks healthy.");
      else if (percent >= 90)
        strcpy(buffer, "$n looks slightly scratched.");
      else if (percent >= 75)
        strcpy(buffer, "$n has some small wounds and bruises.");
      else if (percent >= 50)
        strcpy(buffer, "$n has many wounds.");
      else if (percent >= 30)
        strcpy(buffer, "$n has large wounds and scratches.");
      else if (percent >= 15)
        strcpy(buffer, "$n looks very hurt.");
      else if (percent >= 0)
        strcpy(buffer, "$n is in awful condition.");
      else
        strcpy(buffer, "$n has many large wounds and is near death.");

      act(buffer, FALSE, i, 0, this, TO_VICT);
      describeLimbDamage(i);
//      describeAffects(i);
      sendTo(COLOR_MOBS, fmt("%s look%s %s.\n\r") %
	     (i == this ? "You" : sstring(i->getName()).cap()) %
	     (i == this ? "" : "s") %
        DescMoves((((double) i->getMove()) / ((double) i->moveLimit()))));

      describeSpellEffects(i, this, TRUE);
    } else {
      sendTo("You don't see that here.\n\r");
      return;
    }
  } else {
    sendTo("You usually glance at someone!\n\r");
    return;
  }
}

sstring TStaff::getNameForShow(bool useColor, bool useName, const TBeing *ch) const
{
  char buf2[256];
  sprintf(buf2, "%s [%s]", 
       useName ? name : (useColor ? getName() : getNameNOC(ch).c_str()),
       (getSpell() >= 0 ? (discArray[getSpell()] ? discArray[getSpell()]->name : "Unknown") : "None"));
  return buf2;
}

sstring TScroll::getNameForShow(bool useColor, bool useName, const TBeing *ch) const
{
  char buf2[256];
  sprintf(buf2, "%s [%s/%s/%s]", 
       useName ? name : (useColor ? getName() : getNameNOC(ch).c_str()),
       (getSpell(0) >= 0 ? (discArray[getSpell(0)] ? discArray[getSpell(0)]->name : "Unknown") : "None"),
       (getSpell(1) >= 0 ? (discArray[getSpell(1)] ? discArray[getSpell(1)]->name : "Unknown") : "None"),
       (getSpell(2) >= 0 ? (discArray[getSpell(2)] ? discArray[getSpell(2)]->name : "Unknown") : "None"));

  return buf2;
}
