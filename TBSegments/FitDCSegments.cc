/* 
 * File:   FitDCSegments.cc
 * Author: rafopar
 *
 * Created on August 19, 2020, 10:13 PM
 */

#include <cstdlib>
#include <iostream>
#include <DCObjects.h>
#include <SegDisplay.h>

// ===== Root headers =====
#include <TArc.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLine.h>
#include <TFile.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TGClient.h>
#include <TApplication.h>

// ===== Hipo headers =====
#include <reader.h>
#include <dictionary.h>


void RunSegDisplay(){
    new SegDisplay(gClient->GetRoot(), 800, 800);
}

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    
    TApplication displayApp("displayApp", &argc, argv);
    
    RunSegDisplay();
    
    displayApp.Run();
    
    DCConstants dcConsts;

    TCanvas *c1 = new TCanvas("c1", "", 950, 950);
    c1->DrawFrame(-200., 220., 200., 550.);

    TArc circ1;
    circ1.SetLineColor(2);

    TLine *line1 = new TLine();
    line1->SetLineColor(4);

    bool debug = true;

    char inputFile[256];

    cout<<"argc is "<<argc<<endl;
    if (argc > 1) {
        sprintf(inputFile, "%s", argv[1]);
        //sprintf(outputFile,"%s",argv[2]);
    } else {
        std::cout << " *** please provide a file name..." << std::endl;
        exit(0);
    }

    const int detECal = 7;
    const double Eb = 10.6;

    TFile file_out("DCSegFits.root", "Recreate");

    const int nChi2Bins = 100;
    double chi2Bins[nChi2Bins + 1];
    for (int i = 0; i < nChi2Bins + 1; i++) {
        chi2Bins[i] = TMath::Power(10, double(i) / 15.);
    }
    TH1D h_chi2_1("h_chi2_1", "", nChi2Bins, chi2Bins);

    hipo::reader reader;
    reader.open(inputFile);

    hipo::dictionary factory;

    reader.readDictionary(factory);

    factory.show();

    hipo::event event;

    hipo::bank bDCtdc(factory.getSchema("DC::tdc"));

    int MaxEvToProcess = -1;
    int evcounter = 0;

    
    
    while (reader.next() == true) {
        reader.read(event);
        evcounter = evcounter + 1;


        if (evcounter % 5000 == 0) {
            cout.flush() << "Processed " << evcounter << " events \r";

            if (evcounter >= MaxEvToProcess && MaxEvToProcess > 0) {
                break;
            }
        }

        event.getStructure(bDCtdc);

        int nDCHits = bDCtdc.getRows();

        std::set<DCHit> hitSet[DCConstants::nLayerperSec];

        for (int ihit = 0; ihit < nDCHits; ihit++) {

            int sec = bDCtdc.getInt("sector", ihit) - 1;

            if (sec != 0) {
                continue;
            }

            int layer = bDCtdc.getInt("layer", ihit) - 1;
            int SL = layer / DCConstants::nLayerperSL;
            int w = bDCtdc.getInt("component", ihit) - 1;
            int tdc = bDCtdc.getInt("TDC", ihit);

            double w_Xmid = dcConsts.w_midpoint_x[layer][w];
            double w_Ymid = dcConsts.w_midpoint_y[layer][w];

            double r = (tdc - dcConsts.tMin[SL]) * DCConstants::DMax[SL] / DCConstants::tMax[SL];
            double err_r = 0.15 * r;

            hitSet[layer].insert(DCHit(sec, layer, w, w_Xmid, w_Ymid, r, err_r));

            if (debug) {
                cout << w_Xmid << "   " << w_Ymid << endl;

                if (sec == 0) {
                    circ1.DrawArc(w_Xmid, w_Ymid, r);
                }
            }
        }

        SegFinder segFinder(hitSet);

        vector< vector<DCHit> > v_segments = segFinder.GetAllSegmentCandidates();


        for (vector<DCHit> curSegm : v_segments) {

            if (curSegm.size() >= 5 && curSegm.size() < 12) {
                SegFitter segFitter(curSegm);

                double chi2 = segFitter.GetFitChi2();
                h_chi2_1.Fill(chi2);

                if (debug) {
                    cout << "chi2 = " << chi2 << endl;
                    double slope = segFitter.GetSlope();
                    double offset = segFitter.GetOffset();

                    double x_0 = curSegm.at(0).x < curSegm.at(curSegm.size() - 1).x ? curSegm.at(0).x - curSegm.at(0).r : curSegm.at(curSegm.size() - 1).x - curSegm.at(curSegm.size() - 1).r;
                    double x_max = curSegm.at(0).x > curSegm.at(curSegm.size() - 1).x ? curSegm.at(0).x + curSegm.at(0).r : curSegm.at(curSegm.size() - 1).x + curSegm.at(curSegm.size() - 1).r;


                    if ( chi2 > 10000 ) {
                        line1->DrawLine(x_0, slope * x_0 + offset, x_max, slope * x_max + offset);
                    }
                }
            }

        }

        if (debug) {

            c1->Print("tmp.pdf");
            cin.ignore();
            c1->Clear();
            c1->DrawFrame(-200., 220., 200., 550.);
        }
    }

    for (int i = 0; i < nChi2Bins; i++) {
        h_chi2_1.SetBinContent(i + 1, h_chi2_1.GetBinContent(i + 1) / h_chi2_1.GetBinWidth(i + 1));
    }

    gDirectory->Write();
    return 0;
}