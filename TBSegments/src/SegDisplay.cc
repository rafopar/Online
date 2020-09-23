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
#include <TMath.h>
#include <TGLabel.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGMsgBox.h>
#include <TVirtualX.h>
#include <TApplication.h>
#include <TGFileDialog.h>


using namespace std;

SegDisplay::SegDisplay(const TGWindow *p, UInt_t w, UInt_t h) {

    p_wind = p;

    fMain = new TGMainFrame(p, w, h, kHorizontalFrame);
    fMain->Connect("CloseWindow()", "SegDisplay", this, "CloseApp()");

    TGHorizontalFrame *hframelow = new TGHorizontalFrame(fMain, 1050, 550, kHorizontalFrame);
    // = new TRootEmbeddedCanvas("Ecanvas", hframelow, 1050, 700);

    vframe = new TGVerticalFrame(fMain, 200, 40);

    but_Open = new TGTextButton(vframe, "&Chose a file");
    but_Open->Connect("Clicked()", "SegDisplay", this, "OpenInpFile()");
    vframe->AddFrame(but_Open, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

    fG_RunControls = new TGGroupFrame(vframe, new TGString("Run Controls"), kVerticalFrame);

    but_next = new TGTextButton(fG_RunControls, "&Next");
    but_next->Connect("Clicked()", "SegDisplay", this, "Next()");
    fG_RunControls->AddFrame(but_next, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));


    TGHorizontalFrame *hframe_Run = new TGHorizontalFrame(fG_RunControls, 30, 10);

    TGTextButton *but_Run = new TGTextButton(hframe_Run, "&Run");
    but_Run->Connect("Clicked()", "SegDisplay", this, "AnalyzeNEvents()");
    hframe_Run->AddFrame(but_Run, new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));


    nmb_nev2Run = new TGNumberEntry(hframe_Run, 20000, 9, -1, TGNumberFormat::EStyle::kNESInteger,
            TGNumberFormat::EAttribute::kNEAPositive);
    hframe_Run->AddFrame(nmb_nev2Run, new TGLayoutHints(kLHintsRight, 1, 1, 1, 1));

    fG_RunControls->AddFrame(hframe_Run);

    vframe->AddFrame(fG_RunControls, new TGLayoutHints(kLHintsCenterX, 1, 1, 1, 1));


    fG_docaControls = new TGGroupFrame(vframe, new TGString("DOCA Controls"), kVerticalFrame);
    vframe->AddFrame(fG_docaControls, new TGLayoutHints(kLHintsCenterX, 1, 1, 1, 1));

    TGTextButton *fBut_ResetDocaPars = new TGTextButton(fG_docaControls, "&Reset Doca Parameters");
    fBut_ResetDocaPars->Connect("Clicked()", "SegDisplay", this, "ResetDoca()");
    fG_docaControls->AddFrame(fBut_ResetDocaPars, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));

    TGHorizontalFrame * fr_TMinMax[nSL];
    TGLabel * lbl_MInMax[nSL];

    for (int iSL = 0; iSL < nSL; iSL++) {
        fr_TMinMax[iSL] = new TGHorizontalFrame(fG_docaControls, 100, 50);
        lbl_MInMax[iSL] = new TGLabel(fr_TMinMax[iSL], Form("SL %d, Tmin, Tmax: min, max", iSL));
        fr_TMinMax[iSL]->AddFrame(lbl_MInMax[iSL], new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));
        sl_TMinMax[iSL] = new TGDoubleHSlider(fr_TMinMax[iSL], 200, kDoubleScaleBoth);
        sl_TMinMax[iSL]->SetRange(0., 1500.);
        sl_TMinMax[iSL]->Connect("PositionChanged()", "SegDisplay", this, "UpdateEvent()");

        fr_TMinMax[iSL]->AddFrame(sl_TMinMax[iSL], new TGLayoutHints(kLHintsRight, 1, 1, 1, 1));
        fG_docaControls->AddFrame(fr_TMinMax[iSL], new TGLayoutHints(kLHintsCenterX, 1, 1, 1, 1));
    }







    /**
     * Button Exit
     */
    but_Exit = new TGTextButton(vframe, "&Exit");
    but_Exit->Connect("Clicked()", "SegDisplay", this, "CloseApp()");
    vframe->AddFrame(but_Exit, new TGLayoutHints(kLHintsCenterX, 1, 1, 1, 1));


    /**
     * Tabs
     */

    fTab = new TGTab(fMain, 1000, 100);
    fTab->Connect("Selected(Int_t)", "SegDisplay", this, "DoTab(Int_t)");

    TGCompositeFrame *tf_DCHits = (TGCompositeFrame*) fTab->AddTab("Segments");

    // A Horizontal frame that will contain canvases for secors 1, 2 and 3
    TGHorizontalFrame *fr_DCHitsSec_123 = new TGHorizontalFrame(tf_DCHits, 30, 10);
    for (int i = 0; i < 3; i++) {
        fEC_DCHits[i] = new TRootEmbeddedCanvas(Form("fEC_DCHits_%d", i), fr_DCHitsSec_123, 340, 340);

        fr_DCHitsSec_123->AddFrame(fEC_DCHits[i], new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));
    }
    tf_DCHits->AddFrame(fr_DCHitsSec_123, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));

    // A Horizontal frame that will contain canvases for secors 1, 2 and 3
    TGHorizontalFrame *fr_DCHitsSec_456 = new TGHorizontalFrame(tf_DCHits, 30, 10);
    for (int i = 3; i < 6; i++) {
        fEC_DCHits[i] = new TRootEmbeddedCanvas(Form("fEC_DCHits_%d", i), fr_DCHitsSec_456, 340, 340);
        fr_DCHitsSec_456->AddFrame(fEC_DCHits[i], new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));
    }
    tf_DCHits->AddFrame(fr_DCHitsSec_456, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));

    //    fEC_DCHits = new TRootEmbeddedCanvas("fEC_DCHits", tf_DCHits, 1050, 700);
    //    tf_DCHits->AddFrame(fEC_DCHits, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));

    TGCompositeFrame *tf_FitQual = (TGCompositeFrame*) fTab->AddTab("Fit quality");

    fEC_Chi2Seg = new TRootEmbeddedCanvas("fEC_Chi2Seg", tf_FitQual, 1050, 700);
    tf_FitQual->AddFrame(fEC_Chi2Seg, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));



    fMain->AddFrame(vframe, new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2));
    fMain->AddFrame(fTab, new TGLayoutHints(kLHintsRight, 2, 2, 2, 2));

    fMain->MapSubwindows();
    fMain->Resize(fMain->GetDefaultSize());
    fMain->MapWindow();


    cout << "**** Costructor Kuku1" << endl;
    InitSettings();
    cout << "**** Costructor Kuku2" << endl;
}

