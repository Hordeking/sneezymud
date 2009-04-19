#ifndef __PROCESS_H
#define __PROCESS_H

#include "sstring.h"

// This is the template of a process.
class TProcess {
 public:
  int trigger_pulse;
  sstring name;

  // in general, you shouldn't have to override should_run()
  virtual bool should_run(int) const;

  virtual void run(int) const = 0;

  virtual ~TProcess(){}
};


//// processes
class procWeightVolumeFumble : public TProcess {
 public:
  void run(int) const;
  procWeightVolumeFumble(const int &);
};

class procFactoryProduction : public TProcess {
 public:
  void run(int) const;
  procFactoryProduction(const int &);
};

class procRecordCommodPrices : public TProcess {
 public:
  void run(int) const;
  procRecordCommodPrices(const int &);
};

class procCloseAccountingBooks : public TProcess {
 public:
  void run(int) const;
  procCloseAccountingBooks(const int &);
};

class procCheckTriggerUsers : public TProcess {
 public:
  void run(int) const;
  procCheckTriggerUsers(const int &);
};

class procTrophyDecay : public TProcess {
 public:
  void run(int) const;
  procTrophyDecay(const int &);
};

class procSeedRandom : public TProcess {
 public:
  void run(int) const;
  procSeedRandom(const int &);
};

class procTweakLoadRate : public TProcess {
 public:
  void run(int) const;
  procTweakLoadRate(const int &);
};

class procGlobalRoomStuff : public TProcess {
 public:
  void run(int) const;
  procGlobalRoomStuff(const int &);
};

class procQueryQueue : public TProcess {
 public:
  void run(int) const;
  procQueryQueue(const int &);
};

class procDoPlayerSaves : public TProcess {
 public:
  void run(int) const;
  procDoPlayerSaves(const int &);
};

class procDoRoomSaves : public TProcess {
 public:
  void run(int) const;
  procDoRoomSaves(const int &);
};

class procDeityCheck : public TProcess {
 public:
  void run(int) const;
  procDeityCheck(const int &);
};

class procApocCheck : public TProcess {
 public:
  void run(int) const;
  procApocCheck(const int &);
};

class procSaveFactions : public TProcess {
 public:
  void run(int) const;
  procSaveFactions(const int &);
};

class procSaveNewFactions : public TProcess {
 public:
  void run(int) const;
  procSaveNewFactions(const int &);
};

class procWeatherAndTime : public TProcess {
 public:
  void run(int) const;
  procWeatherAndTime(const int &);
};

class procWholistAndUsageLogs : public TProcess {
 public:
  void run(int) const;
  procWholistAndUsageLogs(const int &);
};

class procCheckForRepo : public TProcess {
 public:
  void run(int) const;
  procCheckForRepo(const int &);
};

class procCheckMail : public TProcess {
 public:
  void run(int) const;
  procCheckMail(const int &);
};

class procPerformViolence : public TProcess {
 public:
  void run(int) const;
  procPerformViolence(const int &);
};

class procFishRespawning : public TProcess {
 public:
  void run(int) const;
  procFishRespawning(const int &);
};

class procReforestation : public TProcess {
 public:
  void run(int) const;
  procReforestation(const int &);
};

class procZoneUpdate : public TProcess {
 public:
  void run(int) const;
  procZoneUpdate(const int &);
};

class procLaunchCaravans : public TProcess {
 public:
  void run(int) const;
  procLaunchCaravans(const int &);
};

class procUpdateAvgPlayers : public TProcess {
 public:
  void run(int) const;
  procUpdateAvgPlayers(const int &);
};

class procCheckGoldStats : public TProcess {
 public:
  void run(int) const;
  procCheckGoldStats(const int &);
};

class procAutoTips : public TProcess {
 public:
  void run(int) const;
  procAutoTips(const int &);
};

class procPingData : public TProcess {
 public:
  void run(int) const;
  procPingData(const int &);
};

class procUpdateAuction : public TProcess {
 public:
  void run(int) const;
  procUpdateAuction(const int &);
};

class procBankInterest : public TProcess {
 public:
  void run(int) const;
  procBankInterest(const int &);
};

class procRecalcFactionPower : public TProcess {
 public:
  void run(int) const;
  procRecalcFactionPower(const int &);
};

class procNukeInactiveMobs : public TProcess {
 public:
  void run(int) const;
  procNukeInactiveMobs(const int &);
};

class procUpdateTime : public TProcess {
 public:
  void run(int) const;
  procUpdateTime(const int &);
};

class procSetZoneEmpty : public TProcess {
 public:
  void run(int) const;
  procSetZoneEmpty(const int &);
};

class procCallRoomSpec : public TProcess {
 public:
  void run(int) const;
  procCallRoomSpec(const int &);
};

class procMobHate : public TProcess {
 public:
  void run(int) const;
  procMobHate(const int &);
};

class procDoComponents : public TProcess {
 public:
  void run(int) const;
  procDoComponents(const int &);
};

/////////////////////

class TScheduler {
  std::vector<TProcess *>procs;

 public:
  void add(TProcess *);
  void run(int);
};


#endif