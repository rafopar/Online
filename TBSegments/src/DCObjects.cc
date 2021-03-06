#include <DCObjects.h>
#include <iostream>
#include <fstream>
#include <string>

#include <TMath.h>
#include <DCObjects.h>

using namespace std;

DCConstants::DCConstants() {

    Init();

}

double DCConstants::w_midpoint_y[nLayerperSec][nWirePerLayer] = {0};
double DCConstants::w_midpoint_x[nLayerperSec][nWirePerLayer] = {0};

const double DCConstants::yMax = 600.;
const double DCConstants::yMin = 180.;
const double DCConstants::xMax = 220.;
const double DCConstants::xMin = -200.;

void DCConstants::Init() {


    //    const double DCConstants::DMax[nSL] = {0.78, 0.81, 1.14, 1.32, 1.87, 1.96};
    //    tMin = {125., 123., 120., 120., 120., 140.};
    //    tMax = {295., 293., 620., 620., 800., 850.};

    wireMidpointFileName = "TBSegments/DataFiles/wires_midpoint.txt";
    onlineDir = std::getenv("ONLINE_DIR");
    std::string fnamewireMidpoints = onlineDir + "/" + wireMidpointFileName;

    ifstream inp(fnamewireMidpoints);
    cout << "** ************* Reading wire midpoint coordinates *********** " << endl;
    cout << "**  The file path is " << fnamewireMidpoints << endl;
    cout << endl;

    if (inp.is_open()) {
        cout << "**  The file successfully opened" << endl;
        cout << endl;

        for (int ilayer = 0; ilayer < nLayerperSec; ilayer++) {
            for (int iw = 0; iw < nWirePerLayer; iw++) {
                inp >> DCConstants::w_midpoint_y[ilayer][iw];
                inp >> DCConstants::w_midpoint_x[ilayer][iw];

                //cout<<w_midpoint_x[ilayer][iw]<<"   "<<w_midpoint_y[ilayer][iw]<<endl;
            }
        }

    } else {
        cout << "Can not open the file " << fnamewireMidpoints << endl;
        cout << "exiting" << endl;
        exit(1);
    }


    /**
     * Reading DOCA parameters, Columns of the txt file is as follows:
     * SL(SuperLayer)   Tmin    Tmax    Dmax
     */
    docaParFilename = "TBSegments/DataFiles/doca_pars.txt";
    std::string fnameDocaPars = onlineDir + "/" + docaParFilename;

    ifstream inpDocaPars(fnameDocaPars);
    cout << "** ************* Reading DOCA Paramaters: Tmin, Tmax and D0 *********** " << endl;
    cout << "**  The file path is " << fnameDocaPars << endl;
    cout << endl;

    if (inpDocaPars.is_open()) {
        cout << "**  The file successfully opened" << endl;
        cout << endl;

        while (!inpDocaPars.eof()) {
            int SL;
            inpDocaPars >> SL;
            inpDocaPars >> tMin[SL];
            inpDocaPars >> tMax[SL];
            inpDocaPars >> DMax[SL];
        }

    } else {
        cout << "Can not open the file " << fnamewireMidpoints << endl;
        cout << "exiting" << endl;
        exit(1);
    }


}

/**
 * CLASS DCHit
 */


DCHit::DCHit(int asec, int alayer, int awireno, double ax, double ay, double ar, double aerr_r) :
sector(asec), layer(alayer), wireNo(awireno), x(ax), y(ay), r(ar), err_r(aerr_r) {
}

DCHit::DCHit(const DCHit& rvalue) : sector(rvalue.sector), layer(rvalue.layer), wireNo(rvalue.wireNo), x(rvalue.x), y(rvalue.y), r(rvalue.r), err_r(rvalue.err_r) {

}

bool DCHit::operator<(const DCHit &arg) const {

    /**
     * This assumes sector and layer are the same.
     * By design only hits from the same sector/layer should be in the same set
     */
    return (this->wireNo < arg.wireNo);
}

bool DCHit::operator==(const DCHit &arg) const {

    /**
     * This assumes sector and layer are the same.
     * By design only hits from the same sector/layer should be in the same set
     */
    return (this->wireNo == arg.wireNo /*&& this->layer == arg.layer */);
}

/**
 *
 * CLASS SegFinder
 */


// ========================== SegFinder ==========================

const int SegFinder::nLayers = 36;
const int SegFinder::nSect = 6;
const int SegFinder::nlayerPerSL = 6;
const int SegFinder::nSL = 6;
const int SegFinder::nMinHits = 5;
const int SegFinder::nMaxHits = 12;
const int SegFinder::nMinLayers = 5;

SegFinder::SegFinder(std::set<DCHit> *ahits) {
    hits = ahits;


    FindSegmentCandidates();
}