void SegDisplay::InitSettings() {

    cout << "**** InitSettings Kuku1" << endl;

    ReadDocaPars();

    DCConsts = DCConstants();
    fInpFileName = "";
    fHipoDir = "~/";
    IsFileOpened = false;
    IsSingleEvent = false;
    lineRawFit = TLine();
    lineRawFit.SetLineColor(2);
    circRawDoca = TArc();
    circRawDoca.SetLineColor(4);
    circRawDoca.SetFillColor(0);

    for (int i = 0; i < 6; i++) {
        c_DCHits[i] = fEC_DCHits[i]->GetCanvas();
        c_DCHits[i]->SetTopMargin(0);
        c_DCHits[i]->SetBottomMargin(0);
        c_DCHits[i]->SetLeftMargin(0);
        c_DCHits[i]->SetRightMargin(0);
        c_DCHits[i]->Range(DCConsts.xMin, DCConsts.yMin, DCConsts.xMax, DCConsts.yMax);
        c_DCHits[i]->FeedbackMode(kTRUE);
        c_DCHits[i]->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)", "SegDisplay", this,
                "MouseAction(Int_t, Int_t, Int_t, TObject*)");
        //   c_DCHits->Divide(3, 2, 0., 0.);
    }

    c_Chi2Seg = fEC_Chi2Seg->GetCanvas();
    c_Chi2Seg->Divide(3, 2);

    cout << "**** InitSettings Kuku2" << endl;

    /**
     * Chi2 bins are not uniform
     */
    for (int i = 0; i < nChi2Bins + 1; i++) {
        chi2Bins[i] = TMath::Power(10, double(i) / 15.);
    }

    for (int isec = 0; isec < DCConstants::nSect; isec++) {
        //        h_DC_Segments_[isec] = new TH2F(Form("h_DC_Segments_%d", isec), "", 100, -200., 220., 100, 200., 550.);
        //        h_DC_Segments_[isec]->SetStats(0);

        c_Chi2Seg->cd(isec + 1)->SetLogx();
        h_Chi2_SegFit_[isec] = new TH1D(Form("h_Chi2_SegFit_%d", isec), "", nChi2Bins, chi2Bins);
    }
    cout << "**** InitSettings Kuku3" << endl;


    for (int iSL = 0; iSL < nSL; iSL++) {
        sl_TMinMax[iSL]->SetPosition(tMin[iSL], tMax[iSL]);
    }

}

