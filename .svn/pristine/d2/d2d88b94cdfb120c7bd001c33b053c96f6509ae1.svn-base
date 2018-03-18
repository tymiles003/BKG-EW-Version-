#ifndef BNCRTNETUPLOADCASTER_H
#define BNCRTNETUPLOADCASTER_H

#include <newmat.h>
#include "bncuploadcaster.h"
#include "bnctime.h"
#include "ephemeris.h"
extern "C" {
#include "clock_orbit_rtcm.h"
}

class bncEphUser;
class bncoutf;
class bncClockRinex;
class bncSP3;

class bncRtnetUploadCaster : public bncUploadCaster {
 Q_OBJECT
 public:
  bncRtnetUploadCaster(const QString& mountpoint,
                  const QString& outHost, int outPort,
                  const QString& ntripVersion,
                  const QString& userName,const QString& password,
                  const QString& crdTrafo, bool  CoM,
                  const QString& sp3FileName,
                  const QString& rnxFileName,
                  int PID, int SID, int IOD, int iRow);
  void decodeRtnetStream(char* buffer, int bufLen);
 protected:
  virtual ~bncRtnetUploadCaster();
 private:
  void processSatellite(const t_eph* eph, int GPSweek,
                        double GPSweeks, const QString& prn,
                        const ColumnVector& rtnAPC,
                        double rtnClk,
                        const ColumnVector& rtnVel,
                        const ColumnVector& rtnCoM,
                        struct ClockOrbit::SatData* sd,
                        QString& outLine);
  void crdTrafo(int GPSWeek, ColumnVector& xyz, double& dc);

  // TODO: the following lines can be deleted if all parameters are updated regarding ITRF2014
  void crdTrafo8(int GPSWeek, ColumnVector& xyz, double& dc);

  int determineUpdateInd(double samplingRate);

  QString        _casterID;
  bncEphUser*    _ephUser;
  QString        _rtnetStreamBuffer;
  QString        _crdTrafo;
  bool           _CoM;
  int            _PID;
  int            _SID;
  int            _IOD;
  int            _samplRtcmClkCorr;
  double         _samplRtcmEphCorr;
  double         _dx;
  double         _dy;
  double         _dz;
  double         _dxr;
  double         _dyr;
  double         _dzr;
  double         _ox;
  double         _oy;
  double         _oz;
  double         _oxr;
  double         _oyr;
  double         _ozr;
  double         _sc;
  double         _scr;
  double         _t0;
  bncClockRinex* _rnx;
  bncSP3*        _sp3;
  QMap<QString, const t_eph*>* _usedEph;
  // TODO: the following lines can be deleted if all parameters are updated regarding ITRF2014
  double         _dx8;
  double         _dy8;
  double         _dz8;
  double         _dxr8;
  double         _dyr8;
  double         _dzr8;
  double         _ox8;
  double         _oy8;
  double         _oz8;
  double         _oxr8;
  double         _oyr8;
  double         _ozr8;
  double         _sc8;
  double         _scr8;
  double         _t08;
};

struct phaseBiasesSat {
  phaseBiasesSat() {
    yawAngle = 0.0;
    yawRate = 0.0;
  }
  double yawAngle;
  double yawRate;
};

struct phaseBiasSignal {
  phaseBiasSignal() {
    bias      = 0.0;
    integerIndicator     = 0;
    wlIndicator          = 0;
    discontinuityCounter = 0;
  }
  QString type;
  double bias;
  unsigned int integerIndicator;
  unsigned int wlIndicator;
  unsigned int discontinuityCounter;
};

#endif
