#ifndef __SHOPOWNED_H
#define __SHOPOWNED_H

class TShopOwned {
  int shop_nr;
  TMonster *keeper;
  TBeing *ch;
  bool owned;
  int access;

 public:
  bool isOwned();
  bool hasAccess(int);
  int getPurchasePrice(int, int);
  int getCorpID();

  void doBuyTransaction(int, const sstring &, const sstring &, TObj *obj=NULL);

  void setDividend(sstring);
  double getDividend();
  void doDividend(int, const sstring &);

  void setReserve(sstring);
  int getMinReserve();
  int getMaxReserve();
  void doReserve();
  void chargeTax(int, const sstring &, TObj *);

  // repair specific
  double getQuality();
  void setQuality(sstring);
  double getSpeed();
  void setSpeed(sstring);

  int getMaxNum(const TObj *);
  void showInfo();
  int setRates(sstring);
  int buyShop(sstring);
  int sellShop();
  int giveMoney(sstring);
  int setAccess(sstring);
  int doLogs(sstring);
  int setString(sstring);

  TShopOwned(int, TMonster *, TBeing *);
  ~TShopOwned();
};


#endif