void SegDisplay::ReadDocaPars() {
    onlineDir = std::getenv("ONLINE_DIR");
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

            sl_TMinMax[SL]->SetPosition(tMin[SL], tMax[SL]);
        }

    } else {
        cout << "Can not open the file " << fnameDocaPars << endl;
        cout << "exiting" << endl;
        exit(1);
    }

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

    //    for (int iSec = 0; iSec < nSec; iSec++) {
    //        delete h_DC_Segments_[iSec];
    //    }
    gApplication->Terminate(0);
}

void SegDisplay::Next() {
    for (int isec = 0; isec < DCConstants::nSect; isec++) {
        c_DCHits[isec]->Clear();
        c_DCHits[isec]->cd();
        c_DCHits[isec]->Range(DCConsts.xMin, DCConsts.yMin, DCConsts.xMax, DCConsts.yMax);
        //        h_DC_Segments_[isec]->Reset();
        //        h_DC_Segments_[isec]->GetYaxis()->UnZoom();
        //        h_DC_Segments_[isec]->GetXaxis()->UnZoom();
        //        h_DC_Segments_[isec]->Draw();
    }
    IsSingleEvent = true;
    RunEvents(1);

    for (int isec = 0; isec < DCConstants::nSect; isec++) {
        c_DCHits[isec]->Modified();
        c_DCHits[isec]->Update();
    }

}

void SegDisplay::AnalyzeNEvents() {

    fNev = (Int_t) nmb_nev2Run->GetNumber();

    if (fNev < 0) {
        popupMSG("Number of events to run should be a positive number");
    }
    if (fNev == 1) {
        IsSingleEvent = true;
    } else {
        IsSingleEvent = false;
    }


    for (int isec = 0; isec < DCConstants::nSect; isec++) {
        h_Chi2_SegFit_[isec]->Reset();
    }

    RunEvents(fNev);

    for (int isec = 0; isec < DCConstants::nSect; isec++) {
        c_Chi2Seg->cd(isec + 1);
        ConvertDifferential(h_Chi2_SegFit_[isec]);
        h_Chi2_SegFit_[isec]->Draw();
    }
    c_Chi2Seg->Modified();
    c_Chi2Seg->Update();
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

        double r = TMath::Max((tdc - tMin[SL]), float(0.)) * DMax[SL] / tMax[SL];
        double x = TMath::Min(float(r), float(DMax[SL])) / DMax[SL];
        double err_r = CalcDocaError(x) * DMax[SL];
        //double err_r = 0.15 * r;

        if (IsSingleEvent) {
            c_DCHits[sec]->cd();
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

                if (IsSingleEvent) {
                    if (chi2 > 500000) {
                        c_DCHits[isec]->cd();
                        lineRawFit.DrawLine(x_0, slope * x_0 + offset, x_max, slope * x_max + offset);
                    }
                } else {
                    h_Chi2_SegFit_[isec]->Fill(chi2);
                }
            }

        }

    }


}

double SegDisplay::CalcDocaError(double x) {
    // x is doca/d_Max

    return 0.06 - 0.14 * TMath::Power(x, 1.5) + 0.18 * TMath::Power(x, 2.5);
}

void SegDisplay::popupMSG(std::string a) {
    new TGMsgBox(gClient->GetRoot(), fMain, "Message", a.c_str(), kMBIconExclamation);
}

void SegDisplay::DoTab(Int_t tab) {
    cout << "Changed to the Tab " << tab << endl;
}

void SegDisplay::ConvertDifferential(TH1* h) {
    for (int i = 0; i < h->GetNbinsX(); i++) {
        h->SetBinContent(i + 1, h->GetBinContent(i + 1) / h->GetBinWidth(i + 1));
    }
}

