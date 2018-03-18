#ifndef PPPMODEL_H
#define PPPMODEL_H

#include <math.h>
#include <newmat.h>
#include <iostream>
#include "bnctime.h"
#include "t_prn.h"
#include "satObs.h"
#include "bncutils.h"

namespace BNC_PPP {

class t_astro {
 public:
  static ColumnVector Sun(double Mjd_TT);
  static ColumnVector Moon(double Mjd_TT);
  static Matrix rotX(double Angle);
  static Matrix rotY(double Angle);
  static Matrix rotZ(double Angle);

 private:
  static const double RHO_DEG;
  static const double RHO_SEC;
  static const double MJD_J2000;

  static double GMST(double Mjd_UT1);
  static Matrix NutMatrix(double Mjd_TT);
  static Matrix PrecMatrix (double Mjd_1, double Mjd_2);
};

class t_tides {
 public:
  t_tides() {
    _lastMjd = 0.0;
    _rSun    = 0.0;
    _rMoon   = 0.0;
  }
  ~t_tides() {}
  ColumnVector displacement(const bncTime& time, const ColumnVector& xyz);
 private:
  double       _lastMjd;
  ColumnVector _xSun;
  ColumnVector _xMoon;
  double       _rSun;
  double       _rMoon;
};

class t_loading {
 public:
  t_loading(const QString& fileName);
  ~t_loading();
  t_irc   readFile(const QString& fileName);
  void    printAll() const;

 private:
  class t_blqData {
   public:
    t_blqData() {}
    Matrix amplitudes;
    Matrix phases;
  };
  t_blqData*                 newBlqData;
  QMap <QString, t_blqData*> blqMap;

};

class t_windUp {
 public:
  t_windUp();
  ~t_windUp() {};
  double value(const bncTime& etime, const ColumnVector& rRec, t_prn prn,
               const ColumnVector& rSat);
 private:
  double lastEtime[t_prn::MAXPRN+1];
  double sumWind[t_prn::MAXPRN+1];
};

class t_tropo {
 public:
  static double delay_saast(const ColumnVector& xyz, double Ele);
};

class t_iono {
 public:
  t_iono();
  ~t_iono();
  double stec(const t_vTec* vTec, double signalPropagationTime,
      const ColumnVector& rSat, const bncTime& epochTime,
      const ColumnVector& xyzSta);
 private:
  double vtecSingleLayerContribution(const t_vTecLayer& vTecLayer);
  void piercePoint(double layerHeight, double epoch, const double* geocSta,
      double sphEle, double sphAzi);
  double _psiPP;
  double _phiPP;
  double _lambdaPP;
  double _lonS;


};

}

#endif