void SegFinder::FindSegmentCandidates() {

    v_SegCandidates.clear();
    v_SegCandidates.shrink_to_fit();

    for (int SL = 0; SL < nSL; SL++) {

        std::set<DCHit> *localhits = &hits[nlayerPerSL * SL];

        //for (DCHit curhit : localhits[0]) {

        for (std::set<DCHit>::iterator it = localhits[0].begin(); it != localhits[0].end(); it++) {

            DCHit curHit = *it; // -> Can make this as a pointer? <<<<<<<<<<<<<<<<<<<<<<<<========================

            int iw = curHit.wireNo;

            vector<DCHit> v_segm;
            v_segm.push_back(curHit);
            int curminwireNo = iw;
            int curmaxwireNo = iw;

            //            cout<<"curHit.wireNo is "<<curHit.wireNo<<endl;

            if (localhits[0].find(DCHit(0, 0., curHit.wireNo + 1, 0., 0., 0., 0.)) != localhits[0].end()) {

                curmaxwireNo = curHit.wireNo + 1;
                std::advance(it, 1);

                v_segm.push_back(*(localhits[0].find(DCHit(0, 0., curHit.wireNo + 1, 0., 0., 0., 0.))));
            }

            /**
             * So current algorithm will keep only segment candidates which have a hit in the 1st later of each SL
             * Later this should be changed of course.
             */

            int nLayer = 1; // This variable shows the number of layers of the segment
            for (int ilayer = 1; ilayer < nlayerPerSL; ilayer++) {

                /**
                 * Any hit with a wire number between curminwireNo -1 and curmaxwireNo + 1 will be added into this segment
                 * This is also is not a perfect algorithm, as for example in a given layer a good segment
                 * can have only 2 adjusent hits.
                 */

                bool hitFoundTHisLayer = false;
                bool minWireFound = false;
                int tmpCurMaswireNo = curmaxwireNo;
                for (int iwire = curminwireNo - 2; iwire < curmaxwireNo + 3; iwire++) {

                    if (localhits[ilayer].find(DCHit(0, 0, iwire, 0., 0., 0., 0.)) != localhits[ilayer].end()) {
                        //if ( std::find_if( localhits[ilayer].begin(), localhits[ilayer].end(), [&](const DCHit curHit ) { /*cout<<"wireNo = "<<curHit.wireNo<<"   iwire = "<<iwire<<"  Match = "<<(curHit == DCHit( 0, 0, iwire, 0., 0., 0., 0. ))<<endl;*/ return curHit == DCHit( 0, 0, iwire, 0., 0., 0., 0. ); } ) != localhits[ilayer].end() ) {

                        v_segm.push_back(*(localhits[ilayer].find(DCHit(0, 0, iwire, 0., 0., 0., 0.))));
                        hitFoundTHisLayer = true;

                        tmpCurMaswireNo = iwire;

                        if (minWireFound = false) {
                            curminwireNo = iwire;
                            minWireFound = true;
                        }

                    }

                }
                curmaxwireNo = tmpCurMaswireNo;

                if (hitFoundTHisLayer) {
                    nLayer = nLayer + 1;
                }
            }


            if (nLayer >= nMinLayers && v_segm.size() < nMaxHits) {
                v_SegCandidates.push_back(v_segm);
            }



        }


    }
}

/**
 * Class SegFitter
 */

SegFitter::SegFitter() {
}

SegFitter::SegFitter(SegFitter &arg) {
    this->fChi2 = arg.GetFitChi2();
    this->offset = arg.GetOffset();
    this->slope = arg.GetSlope();
}

SegFitter::SegFitter(vector<DCHit> hits) {

    /**
     * First we want to rotate points
     */

    int nHits = hits.size();

    double alpha0 = TMath::Pi() / 2. - atan2(hits.at(nHits - 1).y - hits.at(0).y, hits.at(nHits - 1).x - hits.at(0).x);

    const double deltaAlpha = 35 * TMath::DegToRad();
    const int nAlpha = 50;
    const double dAlpha = (2 * deltaAlpha + 1) / double(nAlpha);

    double chi2Min = 1.e15;
    double x0_Optim;
    double alphaOptim;

    for (double alpha = alpha0 - deltaAlpha; alpha <= alpha0 + deltaAlpha; alpha = alpha + dAlpha) {


        vector<DCHit> v_rotatedHits = GetRotatedHits(hits, alpha);

        double chi2 = 0;

        //double x0 = FitCircles(v_rotatedHits, chi2);
        double x0 = FitDotCombinatorics(v_rotatedHits, chi2);

        if (chi2 < chi2Min) {
            chi2Min = chi2;
            x0_Optim = x0;
            alphaOptim = alpha;
        }

    }

    offset = -x0_Optim / cos((TMath::Pi() / 2. - alphaOptim));
    slope = TMath::Tan(TMath::Pi() / 2. - alphaOptim);
    fChi2 = chi2Min;

}

