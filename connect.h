#ifndef __CONNECT_H
#define __CONNECT_H

#include "obj_drug.h"

const unsigned int PROMPT_HIT               = (1<<0);
const unsigned int PROMPT_MANA              = (1<<1);
const unsigned int PROMPT_MOVE              = (1<<2);
const unsigned int PROMPT_GOLD              = (1<<3);
const unsigned int PROMPT_EXP               = (1<<4);
const unsigned int PROMPT_NAME              = (1<<5);
const unsigned int PROMPT_OPPONENT          = (1<<6);
const unsigned int PROMPT_CONDITION         = (1<<7);
const unsigned int PROMPT_COND_LDR          = (1<<8);
const unsigned int PROMPT_ROOM              = (1<<9);
const unsigned int PROMPT_COLOR             = (1<<10);
const unsigned int PROMPT_TANK              = (1<<11);
const unsigned int PROMPT_TANK_OTHER        = (1<<12);
const unsigned int PROMPT_BUILDER_ASSISTANT = (1<<13);
const unsigned int PROMPT_EXPTONEXT_LEVEL   = (1<<14);
const unsigned int PROMPT_VTANSI_BAR        = (1<<15);
const unsigned int PROMPT_PIETY             = (1<<16);
const unsigned int PROMPT_LIFEFORCE         = (1<<17);
// Add new prompt options here.
const unsigned int PROMPT_CLASSIC_ANSIBAR   = (1<<30);
const unsigned int PROMPT_CLIENT_PROMPT     = (1<<31);

enum termTypeT {
     TERM_NONE,  //         = 0;
     TERM_VT100,  //        = 1;
     TERM_ANSI   //         = 2;
};

enum connectStateT {
       CON_PLYNG,
       CON_NME,
       CON_NMECNF,
       CON_PWDNRM,
       CON_PWDCNF,
       CON_QSEX,
       CON_RMOTD,
       CON_QCLASS,
       CON_PWDNCNF,
       CON_WIZLOCK,
       CON_QRACE,
       CON_DELETE,
       CON_STAT_COMBAT,
       CON_STAT_COMBAT2,
       CON_QHANDS,
       CON_DISCON,
       CON_NEWACT,
       CON_ACTPWD,
       CON_NEWLOG,
       CON_NEWACTPWD,
       CON_EMAIL,
       CON_TERM,
       CON_CONN,
       CON_NEWPWD,
       CON_OLDPWD,
       CON_RETPWD,
       CON_DELCHAR,
       CON_ACTDELCNF,
       CON_EDITTING,
       CON_DISCLAIMER,
       CON_TIME,
       CON_CHARDELCNF,
       CON_DISCLAIMER2,
       CON_DISCLAIMER3,
       CON_WIZLOCKNEW,
       CON_STAT_LEARN,
       CON_STAT_UTIL,
       CON_CREATE_DONE,
       CON_STATS_START,
       CON_ENTER_DONE,
       CON_STATS_RULES,
       CON_STATS_RULES2,
       CON_HOME_HUMAN,
       CON_HOME_ELF,
       CON_HOME_DWARF,
       CON_HOME_GNOME,
       CON_HOME_OGRE,
       CON_HOME_HOBBIT,
       CON_PERMA_DEATH,
       CON_MULTIWARN,
       CON_TRAITS,
// if adding more here, update connected_types array as well
       MAX_CON_STATUS,
// these are intentionally higher than MAX_CON
       CON_REDITING,
       CON_OEDITING,
       CON_MEDITING,
       CON_HELP,
       CON_WRITING,
       CON_SEDITING
};

class TAccount;
class TPerson;
class TSocket;

// This stuff has to be here because we include connect.h
// in being.h, so it tries to use these before they are declared.

class commText
{
  private:
    char *text;
    commText *next;
  public:
    char * getText() {
      return text;
    }
    void setText(char * n) {
      text = n;
    }
    commText * getNext() {
      return next;
    }
    void setNext( commText * n) {
      next = n;
    }
   
    commText();
    commText(const commText &a);
    commText & operator=(const commText &a);
    ~commText();
};

class textQ
{
  private:
    commText *begin;
    commText *end;

  public:
    commText * getBegin() {
      return begin;
    }
    void setBegin(commText *n) {
      begin = n;
    }
    commText * getEnd() {
      return end;
    }
    void setEnd(commText *n) {
      end = n;
    }

  private:
    textQ() {} // prevent use
  public:
    textQ(bool);
    textQ(const textQ &a);
    textQ & operator=(const textQ &a);
    ~textQ();

    bool takeFromQ(char *dest, int destsize);
    void putInQ(const sstring &txt);
};

class editStuff
{
  public:
    int x, y;        // Current x andy position on the screen for cursor
    int bottom, end; // Bottom of text, and end of current line
    char **lines;    // Keeps up with text typed in

