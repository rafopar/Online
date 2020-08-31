/* 
 * File:   SegDisplay.cc
 * Author: rafopar
 * 
 * Created on August 26, 2020, 8:10 PM
 */

#include <iostream>
#include "SegDisplay.h"
#include <DCObjects.h>

// Root includes
#include <TGFrame.h>
#include <TGButton.h>
#include <TGMsgBox.h>
#include <TApplication.h>
#include <TGFileDialog.h>


using namespace std;

SegDisplay::SegDisplay(const TGWindow *p, UInt_t w, UInt_t h) {

    p_wind = p;

    fMain = new TGMainFrame(p, w, h, kHorizontalFrame);
    fMain->Connect("CloseWindow()", "SegDisplay", this, "CloseApp()");
    fECanvas = new TRootEmbeddedCanvas("Ecanvas", fMain, 1050, 700);
    //fECanvas->Connect("ProcessedEvent(int, int, int, TObject*)", "SegDisplay", this, "MouseZoom(int, int, int, TObject *)");

    vframe = new TGVerticalFrame(fMain, 200, 40);

    but_Open = new TGTextButton(vframe, "&Chose a file");
    but_Open->Connect("Clicked()", "SegDisplay", this, "OpenInpFile()");
    vframe->AddFrame(but_Open, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

    but_next = new TGTextButton(vframe, "&Next");
    but_next->Connect("Clicked()", "SegDisplay", this, "Next()");
    vframe->AddFrame(but_next, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

    but_Exit = new TGTextButton(vframe, "&Exit");
    but_Exit->Connect("Clicked()", "SegDisplay", this, "CloseApp()");
    vframe->AddFrame(but_Exit, new TGLayoutHints(kLHintsCenterX, 1, 1, 1, 1));

    fMain->AddFrame(vframe, new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2));
    fMain->AddFrame(fECanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
    fMain->MapSubwindows();

    fMain->Resize(fMain->GetDefaultSize());

    fMain->MapWindow();


    cout << "**** Costructor Kuku1" << endl;
    InitSettings();
    cout << "**** Costructor Kuku2" << endl;
}

void SegDisplay::InitSettings() {

    cout << "**** InitSettings Kuku1" << endl;
    DCConsts = DCConstants();
    fInpFileName = "";
    fHipoDir = "~/";
    IsFileOpened = false;
    IsSingleEvent = false;
    lineRawFit = TLine();
    lineRawFit.SetLineColor(2);
    circRawDoca = TArc();
    circRawDoca.SetLineColor(4);
    c_DCHits = fECanvas->GetCanvas();
    c_DCHits->Divide(3, 2, 0., 0.);
    cout << "**** InitSettings Kuku2" << endl;

    c_DCHits->FeedbackMode(kTRUE);
    
    TList *l_primitives = c_DCHits->GetListOfPrimitives();
    
    for( int il = 0; il < l_primitives->GetEntries(); il++ ){
        cout<<l_primitives->At(il)->GetName()<<endl;
    }
    
    for (int isec = 0; isec < DCConstants::nSect; isec++) {
        p_c_DCHits_[isec] = (TPad*) c_DCHits->GetPrimitive(Form("Ecanvas_%d", isec +1 ));
        //p_c_DCHits_[isec]->Connect("ProcessedEvent(int, int, int, TObject*)", "SegDisplay", this, "MouseZoom(int, int, int, TObject *)");
        //((TPad*)c_DCHits->cd(isec + 1))->FeedbackMode(kTRUE);
    }
    cout << "**** InitSettings Kuku3" << endl;
}

bool SegDisplay::RunEvents(int nev) {

    if (!IsFileOpened) {
        popupMSG("You need to chose a file first");
        return false;
    }

    for (int i = 0; i < nev; i++) {

        if (i + 1 % 5000 == 0) {
            cout.flush() << "Processed " << i + 1 << " events \r";
        }

        if (fReader.next() == true) {

            fReader.read(fEvent);

            ProcessDCSeg(fEvent);

        } else {
            return false;
        }
    }

    return true;
}

void SegDisplay::CloseApp() {
    cout << " Exiting the SegDisplay. \n Good bye,  Ցտեսություն, До свидания" << endl;
    gApplication->Terminate(0);
}

void SegDisplay::Next() {
    for (int isec = 0; isec < DCConstants::nSect; isec++) {
        c_DCHits->cd(isec + 1)->Clear();
        c_DCHits->cd(isec + 1)->DrawFrame(-200., 220., 200., 550.);
    }
    IsSingleEvent = true;
    RunEvents(1);
    c_DCHits->Modified();
    c_DCHits->Update();
}

void SegDisplay::OpenInpFile() {
    const char *fileypes[] = {"Hipo Files", "*.hipo", 0, 0};

    fInpDataFileInfo.fFileTypes = fileypes;
    fInpDataFileInfo.fIniDir = StrDup(fHipoDir.c_str());

    fDialog = new TGFileDialog(gClient->GetDefaultRoot(), gClient->GetDefaultRoot(), kFDOpen, &fInpDataFileInfo);

    if (fInpDataFileInfo.fFilename) {
        InitFile(fInpDataFileInfo.fFilename);
    }

}

bool SegDisplay::InitFile(std::string fname) {

    fReader.open(fname.c_str());
    fReader.readDictionary(fFactory);
    fFactory.show();
    fBDCtdc = hipo::bank(fFactory.getSchema("DC::tdc"));

    IsFileOpened = true;
    return IsFileOpened;
}

bool SegDisplay::ProcessDCSeg(hipo::event &event) {
    fEvent.getStructure(fBDCtdc);

    int nDCHits = fBDCtdc.getRows();

    std::set<DCHit> hitSet[DCConstants::nSect][DCConstants::nLayerperSec];

    for (int ihit = 0; ihit < nDCHits; ihit++) {
        int sec = fBDCtdc.getInt("sector", ihit) - 1;

        int layer = fBDCtdc.getInt("layer", ihit) - 1;
        int SL = layer / DCConstants::nLayerperSL;
        int w = fBDCtdc.getInt("component", ihit) - 1;
        int tdc = fBDCtdc.getInt("TDC", ihit);

        double w_Xmid = DCConsts.w_midpoint_x[layer][w];
        double w_Ymid = DCConsts.w_midpoint_y[layer][w];

        double r = (tdc - DCConstants::tMin[SL]) * DCConstants::DMax[SL] / DCConstants::tMax[SL];
        double err_r = 0.15 * r;

        if (IsSingleEvent) {
            c_DCHits->cd(sec + 1);
            circRawDoca.DrawArc(w_Xmid, w_Ymid, r);
        }

        hitSet[sec][layer].insert(DCHit(sec, layer, w, w_Xmid, w_Ymid, r, err_r));

    }

    for (int isec = 0; isec < DCConstants::nSect; isec++) {

        SegFinder segFinder(hitSet[isec]);

        vector< vector<DCHit> > v_segments = segFinder.GetAllSegmentCandidates();

        for (vector<DCHit> curSegm : v_segments) {

            if (curSegm.size() >= 5 && curSegm.size() < 12) {
                SegFitter segFitter(curSegm);

                double chi2 = segFitter.GetFitChi2();
                //h_chi2_1.Fill(chi2);

                double slope = segFitter.GetSlope();
                double offset = segFitter.GetOffset();

                double x_0 = curSegm.at(0).x < curSegm.at(curSegm.size() - 1).x ? curSegm.at(0).x - curSegm.at(0).r : curSegm.at(curSegm.size() - 1).x - curSegm.at(curSegm.size() - 1).r;
                double x_max = curSegm.at(0).x > curSegm.at(curSegm.size() - 1).x ? curSegm.at(0).x + curSegm.at(0).r : curSegm.at(curSegm.size() - 1).x + curSegm.at(curSegm.size() - 1).r;


                if (chi2 > 10000) {
                    lineRawFit.DrawLine(x_0, slope * x_0 + offset, x_max, slope * x_max + offset);
                }

            }

        }

    }


}

void SegDisplay::MouseZoom(int event, int ix, int iy, TObject * selected) {


    cout << "Kuku" << endl;
    if (event == kMouseEnter) {
        cout << "Slected is " << selected->GetName() << endl;
        return;
    }

    return;
}

void SegDisplay::popupMSG(std::string a) {
    new TGMsgBox(gClient->GetRoot(), fMain, "Message", a.c_str(), kMBIconExclamation);
}

SegDisplay::SegDisplay(const SegDisplay& orig) {
}

SegDisplay::~SegDisplay() {
}