vector<DCHit> SegFitter::GetRotatedHits(const vector<DCHit> inpHits, double alpha) {

    vector<DCHit> v_rotatedHits;

    for (DCHit curHit : inpHits) {

        DCHit rotatedHit(curHit);

        rotatedHit.x = curHit.x * cos(alpha) - curHit.y * sin(alpha);
        rotatedHit.y = curHit.x * sin(alpha) + curHit.y * cos(alpha);
        v_rotatedHits.push_back(rotatedHit);
    }

    return v_rotatedHits;

}

double SegFitter::CalcDocaError(double x) {
    // x is doca/d_Max

    return 0.06 - 0.14 * TMath::Power(x, 1.5) + 0.18 * TMath::Power(x, 2.5);
}

double SegFitter::FitCircles(const vector<DCHit> v_inp, double &chi2) {

    double A = 0, B = 0, C = 0, D = 0;
    chi2 = 0;

    for (int i = 0; i < v_inp.size(); i++) {

        DCHit curHit = v_inp.at(i);

        A = A + 1. / TMath::Power(curHit.err_r, 4);
        B = B + (-3.) * curHit.x / TMath::Power(curHit.err_r, 4);
        C = C + (3 * curHit.x * curHit.x - curHit.r * curHit.r) / TMath::Power(curHit.err_r, 4);
        D = D + (curHit.r * curHit.r * curHit.x - curHit.x * curHit.x * curHit.x) / TMath::Power(curHit.err_r, 4);
    }

    double coef[4] = {D, C, B, A};

    double a, b, c;

    TMath::RootsCubic(coef, a, b, c);

    //cout << "************************* Start of Chi2 Calculation ****************************" << endl;

    for (int i = 0; i < v_inp.size(); i++) {

        chi2 = chi2 + TMath::Power((a - v_inp.at(i).x)*(a - v_inp.at(i).x) - v_inp.at(i).r * v_inp.at(i).r, 2) / TMath::Power(v_inp.at(i).err_r, 4);
        //        cout << "a = " << a << "    x = " << v_inp.at(i).x<<"   r = " << v_inp.at(i).r << "    Err_r = " << v_inp.at(i).err_r << "    chi2 Point = "
        //                << TMath::Power((a - v_inp.at(i).x)*(a - v_inp.at(i).x) - v_inp.at(i).r * v_inp.at(i).r, 2) / TMath::Power(v_inp.at(i).err_r, 4) << "    chi2 Tot = " << chi2 << endl;
    }
    //    cout << "************************* End of of Chi2 Calculation ****************************" << endl;

    return a;


}

double SegFitter::FitDotCombinatorics(const vector<DCHit> v_inp, double &chi2Min) {


    // number of hits
    int n = v_inp.size();

    // to keep track of next element in each of
    // the n arrays
    int* indices = new int[n];

    // initialize with first element's index
    for (int i = 0; i < n; i++) {
        indices[i] = 0;
    }


    chi2Min = 1.e15;
    double x0OfChi2Min = 1000.;

    // The looping algorithm is copied from https://www.geeksforgeeks.org/combinations-from-n-arrays-picking-one-element-from-each-array/
    while (1) {

        double A = 0; // A = Sum x_i^{2}/sigm_i^{2};
        double B = 0; // B = Sum 1./sigm_i^{2};

        vector<double> hit_X;
        vector<double> hit_Y;
        vector<double> hit_err_X;

        for (int i = 0; i < n; i++) {
            hit_X.push_back(v_inp.at(i).x + v_inp.at(i).r * (2 * indices[i] - 1));
            hit_Y.push_back(v_inp.at(i).y);
            hit_err_X.push_back(v_inp.at(i).err_r);

            A = A + hit_X.at(i) / (hit_err_X.at(i) * hit_err_X.at(i));
            B = B + 1. / (hit_err_X.at(i) * hit_err_X.at(i));

        }

        double curX0 = A / B;
        double curChi2 = 0;

        for (int ii = 0; ii < hit_X.size(); ii++) {
            curChi2 = curChi2 + (hit_X.at(ii) - curX0)* (hit_X.at(ii) - curX0) / (hit_err_X.at(ii) * hit_err_X.at(ii));
        }

        if (curChi2 < chi2Min) {
            chi2Min = curChi2;
            x0OfChi2Min = curX0;
        }


        int next = n - 1;

        while (next >= 0 && (indices[next] + 1 >= 2)) {
            next--;
        }

        if (next < 0) {
            return x0OfChi2Min;
        }

        indices[next]++;

        for (int i = next + 1; i < n; i++) {
            indices[i] = 0;
        }

    }

}