    editStuff();
    editStuff(const editStuff &a);
    ~editStuff();
};
    
class careerData
{
  public:
    unsigned int kills;            // keep up with kills I've made
    unsigned int group_kills;            // keep up with kills I've made
    unsigned int deaths;           // total deaths I've suffered
    double exp;                // total xp ever gained
    unsigned int crit_hits;
    unsigned int crit_hits_suff;
    unsigned int crit_misses;
    unsigned int crit_kills;
    unsigned int crit_kills_suff;
    unsigned int crit_beheads;
    unsigned int crit_beheads_suff;
    unsigned int crit_sev_limbs;
    unsigned int crit_sev_limbs_suff;
    unsigned int crit_cranial_pierce;
    unsigned int crit_cranial_pierce_suff;
    unsigned int crit_broken_bones;
    unsigned int crit_broken_bones_suff;
    unsigned int crit_crushed_skull;
    unsigned int crit_crushed_skull_suff;
    unsigned int crit_crushed_nerve;
    unsigned int crit_crushed_nerve_suff;
    unsigned int crit_voice;
    unsigned int crit_voice_suff;
    unsigned int crit_eye_pop;
    unsigned int crit_eye_pop_suff;
    unsigned int crit_lung_punct;
    unsigned int crit_lung_punct_suff;
    unsigned int crit_impale;
    unsigned int crit_impale_suff;
    unsigned int arena_victs;
    unsigned int arena_loss;
    unsigned int hits[MAX_ATTACK_MODE_TYPE];
    unsigned int swings[MAX_ATTACK_MODE_TYPE];
    unsigned int dam_done[MAX_ATTACK_MODE_TYPE];
    unsigned int dam_received[MAX_ATTACK_MODE_TYPE];
    unsigned int crit_cleave_two;
    unsigned int crit_cleave_two_suff;
    unsigned int crit_disembowel;
    unsigned int crit_disembowel_suff;
    unsigned int crit_eviscerate;
    unsigned int crit_eviscerate_suff;
    unsigned int crit_kidney;
    unsigned int crit_kidney_suff;
    unsigned int crit_genitalia;
    unsigned int crit_genitalia_suff;
    unsigned int crit_tooth;
    unsigned int crit_tooth_suff;
    unsigned int crit_ripped_out_heart;
    unsigned int crit_ripped_out_heart_suff;
    unsigned int skill_success_attempts;
    unsigned int skill_success_pass;
    unsigned int spell_success_attempts;
    unsigned int spell_success_pass;
    unsigned int prayer_success_attempts;
    unsigned int prayer_success_pass;
    unsigned int pets_bought;
    unsigned int pet_levels_bought;
    unsigned int stuck_in_foot;
    unsigned int ounces_of_blood;
    time_t hit_level40;
    time_t hit_level50;

    careerData();
    ~careerData();

    void setToZero() {
      kills = group_kills = 0;
      exp = 0.0;
      deaths = 0;
      crit_kills = crit_misses = crit_hits = 0;
      crit_kills_suff = crit_hits_suff = 0;
      crit_beheads = crit_sev_limbs = crit_cranial_pierce = 0;
      crit_beheads_suff = crit_sev_limbs_suff = crit_cranial_pierce_suff = 0;
      crit_broken_bones = crit_crushed_skull = 0;
      crit_broken_bones_suff = crit_crushed_skull_suff = 0;
      crit_cleave_two = crit_cleave_two_suff = 0;
      crit_disembowel = crit_disembowel_suff = 0;
      crit_crushed_nerve = crit_crushed_nerve_suff = 0;
      crit_voice = crit_voice_suff = 0;
      crit_eye_pop = crit_eye_pop_suff = 0;
      crit_lung_punct = crit_lung_punct_suff = 0;
      crit_impale = crit_impale_suff = 0;
      crit_eviscerate = crit_eviscerate_suff = 0;
      crit_kidney = crit_kidney_suff = 0;
      crit_genitalia = crit_genitalia_suff = 0;
      crit_tooth = crit_tooth_suff =0;
      crit_ripped_out_heart=crit_ripped_out_heart_suff=0;
      arena_victs = arena_loss = 0;
      skill_success_attempts = 0;
      skill_success_pass = 0;
      spell_success_attempts = 0;
      spell_success_pass = 0;
      prayer_success_attempts = 0;
      prayer_success_pass = 0;

      hit_level40 = hit_level50 = 0;

      int i;
      for (i= 0; i < MAX_ATTACK_MODE_TYPE; i++) {
        hits[i] = 0;
        swings[i] = 0;
        dam_done[i] = 0;
        dam_received[i] = 0;
      }
      pets_bought = 0;
      pet_levels_bought = 0;
      stuck_in_foot = 0;
      ounces_of_blood = 0;
      
    }
};

