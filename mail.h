//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//////////////////////////////////////////////////////////////////////////


#ifndef __MAIL_H
#define __MAIL_H

const int MIN_MAIL_LEVEL=2;
const int STAMP_PRICE=50;
const int FACTION_STAMP_PRICE=5000;
const int MAX_MAIL_SIZE=4000;
const int NAME_SIZE =15;


extern bool has_mail(const sstring &);
extern bool no_mail;

extern void autoMail(TBeing *, const sstring &, const sstring &);

#endif
