//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//////////////////////////////////////////////////////////////////////////

// this code sucks and should be rewritten from scratch at some point

#include "stdsneezy.h"
#include "obj_vehicle.h"


const char *vehicle_speed[] = {"stop", "slow", "medium", "fast"};


TVehicle::TVehicle() :
  TPortal(),
  dir(DIR_NONE),
  speed(0)
{
}

TVehicle::TVehicle(const TVehicle &a) :
  TPortal(a)
{
}

TVehicle & TVehicle::operator=(const TVehicle &a)
{
  if (this == &a) return *this;
  TPortal::operator=(a);
  return *this;
}

TVehicle::~TVehicle()
{
}



void TVehicle::assignFourValues(int x1, int x2, int x3, int x4)
{
    
  setTarget(x1);
  setType(x2);


  setPortalKey(GET_BITS(x4, 23, 24));
  setPortalFlags(GET_BITS(x4, 31, 8));
}

void TVehicle::getFourValues(int *x1, int *x2, int *x3, int *x4) const
{
  *x1=getTarget();
  *x2=getType();

  int r = *x4;
  SET_BITS(r, 23, 24, getPortalKey());
  SET_BITS(r, 31, 8, getPortalFlags());
  *x4 = r;
}




unsigned char TVehicle::getPortalType() const
{
  return 0;
}


char TVehicle::getPortalNumCharges() const
{
  return -1;
}


void TVehicle::lookObj(TBeing *ch, int) const
{
  string buf;

  ssprintf(buf, "%d look", getTarget());
  ch->doAt(buf.c_str(), true);
}



void TVehicle::driveSpeed(TBeing *ch, string arg)
{
  int nspeeds=4;

  if(getDir() == DIR_NONE){
    ch->sendTo("You need to choose a direction to drive in first.\n\r");
    return;
  }

  for(int i=0;i<nspeeds;++i){
    if(is_abbrev(arg, vehicle_speed[i])){
      if(vehicle_speed[i] == "stop"){
	if(vehicle_speed[getSpeed()] == "fast"){
	  if(getType()==VEHICLE_BOAT){
	    act("$n throws the anchor overboard, bringing $p to a rough halt.",
		0, ch, this, 0, TO_ROOM);
	    act("You throw the anchor overboard, bringing $p to a rough halt.",
		0, ch, this, 0, TO_CHAR);
	  } else {
	    act("$n slams on the brakes, bringing $p to a skidding halt.",
		0, ch, this, 0, TO_ROOM);
	    act("You slam on the brakes, bringing $p to a skidding halt.", 
		0, ch, this, 0, TO_CHAR);
	  }
	} else if(vehicle_speed[getSpeed()] == "stop"){
	  act("$p is already stopped.", 0, ch, this, 0, TO_CHAR);
	} else {
	  act("$n brings $p to a stop.",
	      0, ch, this, 0, TO_ROOM);
	  act("You bring $p to a stop.",
	      0, ch, this, 0, TO_CHAR);
	}

	setSpeed(i);
      } else {
	if(i > getSpeed()){
	  act("$p begins to accelerate.", 0, ch, this, 0, TO_ROOM);
	  act("$p begins to accelerate.", 0, ch, this, 0, TO_CHAR);
	} else if(i < getSpeed()){
	  act("$p begins to decelerate.", 0, ch, this, 0, TO_ROOM);
	  act("$p begins to decelerate.", 0, ch, this, 0, TO_CHAR);
	} else {
	  act("$p is already going that speed.", 0, ch, this, 0, TO_CHAR);
	}

	setSpeed(i);
      }
      return;
    }
  }

  ch->sendTo("You can't figure out how to drive that fast.\n\r");
}

void TVehicle::driveDir(TBeing *ch, dirTypeT dir)
{
  string buf;

  setDir(dir);

  ssprintf(buf, "$n directs $p to the %s.", dirs[dir]);
  act(buf.c_str(), 0, ch, this, 0, TO_ROOM);
  ssprintf(buf, "You direct $p to the %s.", dirs[dir]);
  act(buf.c_str(), 0, ch, this, 0, TO_CHAR);
}


void TVehicle::driveExit(TBeing *ch)
{
  act("$n exits $p.", 0, ch, this, 0, TO_ROOM);
  act("You exit $p.", 0, ch, this, 0, TO_CHAR);

  --(*ch);
  *roomp+=*ch;
}

void TVehicle::driveLook(TBeing *ch, bool silent=false)
{
  string buf;

  if(!silent)
    ch->sendTo("You look outside.\n\r");

  ssprintf(buf, "%d look", in_room);
  ch->doAt(buf.c_str(), true);
}


