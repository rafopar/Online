/* 
 * File:   DCObjects.h
 * Author: rafopar
 *
 * Created on August 19, 2020, 7:41 PM
 */

#ifndef DCOBJECTS_H
#define DCOBJECTS_H

#include <set>
#include <vector>
#include <string>



using namespace std;

class DCConstants {
public:
    DCConstants();

    static const int nSect = 6;
    static const int nSL = 6;
    static const int nLayerperSL = 6;
    static const int nLayerperSec = nSL*nLayerperSL;
    static const int nWirePerLayer = 112;

    static double w_midpoint_x[nLayerperSec][nWirePerLayer];
    static double w_midpoint_y[nLayerperSec][nWirePerLayer];

    double tMin[nSL];
    double tMax[nSL];
    double DMax[nSL];

private:
    void Init();

    std::string wireMidpointFileName;
    std::string onlineDir;
    std::string docaParFilename;
};

/**
 * 
 * CLAS DCHit
 * 
 * @param asec      : Sector
 * @param alayer    : layer
 * @param awireno   : Wire number
 * @param ax        : X coordinate of the wire midpoint
 * @param ay        : Y coordinate of the wire midpoint
 * @param ar        : Radius (doca) in mm
 * @param aerr_r    : Uncertainty of the radius
 */
class DCHit {
public:
    DCHit(int asec, int alayer, int awireno, double ax, double ay, double ar, double aerr_r);

    DCHit(const DCHit&);

    bool operator<(const DCHit&) const;

    unsigned int sector;
    unsigned int layer;
    unsigned int wireNo;
    double x;
    double y;
    double r;
    double err_r;
};

/**
 * CLASS SefFinder
 */


class SegFinder {
public:
    SegFinder(std::set<DCHit> *);

    vector< vector<DCHit> > GetAllSegmentCandidates() const {
        return v_SegCandidates;
    }

private:

    static const int nLayers; // # of layers
    static const int nSect; // # of sectors
    static const int nSL; // # of SuperLayers
    static const int nlayerPerSL; // # of layers per SL
    static const int nMinHits; // # of layers per SL
    static const int nMaxHits; // # of layers per SL
    static const int nMinLayers; // # of layers per SL

    std::set<DCHit> *hits;

    void FindSegmentCandidates();

    vector< vector<DCHit> > v_SegCandidates;

};

/**
 * CLASS SegFitter
 */

class SegFitter {
public:
    SegFitter(vector<DCHit>);

    vector<DCHit> GetRotatedHits(const vector<DCHit>, double);
    double FitCircles(const vector<DCHit> v_inp, double &chi2);

    double GetSlope() const {
        return slope;
    }

    double GetOffset() const {
        return offset;
    }

    double GetFitChi2()const {
        return fChi2;
    }

    double CalcDocaError(double); // Calculates the DOCA error. According to the DC recon code // referred there as Mac's formula

private:
    double slope;
    double offset;
    double fChi2; // Chi2 of the SegmentFit

};


#endif /* DCOBJECTS_H */