class sessionData
{
  public:
    time_t connect;
    int kills;
    int groupKills;
    double xp;
    double perc;
    byte group_share;
    bool amGroupTank;
    sstring groupName;
    unsigned int hits[MAX_ATTACK_MODE_TYPE];
    unsigned int swings[MAX_ATTACK_MODE_TYPE];
    unsigned int rounds[MAX_ATTACK_MODE_TYPE];
    unsigned int swings_received[MAX_ATTACK_MODE_TYPE];
    unsigned int hits_received[MAX_ATTACK_MODE_TYPE];
    unsigned int rounds_received[MAX_ATTACK_MODE_TYPE];
    unsigned int level_attacked[MAX_ATTACK_MODE_TYPE];
    unsigned int potential_dam_done[MAX_ATTACK_MODE_TYPE];
    unsigned int potential_dam_received[MAX_ATTACK_MODE_TYPE];
    unsigned int combat_dam_done[MAX_ATTACK_MODE_TYPE];
    unsigned int combat_dam_received[MAX_ATTACK_MODE_TYPE];
    unsigned int skill_dam_done[MAX_ATTACK_MODE_TYPE];
    unsigned int skill_dam_received[MAX_ATTACK_MODE_TYPE];
    int mod_done[MAX_ATTACK_MODE_TYPE];
    int mod_received[MAX_ATTACK_MODE_TYPE];
    unsigned int skill_success_attempts;
    unsigned int skill_success_pass;
    unsigned int spell_success_attempts;
    unsigned int spell_success_pass;
    unsigned int prayer_success_attempts;
    unsigned int prayer_success_pass;
 
    sessionData();
    ~sessionData();

    void setToZero() {
      connect = time(0);
      kills = 0;
      groupKills = 0;
      xp = 0.0;
      perc = 0.0;
      group_share = 1;
      groupName = "A group of adventurers";
      amGroupTank = false;
      skill_success_attempts = 0;
      skill_success_pass = 0;
      spell_success_attempts = 0;
      spell_success_pass = 0;
      prayer_success_attempts = 0;
      prayer_success_pass = 0;

      attack_mode_t i;
      for (i= ATTACK_NORMAL; i < MAX_ATTACK_MODE_TYPE; i++) {
        hits[i] = 0;
        swings[i] = 0;
        rounds[i] = 0;
        combat_dam_done[i] = 0;
        combat_dam_received[i] = 0;
        potential_dam_done[i] = 0;
        potential_dam_received[i] = 0;
        skill_dam_done[i] = 0;
        skill_dam_received[i] = 0;
        swings_received[i] = 0;
        hits_received[i] = 0;
        rounds_received[i] = 0;
        level_attacked[i] = 0;
        mod_done[i] = 0;
        mod_received[i] = 0;
      }
    }
};

class promptData
{
  public:
    unsigned int type;
    sstring hpColor;
    sstring manaColor;
    sstring moveColor;
    sstring moneyColor;
    sstring expColor;
    sstring oppColor;
    sstring roomColor;
    sstring tankColor;
    sstring pietyColor;
    sstring lifeforceColor;
    sstring prompt;
//    double xptnl[MAX_CLASSES];  getExpClassLevel is same for all classes
    double xptnl;

    promptData();
    ~promptData();
};

class bonusStatPoints {
 public:
  int total;
  int combat;
  int combat2;
  int learn;
  int util;

  bonusStatPoints();
};



// Descriptor class
class Descriptor
{
  private:
    bool host_resolved;           // hostname has been resolved by DNS
  public:
    TSocket *socket;
    editStuff edit;
    sstring host;                 // hostname
    sstring pwd;                 // password                   
    connectStateT connected;                // mode of 'connectedness'    
    int wait;                     // wait for how many loops    
    size_t showstr_head;           // for paging through texts  
    int tot_pages;               // for tracking paged info
    int cur_page;                //       -
    sstring str;                  // for the modify-str system
    int max_str;
    int prompt_mode;              // control of prompt-printing 
    char m_raw[4096];             // buffer for raw input    
    textQ output;                 // q of sstrings to send    
    textQ input;                  // q of unprocessed input  
    sessionData session;          // data for this session
    careerData career;            // data for career
    bonusStatPoints bonus_points;
    drugData drugs[MAX_DRUG];
    unsigned int autobits;
    unsigned int best_rent_credit;
    int playerID;
    char last_teller[128];
    TBeing *character;            // linked to char (might be a poly)
    TAccount *account;            // linked to account
    TPerson *original;            // original char (always a person)
    snoopData snoop;              // to snoop people           
    Descriptor *next;             // link to next descriptor    
    char *pagedfile;              // what file is getting paged 
    char name[20];                // dummy field (idea, bug, mail use it)
    TObj *obj;                    // for object editor
    TMonster *mob;                // for monster editor 
    aliasData alias[16];          // aliases for players
    char history[10][MAX_INPUT_LENGTH];
    betData bet;
    cBetData bet_opt;
    byte screen_size;
    byte point_roll;
    time_t talkCount;
    bool m_bIsClient;
    sh_int bad_login;              // login catches for hackers 
    int severity;
    int office;
    int blockastart;
    int blockaend;
    int blockbstart;
    int blockbend;
    lastChangeData last;
    ubyte deckSize;
    char delname[20];
    promptData prompt_d;
    unsigned long plr_act;
    unsigned int plr_color;
    colorSubT plr_colorSub;
    unsigned int plr_colorOff;