void TVehicle::vehiclePulse(int pulse)
{
  TThing *t;
  TBeing *tb;
  TRoom *troom=roomp;
  string buf;
  char shortdescr[256];
  vector<TBeing *>tBeing(0);

  if(!troom)
    return;

  if(getSpeed()==0)
    return;
  
  // this is where we regulate speed
  if(pulse % ((ONE_SECOND-2)/getSpeed()))
    return;

  if(getDir() == DIR_NONE)
    return;

  strcpy(shortdescr, shortDescr);
  cap(shortdescr);

  if(!troom->dir_option[getDir()]){
    // first count how many valid exits we have
    int dcount=0;
    for(dirTypeT dir=DIR_NORTH;dir<MAX_DIR;dir++){
      if(troom->dir_option[dir] && dir != rev_dir[getDir()])
	++dcount;
    }
    
    // if there's only one that isn't the way we came, change direction
    if(dcount == 1){
      for(dirTypeT dir=DIR_NORTH;dir<MAX_DIR;dir++){
	if(troom->dir_option[dir] && dir != rev_dir[getDir()]){
	  setDir(dir);

	  sendrpf(COLOR_OBJECTS, roomp, "%s changes direction to the %s and keeps moving.\n\r", shortdescr, dirs[getDir()]);

	  break;
	}
      }
    } else
      return; // otherwise just stop
  }

  // we let them go one room into non-water, like "beaching" the boat
  // or entering docks.
  TRoom *dest=real_roomp(troom->dir_option[getDir()]->to_room);
  if(getType() == VEHICLE_BOAT && !dest->isWaterSector() && 
     !roomp->isWaterSector()){
    return;
  }

  // send message to people in old room here
  if(getType()==VEHICLE_BOAT){
    if(vehicle_speed[getSpeed()] == "fast"){
      sendrpf(COLOR_OBJECTS, roomp, "%s sails rapidly to the %s.\n\r",
	      shortdescr, dirs[getDir()]);
    } else if(vehicle_speed[getSpeed()] == "medium"){
      sendrpf(COLOR_OBJECTS, roomp, "%s sails to the %s.\n\r",
	      shortdescr, dirs[getDir()]);
    } else if(vehicle_speed[getSpeed()] == "slow"){
      sendrpf(COLOR_OBJECTS, roomp, "%s drifts to the %s.\n\r",
	      shortdescr, dirs[getDir()]);
    }
  } else {
    if(vehicle_speed[getSpeed()] == "fast"){
      sendrpf(COLOR_OBJECTS, roomp, "%s speeds off to the %s.\n\r",
	      shortdescr, dirs[getDir()]);
    } else if(vehicle_speed[getSpeed()] == "medium"){
      sendrpf(COLOR_OBJECTS, roomp, "%s rolls off to the %s.\n\r",
	      shortdescr, dirs[getDir()]);
    } else if(vehicle_speed[getSpeed()] == "slow"){
      sendrpf(COLOR_OBJECTS, roomp, "%s creeps off to the %s.\n\r",
	      shortdescr, dirs[getDir()]);
    }
  }


  --(*this);
  thing_to_room(this, troom->dir_option[getDir()]->to_room);

  // send message to people in new room here
  if(getType() == VEHICLE_BOAT){
    if(vehicle_speed[getSpeed()] == "fast"){
      sendrpf(COLOR_OBJECTS, roomp, "%s sails rapidly in from the %s.\n\r",
	      shortdescr, dirs[rev_dir[getDir()]]);
      ssprintf(buf, "$p speeds %s.", dirs[getDir()]);
    } else if(vehicle_speed[getSpeed()] == "medium"){
      sendrpf(COLOR_OBJECTS, roomp, "%s sails in from the %s.\n\r",
	      shortdescr, dirs[rev_dir[getDir()]]);
      ssprintf(buf, "$p rolls %s.", dirs[getDir()]);
    } else if(vehicle_speed[getSpeed()] == "slow"){
      sendrpf(COLOR_OBJECTS, roomp, "%s drifts in from the %s.\n\r",
	      shortdescr, dirs[rev_dir[getDir()]]);
      ssprintf(buf, "$p creeps %s.", dirs[getDir()]);
    }
  } else {
    if(vehicle_speed[getSpeed()] == "fast"){
      sendrpf(COLOR_OBJECTS, roomp, "%s speeds in from the %s.\n\r",
	      shortdescr, dirs[rev_dir[getDir()]]);
      ssprintf(buf, "$p speeds %s.", dirs[getDir()]);
    } else if(vehicle_speed[getSpeed()] == "medium"){
      sendrpf(COLOR_OBJECTS, roomp, "%s rolls in from the %s.\n\r",
	      shortdescr, dirs[rev_dir[getDir()]]);
      ssprintf(buf, "$p rolls %s.", dirs[getDir()]);
    } else if(vehicle_speed[getSpeed()] == "slow"){
      sendrpf(COLOR_OBJECTS, roomp, "%s creeps in from the %s.\n\r",
	      shortdescr, dirs[rev_dir[getDir()]]);
      ssprintf(buf, "$p creeps %s.", dirs[getDir()]);
    }
  }
  
  // send message to people in vehicle
  troom=real_roomp(getTarget());


  // the doAt in driveLook() would screw up the getStuff list
  // so we "pre-cache" it
  for (t=troom->getStuff(); t; t = t->nextThing)
    if((tb=dynamic_cast<TBeing *>(t)))
      tBeing.push_back(tb);
  
  for(unsigned int tBeingIndex=0;tBeingIndex<tBeing.size();tBeingIndex++){
    act(buf.c_str(), 0, tBeing[tBeingIndex], this, 0, TO_CHAR);
    driveLook(tBeing[tBeingIndex], true);
  }
}


void TVehicle::driveStatus(TBeing *ch)
{
  string buf;

  ssprintf(buf, "$p is pointing to the %s.\n\r", dirs[getDir()]);
  act(buf.c_str(), 0, ch, this, 0, TO_CHAR);
  ssprintf(buf, "$p is traveling at a %s speed.\n\r",
	   vehicle_speed[getSpeed()]);
  act(buf.c_str(), 0, ch, this, 0, TO_CHAR);
}