void SegDisplay::UpdateEvent() {

    if (!IsFileOpened) {
        return;
    }

    // Loop over all SLs, to updated DOCD related parameters tMin, tMax and Dmax
    for (int iSL = 0; iSL < nSL; iSL++) {
        sl_TMinMax[iSL]->GetPosition(&tMin[iSL], &tMax[iSL]);
    }

    for (int isec = 0; isec < DCConstants::nSect; isec++) {
        c_DCHits[isec]->cd()->Clear();
        //        c_DCHits[isec]->Range(DCConsts.xMin, DCConsts.yMin, DCConsts.xMax, DCConsts.yMax);
        //        h_DC_Segments_[isec]->Reset();
        //        h_DC_Segments_[isec]->Draw();
    }

    IsSingleEvent = true;
    ProcessDCSeg(fEvent);

    for (int isec = 0; isec < DCConstants::nSect; isec++) {
        c_DCHits[isec]->Modified();
        c_DCHits[isec]->Update();
    }

}

void SegDisplay::MouseAction(Int_t ev, Int_t ix, Int_t iy, TObject* selected) {

    if (strcmp("TCanvas", selected->ClassName()) != 0) {
        return;
    }

    int iXMax = ((TCanvas*) selected)->GetWw();
    int iYMax = ((TCanvas*) selected)->GetWw();


    ((TCanvas*) selected)->cd();
    double yNewMax;
    double yNewMin;

    double xNewMax;
    double xNewMin;
    
    double delta_X, delta_Y;
    
    switch (ev) {

        case kMouseEnter:
            return;
        case kButton1Down:

            ixClick = ix;
            iyClick = iy;
            break;
        case kButton1Motion:

            //          cout << "Detected Mouse Motion      ix = " << ix << "    iy = " << iy << endl;
            gVirtualX->DrawBox(ixClick, iyClick, TMath::Min(ix, iXMax), TMath::Min(iy, iYMax), TVirtualX::kHollow);
            //gVirtualX->DrawLine(ixClick, iyClick, TMath::Min(ix, iXMax), TMath::Min(iy, iYMax));

            break;
        case kButton1Up:
            cout << "Event is " << ev << endl;
            //            cout<<"kButton1Up is detected"<<endl;
            ixRelease = TMath::Max(ix, 0);
            ixRelease = TMath::Min(ixRelease, iXMax);
            iyRelease = TMath::Max(iy, 0);
            iyRelease = TMath::Min(iyRelease, iYMax);

            yNewMax = DCConsts.yMax - (DCConsts.yMax - DCConsts.yMin) * double(TMath::Min(iyClick, iyRelease)) / double(iYMax);
            yNewMin = DCConsts.yMax - (DCConsts.yMax - DCConsts.yMin) * double(TMath::Max(iyClick, iyRelease)) / double(iYMax);

            xNewMax = DCConsts.xMin + (DCConsts.xMax - DCConsts.xMin) * double(TMath::Max(ixClick, ixRelease)) / double(iXMax);
            xNewMin = DCConsts.xMin + (DCConsts.xMax - DCConsts.xMin) * double(TMath::Min(ixClick, ixRelease)) / double(iXMax);

            delta_Y = yNewMax - yNewMin;
            delta_X = xNewMax - xNewMin;
            
            // We need to have a square zoom, that circles will appear as circles
            
            if( delta_X < delta_Y ){
                if ( ixClick < iyRelease ){
                    xNewMax = TMath::Min(DCConsts.xMax, xNewMin + delta_Y);
                    xNewMin = xNewMax - delta_Y;
                }else{
                    xNewMin = TMath::Max(DCConsts.xMin, xNewMax - delta_Y);
                    xNewMax = xNewMin + delta_Y;
                }
            }else {
                if (iyClick < iyRelease) {
                    yNewMin = TMath::Max(DCConsts.yMin, yNewMax - delta_X);
                    yNewMax = yNewMin + delta_X;
                } else {
                    yNewMax = TMath::Min(DCConsts.yMax, yNewMin + delta_X);
                    yNewMin = yNewMax - delta_X;
                }
            }
            
            
            
            ((TCanvas*) selected)->Range(xNewMin, yNewMin, xNewMax, yNewMax);
            ((TCanvas*) selected)->Modified();
            ((TCanvas*) selected)->Update();
            break;
        default:
            return;


    }

}

void SegDisplay::ResetDoca() {
    // Read the parameters from the file and assign o tMin, tMax and dMax
    ReadDocaPars();
    // Update the viewer
    UpdateEvent();
}

SegDisplay::SegDisplay(const SegDisplay & orig) {
}

SegDisplay::~SegDisplay() {
}