    // Functions
  private:
    Descriptor();  // prevent default constructor from being used
  public:
    Descriptor(TSocket *);
    Descriptor(const Descriptor &);
    Descriptor & operator=(const Descriptor &a);
    ~Descriptor();

    int outputProcessing();
    int inputProcessing();
    void flush();
    void flushInput();
    int sendLogin(const sstring &);
    bool checkForMultiplay();
    bool checkForAccount(sstring &, bool silent = FALSE);
    bool checkForCharacter(sstring &);
    int doAccountStuff(sstring &);
    int clientCreateAccount(sstring &);
    int clientCreateChar(sstring &);
    bool isEditing();
    void Edit(sstring &);
    void deleteAccount();
    void menuWho();
    void saveAccount();
    int doAccountMenu(const sstring &);
    void add_to_history_list(const sstring &);
    int getFreeStat(connectStateT);
    int nanny(const sstring &);
    void sendMotd(int);
    void go_back_menu(connectStateT);
    void EchoOn();
    void EchoOff();
    void sendHomeList();
    void sendStartStatList();
    void sendDoneScreen();
    const sstring getStatDescription(int);
    void sendStatList(int, int);
    void sendStatRules(int);
    void sendRaceList();
    void sendClassList(int);
    void sendTraitsList();
    bool start_page_file(const sstring &, const sstring &);
    bool canChooseClass(int, bool multi = FALSE, bool triple = FALSE);
    int client_nanny(sstring &);
    void writeToQ(const sstring &arg);
    void clientf(const sstring &msg);
    bool page_file(const sstring &);
    void page_string(const sstring &, showNowT = SHOWNOW_NO, allowReplaceT allow = ALLOWREP_NO);
    void show_string(const sstring &, showNowT, allowReplaceT);
    const sstring badClassMessage(int Class, bool multi = FALSE, bool triple = FALSE);
#if 0
    char *badRaceMessage(int race);
#endif
    void send_client_motd();
    void send_client_inventory();
    void send_client_room_people();
    void send_client_room_objects();
    void send_client_prompt(int, int);
    void send_client_exits();
    int read_client(sstring &);
    void sstring_add(sstring &);
    void fdSocketClose(int);
    void saveAll();
    void worldSend(const sstring &, TBeing *);
    void sendShout(TBeing *, const sstring &);
    void updateScreenAnsi(unsigned int update);
    void updateScreenVt100(unsigned int update);
    int move(int, int);
    void send_bug(const sstring &, const sstring &);
    void add_comment(const sstring &, const sstring &);
    void cleanUpStr();
    bool getHostResolved();
    void setHostResolved(bool, const sstring &);
    void beep() {
      writeToQ("");
    }

    const sstring doColorSub() const;
    const sstring ansi_color_bold(const sstring &s) const;
    const sstring ansi_color_bold(const sstring &s, unsigned int) const;
    const sstring ansi_color(const sstring &s) const;
    const sstring ansi_color(const sstring &s, unsigned int) const;
    bool hasColor() const;
    bool hasColorVt() const;
    const sstring highlight(sstring &s) const;
    const sstring whiteBold() const;
    const sstring blackBold() const;
    const sstring redBold() const;
    const sstring underBold() const;
    const sstring blueBold() const;
    const sstring cyanBold() const;
    const sstring greenBold() const;
    const sstring orangeBold() const;
    const sstring purpleBold() const;
    const sstring white() const;
    const sstring black() const;
    const sstring red() const;
    const sstring under() const;
    const sstring bold() const;
    const sstring norm() const;
    const sstring blue() const;
    const sstring cyan() const;
    const sstring green() const;
    const sstring orange() const;
    const sstring purple() const;
    const sstring invert() const;
    const sstring flash() const;
    const sstring BlackOnBlack() const;
    const sstring BlackOnWhite() const;
    const sstring WhiteOnBlue() const;
    const sstring WhiteOnCyan() const;
    const sstring WhiteOnGreen() const;
    const sstring WhiteOnOrange() const;
    const sstring WhiteOnPurple() const;
    const sstring WhiteOnRed() const;
};

#endif
