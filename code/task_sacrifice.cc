#include "stdsneezy.h"

int task_sacrifice(TBeing *ch, cmdTypeT cmd, const char *, int pulse, TRoom *, TObj *obj)
{
  TThing *t, *t2;
  TMonster *guard;
  TTool *totem=NULL;
  int found=0;
  //  TBeing *dummy;
  int learning = ch->getSkillValue(SKILL_SACRIFICE);
  TBaseCorpse *corpse = dynamic_cast<TBaseCorpse *>(obj);
  int percent = ::number(1, 100);
  int factor = ((::number(0,2)) * ch->getSkillValue(SKILL_SACRIFICE));
  int factor2 = ((((::number(0,2)) * ch->getSkillValue(SKILL_SACRIFICE)) / 10) * 6);
 
  if (ch->utilityTaskCommand(cmd) || ch->nobrainerTaskCommand(cmd))
    return FALSE;

  if (ch->isLinkdead() || (ch->in_room != ch->task->wasInRoom)) {
    act("You cease the ritual sacrifice of $p.", FALSE, ch, corpse, 0, TO_CHAR);
    act("$n stops trying to sacrifice $p.", TRUE, ch, corpse, 0, TO_ROOM);
    ch->stopTask();
    return FALSE;
  }

  if ((obj != ch->heldInPrimHand()) && (obj != ch->heldInSecHand())) {
    for (t = ch->roomp->stuff; t; t = t2) {
      t2 = t->nextThing;
      if (obj == dynamic_cast<TObj *>(t))
        found = 1;
    }
    if (!found) {
      act("You can't find the object of the ritual! Wasn't there a corpse here?", FALSE, ch, 0, 0, TO_CHAR);
      act("$n stops singing and looks confused.", TRUE, ch, 0, 0, TO_ROOM);
      ch->stopTask();
      return FALSE;
    }
  }

  if (!(t = get_thing_char_using(ch, "totem", 0, FALSE, FALSE)) || !(totem=dynamic_cast<TTool *>(t))) {
    ch->sendTo("You need to own a totem to perform the ritual.\n\r");
    ch->stopTask();
    return FALSE;
  }

  for (t = ch->roomp->stuff; t; t = t2) {
    t2 = t->nextThing;
    guard = dynamic_cast<TMonster *>(t);
    if (!guard)
      continue;
    if (!guard->isPolice() || !guard->canSee(ch) || !guard->awake())
      continue;
    guard->doSay("Hey! Get the hell out of here! Damn Voodoo Witch!");
    act("You cease the ritual sacrifice of $p.", FALSE, ch, corpse, 0, TO_CHAR);
    act("$n stops chanting over the corpse of $p.", TRUE, ch, corpse, 0, TO_ROOM);
    ch->stopTask();
    return FALSE;
  }

  if (ch->task->timeLeft < 0){
    act("You have completed the sacrifice of $p.", FALSE, ch, corpse, 0, TO_CHAR);
    act("$n has completed $s ritual sacrifice of $p.", FALSE, ch, corpse, 0, TO_ROOM);
    ch->stopTask();
    return DELETE_ITEM;
  }

  if (percent < ch->getSkillValue(SKILL_SACRIFICE)) {
    act("$p is being accepted by the loa.", FALSE, ch, corpse, 0, TO_CHAR);
    ch->addToLifeforce(factor);
    ch->updatePos();
  } else {
    ch->addToLifeforce(-factor2);
    if (0 >= ch->getLifeforce()) {
      ch->setLifeforce(0);
      act("The loa demands that you cease this vain sacrifice, and you comply.", FALSE, ch, 0, 0, TO_CHAR);
      ch->addToHit(-2);
      ch->updatePos();
      ch->task->timeLeft = -1;
    }
  }

  switch (cmd) {
  case CMD_TASK_CONTINUE:
    ch->task->calcNextUpdate(pulse, 2);
    totem->addToToolUses(-1);
    if (totem->getToolUses() <= 0) {
      act("Your $o has been confiscated by the loa! It must have been too weak.", FALSE, ch, totem, 0, TO_CHAR);
      act("$n looks pale as $s $o shatters.", FALSE, ch, totem, 0, TO_ROOM);
      ch->stopTask();
      delete totem;
      return FALSE;
    }
    switch (ch->task->timeLeft) {
      case 2:
	if (!corpse) {
	  act("You can't find the object of the ritual! Wasn't there $p's corpse here?", FALSE, ch, corpse, 0, TO_CHAR);
	  act("$n stops singing and looks confused.", TRUE, ch, 0, 0, TO_ROOM);
	  return FALSE;
	} else {
  	  if (10 >= corpse->obj_flags.decay_time) {
	    corpse->obj_flags.decay_time = 10;
	  }
          act("You continue the rada song to the loa in hopes they will accept your sacrifice.", FALSE, ch, 0, 0, TO_CHAR);
          act("$n sings in an unfamiliar tongue over $p.", TRUE, ch, corpse, 0, TO_ROOM);
	  if (bSuccess(ch, learning, SKILL_SACRIFICE)) {
	    ch->task->timeLeft--;
	  } else {
	    ch->task->timeLeft = 2;
	  }
	}
	break;
      case 1:
	if (!corpse) {
	  act("You can't find the object of the ritual! Wasn't there a corpse here?", FALSE, ch, 0, 0, TO_CHAR);
	  act("$n stops singing and looks confused.", TRUE, ch, 0, 0, TO_ROOM);
	  return FALSE;
	} else {
  	  if (10 >= corpse->obj_flags.decay_time) {
	    corpse->obj_flags.decay_time = 10;
	  }
          act("Your $o's eyes glow <r>blood red<1>.", FALSE, ch, totem, 0, TO_CHAR);
          act("The eyes on $n's $o begin to glow a <r>blood red<1>.", TRUE, ch, totem, 0, TO_ROOM);
	  if (bSuccess(ch, learning, SKILL_SACRIFICE)) {
	    ch->task->timeLeft--;
 	  } else {
	    ch->task->timeLeft = 1;
	  }
	}
	break;
      case 0:
	if (!corpse) {
	  act("You can't find the object of the ritual! Wasn't there a corpse here?", FALSE, ch, 0, 0, TO_CHAR);
	  act("$n stops singing and looks confused.", TRUE, ch, 0, 0, TO_ROOM);
	  return FALSE;
	} else {
  	  if (10 >= corpse->obj_flags.decay_time) {
	    corpse->obj_flags.decay_time = 10;
	  }
	  act("You continue to sing the rada song over $p.", FALSE, ch, corpse, 0, TO_CHAR);
	  act("$n's ritual sacrifice causes $p's face to glow <G>pale green<1>.", FALSE, ch, corpse, 0, TO_ROOM);
	  if (bSuccess(ch, learning, SKILL_SACRIFICE)) {
	    ch->task->timeLeft--;
	  } else {
	    ch->task->timeLeft = 0;
	  }
	}
	break;
    }
    break;
  case CMD_ABORT:
  case CMD_STOP:
      act("You stop performing the ritual.", FALSE, ch, 0, 0, TO_CHAR);
      act("$n stops singing.", TRUE, ch, 0, 0, TO_ROOM);
      ch->stopTask();
      break;
  case CMD_TASK_FIGHTING:
      ch->sendTo("You can't sacrifice a corpse while under attack.\n\r");
      ch->stopTask();
      break;
  default:
      if (cmd < MAX_CMD_LIST)
        warn_busy(ch);
      break;
  }
  return TRUE;
}
