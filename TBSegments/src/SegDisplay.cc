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

#include <chrono>

using namespace std;

const double SegDisplay::tiltAngle = 25 * TMath::DegToRad();

SegDisplay::SegDisplay(const TGWindow *p, UInt_t w, UInt_t h) {

    p_wind = p;

    fMain_docaPars = NULL;

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

    TGTextButton *fBut_OpenDocaParWindow = new TGTextButton(fG_docaControls, "&Set detailed Doca Pars");
    fBut_OpenDocaParWindow->Connect("Clicked()", "SegDisplay", this, "SetManualDOCAPars()");
    fG_docaControls->AddFrame(fBut_OpenDocaParWindow, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));

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


    fG_DisplayControls = new TGGroupFrame(vframe, new TGString("Display Controls"), kVerticalFrame);
    vframe->AddFrame(fG_DisplayControls, new TGLayoutHints(kLHintsCenterX, 1, 1, 1, 1));

    TGTextButton *fBut_Unzoom = new TGTextButton(fG_DisplayControls, "&Unzoom all");
    fBut_Unzoom->Connect("Clicked()", "SegDisplay", this, "UnzoomAll()");
    fG_DisplayControls->AddFrame(fBut_Unzoom, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));


    but_rawHits = new TGCheckButton(fG_DisplayControls, "Raw Hits");
    but_rawHits->Connect("Clicked()", "SegDisplay", this, "UpdateEvent()");
    but_rawHits->SetOn(kTRUE);
    fG_DisplayControls->AddFrame(but_rawHits, new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));
    but_rawSegs = new TGCheckButton(fG_DisplayControls, "Raw Segments");
    but_rawSegs->Connect("Clicked()", "SegDisplay", this, "UpdateEvent()");
    but_rawSegs->SetOn(kTRUE);
    fG_DisplayControls->AddFrame(but_rawSegs, new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));
    but_HBHits = new TGCheckButton(fG_DisplayControls, "HB Hits");
    but_HBHits->Connect("Clicked()", "SegDisplay", this, "UpdateEvent()");
    but_HBHits->SetOn(kTRUE);
    fG_DisplayControls->AddFrame(but_HBHits, new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));
    but_TBHits = new TGCheckButton(fG_DisplayControls, "TB Hits");
    but_TBHits->Connect("Clicked()", "SegDisplay", this, "UpdateEvent()");
    but_TBHits->SetOn(kTRUE);
    fG_DisplayControls->AddFrame(but_TBHits, new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));
    but_TBHitsFit = new TGCheckButton(fG_DisplayControls, "TB Hits Fit");
    but_TBHitsFit->Connect("Clicked()", "SegDisplay", this, "UpdateEvent()");
    but_TBHitsFit->SetOn(kTRUE);
    fG_DisplayControls->AddFrame(but_TBHitsFit, new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));
    but_TBSegs = new TGCheckButton(fG_DisplayControls, "TB Segments");
    but_TBSegs->Connect("Clicked()", "SegDisplay", this, "UpdateEvent()");
    but_TBSegs->SetOn(kTRUE);
    fG_DisplayControls->AddFrame(but_TBSegs, new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));


    /**
     * Button Exit
     */
    but_Exit = new TGTextButton(vframe, "&Exit");
    but_Exit->Connect("Clicked()", "SegDisplay", this, "CloseApp()");
    vframe->AddFrame(but_Exit, new TGLayoutHints(kLHintsCenterX, 1, 1, 1, 1));


    /**
     * Tabs
     */

    fTab = new TGTab(fMain, 1400, 100);
    fTab->Connect("Selected(Int_t)", "SegDisplay", this, "DoTab(Int_t)");

    TGCompositeFrame *tf_DCHits = (TGCompositeFrame*) fTab->AddTab("Segments");

    // A Horizontal frame that will contain canvases for secors 1, 2 and 3
    TGHorizontalFrame *fr_DCHitsSec_123 = new TGHorizontalFrame(tf_DCHits, 30, 10);
    for (int i = 0; i < 3; i++) {
        fEC_DCHits[i] = new TRootEmbeddedCanvas(Form("fEC_DCHits_%d", i), fr_DCHitsSec_123, 390, 390);

        fr_DCHitsSec_123->AddFrame(fEC_DCHits[i], new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));
    }
    tf_DCHits->AddFrame(fr_DCHitsSec_123, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));

    // A Horizontal frame that will contain canvases for secors 1, 2 and 3
    TGHorizontalFrame *fr_DCHitsSec_456 = new TGHorizontalFrame(tf_DCHits, 30, 10);
    for (int i = 3; i < 6; i++) {
        fEC_DCHits[i] = new TRootEmbeddedCanvas(Form("fEC_DCHits_%d", i), fr_DCHitsSec_456, 390, 390);
        fr_DCHitsSec_456->AddFrame(fEC_DCHits[i], new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));
    }
    tf_DCHits->AddFrame(fr_DCHitsSec_456, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));

    //    fEC_DCHits = new TRootEmbeddedCanvas("fEC_DCHits", tf_DCHits, 1050, 700);
    //    tf_DCHits->AddFrame(fEC_DCHits, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));

    TGCompositeFrame *tf_FitQual = (TGCompositeFrame*) fTab->AddTab("Fit quality");

    fChi2Tab = new TGTab(tf_FitQual, 1400, 900);
    fChi2Tab->Connect("Selected(Int_t)", "SegDisplay", this, "DoTab(Int_t)");

    tf_FitQual->AddFrame(fChi2Tab, new TGLayoutHints(kLHintsRight, 2, 2, 2, 2));

    TGCompositeFrame *tf_Chi2All = (TGCompositeFrame*) fChi2Tab->AddTab("Chi2 All");

    fEC_Chi2Seg = new TRootEmbeddedCanvas("fEC_Chi2Seg", tf_Chi2All, 1200, 775);
    tf_Chi2All->AddFrame(fEC_Chi2Seg, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));

    TGCompositeFrame *tf_Chi2TBSegMatched = (TGCompositeFrame*) fChi2Tab->AddTab("Chi2 TBSegMatched");
    fEC_Chi2SegTBSegMatch = new TRootEmbeddedCanvas("fEC_Chi2SegTBSegMatch", tf_Chi2TBSegMatched, 1200, 775);
    tf_Chi2TBSegMatched->AddFrame(fEC_Chi2SegTBSegMatch, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));

    TGCompositeFrame *tf_chi2Eff = (TGCompositeFrame*) fChi2Tab->AddTab("Chi2 Cut Efficiency");
    fEC_Chi2CutEff = new TRootEmbeddedCanvas("fEC_Chi2SegTBSegMatch", tf_chi2Eff, 1200, 775);
    tf_chi2Eff->AddFrame(fEC_Chi2CutEff, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
    //c_Chi2CutEff

    TGCompositeFrame *tf_TBHits = (TGCompositeFrame*) fTab->AddTab("TB Hits");
    fEC_TB_Hit_t0 = new TRootEmbeddedCanvas("fEC_TB_Hit_t0", tf_TBHits, 1200, 700);
    tf_TBHits->AddFrame(fEC_TB_Hit_t0, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));

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
    lineRawFit.SetLineColor(1);

    lineTBSeg = TLine();
    lineTBSeg.SetLineColor(4);

    //latChi2Raw = TLatex();
    latChi2Raw.SetTextColor(1);
    latChi2Raw.SetTextFont(42);
    //latChi2Raw.SetNDC();

    circRawDoca = TArc();
    circRawDoca.SetLineColor(1);
    circRawDoca.SetFillStyle(0);

    circRawDocaBest = TArc();
    circRawDocaBest.SetLineColor(3);
    circRawDocaBest.SetLineWidth(2);
    circRawDocaBest.SetFillStyle(0);

    circHBTrkDoca = TArc();
    circHBTrkDoca.SetLineColor(2);
    circHBTrkDoca.SetFillStyle(0);

    circTBDoca = TArc();
    circTBDoca.SetLineColor(6);
    circTBDoca.SetFillStyle(0);

    circTBTrkDoca = TArc();
    circTBTrkDoca.SetLineColor(4);
    circTBTrkDoca.SetFillStyle(0);

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

    c_Chi2SegTBSegMatched = fEC_Chi2SegTBSegMatch->GetCanvas();
    c_Chi2SegTBSegMatched->Divide(3, 2);

    c_Chi2CutEff = fEC_Chi2CutEff->GetCanvas();
    c_Chi2CutEff->Divide(3, 2);

    c_TBT0 = fEC_TB_Hit_t0->GetCanvas();
    c_TBT0->Divide(3, 2);

    cout << "**** InitSettings Kuku2" << endl;

    /**
     * Chi2 bins are not uniform
     */
    for (int i = 0; i < nChi2Bins + 1; i++) {
        chi2Bins[i] = TMath::Power(10, double(i) / 20.) - 1.;
        //chi2Bins[i] = double(i)*1000/double(nChi2Bins);
    }

    for (int isec = 0; isec < DCConstants::nSect; isec++) {
        //        h_DC_Segments_[isec] = new TH2F(Form("h_DC_Segments_%d", isec), "", 100, -200., 220., 100, 200., 550.);
        //        h_DC_Segments_[isec]->SetStats(0);

        c_Chi2Seg->cd(isec + 1)->SetLogx();
        c_Chi2SegTBSegMatched->cd(isec + 1)->SetLogx();
        c_Chi2CutEff->cd(isec + 1)->SetLogx();
        h_Chi2_SegFit_[isec] = new TH1D(Form("h_Chi2_SegFit_%d", isec), "", nChi2Bins, chi2Bins);

        for (int iSL = 0; iSL < nSL; iSL++) {
            h_TBT0_[isec][iSL] = new TH1D(Form("h_TBT0_%d_%d", isec, iSL), "", 200, 50., 210.);
            h_TBT0_[isec][iSL]->SetLineColor(iSL + 1);

            h_Chi2_SegFitSeparated_[isec][iSL] = new TH1D(Form("h_Chi2_SegFit_%d_%d", isec, iSL), "", nChi2Bins, chi2Bins);
            h_Chi2_SegFitSeparated_[isec][iSL]->SetLineColor(iSL + 1);
            h_Chi2_SegFitSeparatedTBHitMatched_[isec][iSL] = new TH1D(Form("h_Chi2_SegFitSeparatedTBHitMatched_isec_%d_%d", isec, iSL), "", nChi2Bins, chi2Bins);
            h_Chi2_SegFitSeparatedTBHitMatched_[isec][iSL]->SetLineColor(iSL + 1);
            h_Chi2_CutEff_[isec][iSL] = new TH1D(Form("h_Chi2_CutEff_%d_%d", isec, iSL), "", nChi2Bins, chi2Bins);
            h_Chi2_CutEff_[isec][iSL]->SetLineColor(iSL + 1);
        }
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

/**
 * 
 * @param seg1
 * @param seg2z
 * @return true if seg1 and seg2 are coincide. Here coincidence means, there are
 * at least three DC hit that these segments share.
 */
bool SegDisplay::CompareSegments(vector<DCHit> &seg1, vector<DCHit> &seg2) {

    int nMatchHits = 0;

    for (DCHit hit1 : seg1) {

        //cout << "******************* The size of seg1 is " << seg1.size() << "    The size of seg2 is " << seg2.size() << endl;
        for (DCHit hit2 : seg2) {
            //            cout << "************************************************" << endl;
            //            cout << "Sectors     : " << hit1.sector << "   " << hit2.sector << endl;
            //            cout << "Layers      : " << hit1.layer << "   " << hit2.layer << endl;
            //            cout << "Wire numbers: " << hit1.wireNo << "   " << hit2.wireNo << endl;

            if (hit1 == hit2) {
                //                cout << "!!! MATHCH, going to the next Raw DC Hit" << endl;
                nMatchHits = nMatchHits + 1;
                break;
            }

        }

        if (nMatchHits >= nHitMatchPerSeg) {
            //cout<<"Ok, segments are matched. At the moment the number of matched hits is "<<nMatchHits<<endl;
            break;
        }
    }

    return nMatchHits >= nHitMatchPerSeg ? true : false;
}

bool SegDisplay::RunEvents(int nev) {

    if (!IsFileOpened) {
        popupMSG("You need to chose a file first");
        return false;
    }

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for (int i = 0; i < nev; i++) {

        if (i + 1 % 5000 == 0) {
            cout.flush() << "Processed " << i + 1 << " events \r";
        }

        /*
         * Before going to next event reset some variables
         */

        for (int isec = 0; isec < nSec; isec++) {
            for (int iSL = 0; iSL < nSL; iSL++) {
                tbSegments[isec][iSL].clear();
                tbSegments[isec][iSL].shrink_to_fit();
            }
        }

        if (fReader.next() == true) {

            //cout << "Going to the next event" << endl;

            IsPassedChi2Cut = false;
            fReader.read(fEvent);

            if (but_TBSegs->IsOn()) {
                ProcessTBSegs(fEvent);
            }

            if (but_rawHits->IsOn() || but_rawSegs->IsOn()) {
                ProcessRawDCSeg(fEvent);
            }

            if (but_HBHits->IsOn()) {
                ProcessHBHits(fEvent);
            }

            if (but_TBHits->IsOn() || but_TBHitsFit->IsOn()) {
                ProcessTBHits(fEvent);
            }


            if( IsPassedChi2Cut ){
                fwriter.addEvent(fEvent);
            }
            
        } else {
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            double dt_inSec = 1.e-3 * std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            cout << "Total elapsed time for processing " << nev << " events is " << dt_inSec << " sec,  and the average time per event is " << dt_inSec / double(i) << " sec" << endl;
            return false;
        }
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    double dt_inSec = 1.e-3 * std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    cout << "Total elapsed time for processing " << nev << " events is " << dt_inSec << " sec,  and the average time per event is " << dt_inSec / double(nev) << " sec" << endl;


    return true;
}

void SegDisplay::CloseApp() {
    cout << " Exiting the SegDisplay. \n Good bye,  Ցտեսություն, До свидания" << endl;

    //    for (int iSec = 0; iSec < nSec; iSec++) {
    //        delete h_DC_Segments_[iSec];
    //    }
    fwriter.close();
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

    /*
     * We need to clear up TLatex objects otherwise it could result in a memory leak
     */

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

        for (int iSL = 0; iSL < nSL; iSL++) {
            h_TBT0_[isec][iSL]->Reset();
        }
    }

    RunEvents(fNev);

    for (int isec = 0; isec < DCConstants::nSect; isec++) {
        c_Chi2Seg->cd(isec + 1);
        ConvertDifferential(h_Chi2_SegFit_[isec]);
        //h_Chi2_SegFit_[isec]->Draw();

        double maxVal = 0;
        ConvertDifferential(h_Chi2_SegFitSeparated_[isec][0]);
        h_Chi2_SegFitSeparated_[isec][0]->Draw();
        for (int iSL = 1; iSL < nSL; iSL++) {
            ConvertDifferential(h_Chi2_SegFitSeparated_[isec][iSL]);
            maxVal = TMath::Max(maxVal, h_Chi2_SegFitSeparated_[isec][iSL]->GetMaximum());
            h_Chi2_SegFitSeparated_[isec][iSL]->Draw("Same");
        }
        h_Chi2_SegFitSeparated_[isec][0]->SetMaximum(1.05 * maxVal);

        c_Chi2SegTBSegMatched->cd(isec + 1);
        ConvertDifferential(h_Chi2_SegFitSeparatedTBHitMatched_[isec][0]);
        maxVal = 0;
        h_Chi2_SegFitSeparatedTBHitMatched_[isec][0]->Draw();
        for (int iSL = 1; iSL < nSL; iSL++) {
            ConvertDifferential(h_Chi2_SegFitSeparatedTBHitMatched_[isec][iSL]);
            maxVal = TMath::Max(maxVal, h_Chi2_SegFitSeparatedTBHitMatched_[isec][iSL]->GetMaximum());
            h_Chi2_SegFitSeparatedTBHitMatched_[isec][iSL]->Draw("Same");
        }
        h_Chi2_SegFitSeparatedTBHitMatched_[isec][0]->SetMaximum(1.05 * maxVal);


        c_Chi2CutEff->cd(isec + 1);

        for (int iSL = 0; iSL < nSL; iSL++) {
            h_Chi2_CutEff_[isec][iSL] = (TH1D*) h_Chi2_SegFitSeparatedTBHitMatched_[isec][iSL]->GetCumulative(1);
            h_Chi2_CutEff_[isec][iSL]->SetName(Form("h_Chi2_CutEff_%d_%d", isec, iSL));
            h_Chi2_CutEff_[isec][iSL]->Scale(1. / h_Chi2_CutEff_[isec][iSL]->GetBinContent(h_Chi2_CutEff_[isec][iSL]->GetMaximumBin()));

            iSL == 0 ? h_Chi2_CutEff_[isec][iSL]->Draw("hist") : h_Chi2_CutEff_[isec][iSL]->Draw("hist Same");
        }
        //h_Chi2_CutEff_
        maxVal = 0;
        c_TBT0->cd(isec + 1);
        h_TBT0_[isec][0]->Draw();
        for (int iSL = 1; iSL < nSL; iSL++) {
            h_TBT0_[isec][iSL]->Draw("Same");
            maxVal = TMath::Max(maxVal, h_TBT0_[isec][iSL]->GetMaximum());
        }
        h_TBT0_[isec][0]->SetMaximum(1.05 * maxVal);
    }
    c_Chi2Seg->Modified();
    c_Chi2Seg->Update();
    c_Chi2SegTBSegMatched->Modified();
    c_Chi2SegTBSegMatched->Update();
    c_Chi2CutEff->Modified();
    c_Chi2CutEff->Update();

    c_TBT0->Modified();
    c_TBT0->Update();
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
    fBHBHits = hipo::bank(fFactory.getSchema("HitBasedTrkg::HBHits"));
    fBTBHits = hipo::bank(fFactory.getSchema("TimeBasedTrkg::TBHits"));
    fBTBSegs = hipo::bank(fFactory.getSchema("TimeBasedTrkg::TBSegments"));
    IsFileOpened = true;
    
    chi2Cut = 12;
    fwriter.getDictionary().addSchema(fFactory.getSchema("DC::tdc"));
    fwriter.open(Form("Skimmed_testDCRB_Chi2Cut_%1.1f.hipo", chi2Cut));
    
    return IsFileOpened;
}

bool SegDisplay::ProcessRawDCSeg(hipo::event &event) {
    fEvent.getStructure(fBDCtdc);

    int nDCHits = fBDCtdc.getRows();

    std::set<DCHit> hitSet[DCConstants::nSect][DCConstants::nLayerperSec];


    for (int ihit = 0; ihit < nDCHits; ihit++) {
        int sec = fBDCtdc.getInt("sector", ihit) - 1;

        int layer = fBDCtdc.getInt("layer", ihit) - 1;
        int SL = layer / DCConstants::nLayerperSL;
        int w = fBDCtdc.getInt("component", ihit) - 2;
        //int w = fBDCtdc.getInt("component", ihit) - 1;
        int tdc = fBDCtdc.getInt("TDC", ihit);


        /**
         * Needs to be fixed/understood!!
         */
        w = TMath::Max(w, 0);

        if (w < 0) {
            cout << "***************** Oho wire number is negative  " << w << endl;
            cin.ignore();
        }

        double w_Xmid = DCConsts.w_midpoint_x[layer][w];
        double w_Ymid = DCConsts.w_midpoint_y[layer][w];


        // The "900 - 575*Iwir/112" is Mac's formula for SL2 and SL3 (counting from 0)
        double r = (SL == 2 || SL == 3) ? TMath::Min(DMax[SL], TMath::Max((tdc - tMin[SL]), float(0.)) * DMax[SL] / (tMax[SL] - 575. * (w + 1.) / double(DCConstants::nWirePerLayer) - tMin[SL])) :
                TMath::Min(DMax[SL], TMath::Max((tdc - tMin[SL]), float(0.)) * DMax[SL] / (tMax[SL] - tMin[SL]));
        double x = TMath::Min(float(r), float(DMax[SL])) / DMax[SL];
        
        /**
         *                 FIX Me
         * the 0.1 should not be hardcoded
         */
        double err_r = 0.1 + CalcDocaError(x) * DMax[SL];
        //double err_r = 0.15 * r;

        if (IsSingleEvent) {
            c_DCHits[sec]->cd();

            if (but_rawHits->IsOn()) {
                circRawDoca.DrawArc(w_Xmid, w_Ymid, r);
            }
        }

        hitSet[sec][layer].insert(DCHit(sec, layer, w, w_Xmid, w_Ymid, r, err_r));

    }

    for (int isec = 0; isec < DCConstants::nSect; isec++) {

        SegFinder segFinder(hitSet[isec]);

        vector< vector<DCHit> > v_segments = segFinder.GetAllSegmentCandidates();

        for (vector<DCHit> curSegm : v_segments) {

            if (curSegm.size() >= 5 && curSegm.size() < 12) {

                int curSec = curSegm.at(0).sector;
                int curSL = curSegm.at(0).layer / 6;

                bool tbSegMatched = false;

                for (vector<DCHit> curTBSeg : tbSegments[curSec][curSL]) {

                    // Checking if the current Raw Segment is matched to any recon TBsegment
                    if (CompareSegments(curSegm, curTBSeg)) {
                        tbSegMatched = true;
                        break;
                    }
                }

                vector<DCHit> v_LayerHit_[DCConstants::nLayerperSL];
                int indLayer[DCConstants::nLayerperSL] = {0};

                /**
                 * We want make sub segments from the given segment, such in the sub-segment each layer will only have
                 * a single hit in the layer
                 */

                for (DCHit curHit : curSegm) {
                    v_LayerHit_[curHit.layer % DCConstants::nLayerperSL].push_back(curHit);
                }

                vector< vector<DCHit> > vv;
                for (int iL = 0; iL < DCConstants::nLayerperSL; iL++) {
                    if (v_LayerHit_[iL].size() > 0) {
                        vv.push_back(v_LayerHit_[iL]);
                    }
                }

                double chi2Min = 1.e25;
                double chi2;
                vector<DCHit> segMinChi2;
                SegFitter segFitMinChi2;

                // Doing something similar to the algorithm in https://stackoverflow.com/questions/46568287/implementing-an-iterator-over-combinations-of-many-vectors
                std::vector<std::vector<DCHit>::iterator> vit;

                for (auto& v : vv) {
                    vit.push_back(v.begin());
                }
                int K = vv.size();

                while (vit[0] != vv[0].end()) {
                    vector<DCHit> curSubSeg;
                    //                    std::cout << "Processing combination: [";
                    for (auto& i : vit) {
                        //                      std::cout << " (" << (*i).layer << "," << (*i).wireNo << ") ";
                        curSubSeg.push_back(*i);
                    }
                    //                std::cout << "]\n";

                    SegFitter segFitter(curSubSeg);
                    //              cout << "The size of cur SubSeg is " << curSubSeg.size() << endl;
                    if (segFitter.GetFitChi2() < chi2Min) {
                        //                cout << "Cur MinChi2 = " << segFitter.GetFitChi2() << endl;
                        chi2Min = segFitter.GetFitChi2();
                        segMinChi2 = curSubSeg;
                        segFitMinChi2 = segFitter;
                    }

                    // increment "odometer" by 1
                    ++vit[K - 1];
                    for (int i = K - 1; (i > 0) && (vit[i] == vv[i].end()); --i) {
                        vit[i] = vv[i].begin();
                        ++vit[i - 1];
                    }
                }

                int ndf = segMinChi2.size() - 2;
                chi2 = chi2Min;
                //h_chi2_1.Fill(chi2);


                if( chi2 < chi2Cut ){
                    IsPassedChi2Cut = true;
                }

                double slope = segFitMinChi2.GetSlope();
                double offset = segFitMinChi2.GetOffset();
                //                cout << "Size of MinChi2 Segment is " << segMinChi2.size() << endl;
                double x_0 = segMinChi2.at(0).x < segMinChi2.at(segMinChi2.size() - 1).x ? segMinChi2.at(0).x - segMinChi2.at(0).r : segMinChi2.at(segMinChi2.size() - 1).x - segMinChi2.at(segMinChi2.size() - 1).r;
                double x_max = segMinChi2.at(0).x > segMinChi2.at(segMinChi2.size() - 1).x ? segMinChi2.at(0).x + segMinChi2.at(0).r : segMinChi2.at(segMinChi2.size() - 1).x + segMinChi2.at(segMinChi2.size() - 1).r;

                int SL = segMinChi2.at(0).layer / DCConstants::nLayerperSL;

                if (IsSingleEvent) {
                    /* if ( chi2 > 20000. )*/
                    {
                        c_DCHits[isec]->cd();
                        if (but_rawSegs->IsOn()) {
                            lineRawFit.DrawLine(x_0, slope * x_0 + offset, x_max, slope * x_max + offset);
                            //latChi2Raw.DrawLatex((x_max - DCConsts.xMin) / (DCConsts.xMax - DCConsts.xMin), (slope * x_max + offset - DCConsts.yMin) / (DCConsts.yMax - DCConsts.yMin), Form("%1.2f", chi2/double(ndf)));
                            latChi2Raw.DrawLatex(x_max, slope * x_max + offset, Form("%1.2f", chi2/double(ndf) ));
                            //v_chi2Texts.push_back(latChi2Raw.DrawLatex(x_max, slope * x_max + offset, Form("%1.2f", chi2)));

                            for (DCHit curSingleHit : segMinChi2 ) {
                                cout<<curSingleHit.x<<"    "<<curSingleHit.y<<"   "<<curSingleHit.r<<endl;
                                circRawDocaBest.DrawArc(curSingleHit.x, curSingleHit.y, curSingleHit.r);
                            }

                        }
                    }
                } else {
                    h_Chi2_SegFit_[isec]->Fill(chi2/double(ndf));
                    h_Chi2_SegFitSeparated_[isec][SL]->Fill(chi2/double(ndf));
                    if (tbSegMatched) {
                        h_Chi2_SegFitSeparatedTBHitMatched_[isec][SL]->Fill(chi2/double(ndf));
                    }
                }

            }

        }

    }

    return true;
}

bool SegDisplay::ProcessHBHits(hipo::event&) {


    fEvent.getStructure(fBHBHits);

    int nHits = fBHBHits.getRows();

    if (IsSingleEvent) {
        for (int ihit = 0; ihit < nHits; ihit++) {
            int sec = fBHBHits.getInt("sector", ihit) - 1;
            int SL = fBHBHits.getInt("superlayer", ihit) - 1;
            int layer = DCConsts.nLayerperSL * SL + fBHBHits.getInt("layer", ihit) - 1;
            int w = fBHBHits.getInt("wire", ihit) - 2;
            int tdc = fBHBHits.getInt("TDC", ihit);
            double doca = double(fBHBHits.getFloat("trkDoca", ihit));

            if (doca < 0) {
                continue;
            }

            double w_Xmid = DCConsts.w_midpoint_x[layer][w];
            double w_Ymid = DCConsts.w_midpoint_y[layer][w];

            c_DCHits[sec]->cd();
            circHBTrkDoca.DrawArc(w_Xmid, w_Ymid, doca);
        }

    }
    return true;
}

bool SegDisplay::ProcessTBHits(hipo::event&) {
    fEvent.getStructure(fBTBHits);

    int nHits = fBTBHits.getRows();

    for (int ihit = 0; ihit < nHits; ihit++) {

        int sec = fBTBHits.getInt("sector", ihit) - 1;
        int SL = fBTBHits.getInt("superlayer", ihit) - 1;
        int layer = DCConsts.nLayerperSL * SL + fBTBHits.getInt("layer", ihit) - 1;
        int w = fBTBHits.getInt("wire", ihit) - 2;
        int tdc = fBTBHits.getInt("TDC", ihit);
        double trkdoca = double(fBTBHits.getFloat("trkDoca", ihit));
        double doca = double(fBTBHits.getFloat("doca", ihit));

        double tb_TStart = double( fBTBHits.getFloat("TStart", ihit));
        double tb_T0 = double( fBTBHits.getFloat("T0", ihit));
        double tb_TFlight = double( fBTBHits.getFloat("TFlight", ihit));
        double tb_TProp = double( fBTBHits.getFloat("TProp", ihit));
        double tb_TBeta = double( fBTBHits.getFloat("tBeta", ihit));

        double t0_tBHit = tb_TStart + tb_T0 + tb_TFlight + tb_TProp + tb_TBeta;

        if (doca < 0) {
            continue;
        }

        double w_Xmid = DCConsts.w_midpoint_x[layer][w];
        double w_Ymid = DCConsts.w_midpoint_y[layer][w];

        if (IsSingleEvent) {
            c_DCHits[sec]->cd();
            if (but_TBHits->IsOn()) {
                circTBDoca.DrawArc(w_Xmid, w_Ymid, doca);
            }
            if (but_TBHitsFit->IsOn()) {
                circTBTrkDoca.DrawArc(w_Xmid, w_Ymid, trkdoca);
            }
        } else {
            h_TBT0_[sec][SL]->Fill(t0_tBHit);
        }
    }


    return true;
}

bool SegDisplay::ProcessTBSegs(hipo::event&) {
    fEvent.getStructure(fBTBSegs);

    // In a rush now, but this should be done properly, i.e. check if the bank exist etc...
    fEvent.getStructure(fBDCtdc);

    int nSegs = fBTBSegs.getRows();




    for (int iseg = 0; iseg < nSegs; iseg++) {

        int sec = fBTBSegs.getInt("sector", iseg) - 1;
        int sl = fBTBSegs.getInt("superlayer", iseg) - 1;

        vector<DCHit> curSegm;

        for (int ihit = 0; ihit < 11; ihit++) {
            int hitID = fBTBSegs.getInt(Form("Hit%d_ID", ihit + 1), iseg) - 1;

            if (hitID >= 0) {

                int wireNo = TMath::Max(fBDCtdc.getInt("component", hitID) - 2, 0);
                int layer = fBDCtdc.getInt("layer", hitID) - 1;

                curSegm.push_back(DCHit(sec, layer, wireNo, -1, -1, -1, -1));
            } else {
                break;
            }

        }

        tbSegments[sec][sl].push_back(curSegm);

        double x1 = double(fBTBSegs.getFloat("SegEndPoint1X", iseg));
        double x2 = double(fBTBSegs.getFloat("SegEndPoint2X", iseg));
        double z1 = double(fBTBSegs.getFloat("SegEndPoint1Z", iseg));
        double z2 = double(fBTBSegs.getFloat("SegEndPoint2Z", iseg));

        double x1Rot = x1 * cos(tiltAngle) - z1 * sin(tiltAngle);
        double z1Rot = x1 * sin(tiltAngle) + z1 * cos(tiltAngle);
        double x2Rot = x2 * cos(tiltAngle) - z2 * sin(tiltAngle);
        double z2Rot = x2 * sin(tiltAngle) + z2 * cos(tiltAngle);

        if (IsSingleEvent) {
            c_DCHits[sec]->cd();
            lineTBSeg.DrawLine(x1Rot, z1Rot, x2Rot, z2Rot);
        }
    }

    return true;
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

void SegDisplay::ConvertDifferential(TH1 * h) {
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

    if (but_rawHits->IsOn() || but_rawSegs->IsOn()) {
        ProcessRawDCSeg(fEvent);
    }

    if (but_HBHits->IsOn()) {
        ProcessHBHits(fEvent);
    }
    if (but_TBHits->IsOn() || but_TBHitsFit->IsOn()) {
        ProcessTBHits(fEvent);
    }

    if (but_TBSegs->IsOn()) {
        ProcessTBSegs(fEvent);
    }

    for (int isec = 0; isec < DCConstants::nSect; isec++) {
        c_DCHits[isec]->Modified();
        c_DCHits[isec]->Update();
    }

}

void SegDisplay::MouseAction(Int_t ev, Int_t ix, Int_t iy, TObject * selected) {

    if (strcmp("TCanvas", selected->ClassName()) != 0) {
        return;
    }

    int iXMax = ((TCanvas*) selected)->GetWw();
    int iYMax = ((TCanvas*) selected)->GetWw();

    ((TCanvas*) selected)->cd();
    double yNewMax, yOldMax;
    double yNewMin, yOldMin;

    double xNewMax, xOldMax;
    double xNewMin, xOldMin;

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
            ((TCanvas*) selected)->GetRange(xOldMin, yOldMin, xOldMax, yOldMax);

            //            cout<<"kButton1Up is detected"<<endl;
            ixRelease = TMath::Max(ix, 0);
            ixRelease = TMath::Min(ixRelease, iXMax);
            iyRelease = TMath::Max(iy, 0);
            iyRelease = TMath::Min(iyRelease, iYMax);


            yNewMax = yOldMax - (yOldMax - yOldMin) * double(TMath::Min(iyClick, iyRelease)) / double(iYMax);
            yNewMin = yOldMax - (yOldMax - yOldMin) * double(TMath::Max(iyClick, iyRelease)) / double(iYMax);

            xNewMax = xOldMin + (xOldMax - xOldMin) * double(TMath::Max(ixClick, ixRelease)) / double(iXMax);
            xNewMin = xOldMin + (xOldMax - xOldMin) * double(TMath::Min(ixClick, ixRelease)) / double(iXMax);
            //            yNewMax = DCConsts.yMax - (DCConsts.yMax - DCConsts.yMin) * double(TMath::Min(iyClick, iyRelease)) / double(iYMax);
            //            yNewMin = DCConsts.yMax - (DCConsts.yMax - DCConsts.yMin) * double(TMath::Max(iyClick, iyRelease)) / double(iYMax);
            //
            //            xNewMax = DCConsts.xMin + (DCConsts.xMax - DCConsts.xMin) * double(TMath::Max(ixClick, ixRelease)) / double(iXMax);
            //            xNewMin = DCConsts.xMin + (DCConsts.xMax - DCConsts.xMin) * double(TMath::Min(ixClick, ixRelease)) / double(iXMax);

            delta_Y = yNewMax - yNewMin;
            delta_X = xNewMax - xNewMin;

            // We need to have a square zoom, that circles will appear as circles

            if (delta_X < delta_Y) {
                if (ixClick < iyRelease) {
                    xNewMax = TMath::Min(DCConsts.xMax, xNewMin + delta_Y);
                    xNewMin = xNewMax - delta_Y;
                } else {
                    xNewMin = TMath::Max(DCConsts.xMin, xNewMax - delta_Y);
                    xNewMax = xNewMin + delta_Y;
                }
            } else {
                if (iyClick < iyRelease) {
                    yNewMin = TMath::Max(DCConsts.yMin, yNewMax - delta_X);
                    yNewMax = yNewMin + delta_X;
                } else {
                    yNewMax = TMath::Min(DCConsts.yMax, yNewMin + delta_X);
                    yNewMin = yNewMax - delta_X;
                }
            }



            //ProcessRawDCSeg(fEvent);
            ((TCanvas*) selected)->Range(xNewMin, yNewMin, xNewMax, yNewMax);
            ((TCanvas*) selected)->Modified();
            ((TCanvas*) selected)->Update();
            break;
        default:

            break;

    }

}

void SegDisplay::ResetDoca() {
    // Read the parameters from the file and assign o tMin, tMax and dMax
    ReadDocaPars();
    // Update the viewer
    UpdateEvent();
}

void SegDisplay::UnzoomAll() {

    for (int isec = 0; isec < nSec; isec++) {
        c_DCHits[isec]->Range(DCConsts.xMin, DCConsts.yMin, DCConsts.xMax, DCConsts.yMax);
    }

    // Update the viewer
    UpdateEvent();
}

void SegDisplay::SetManualDOCAPars() {

    if (fMain_docaPars) {
        fMain_docaPars->RaiseWindow();
        return;
    }

    fMain_docaPars = new TGTransientFrame(gClient->GetRoot(), p_wind, 400, 200);
    fMain_docaPars->Connect("CloseWindow()", "SegDisplay", this, "CloseManualDOCAPars()");
    fMain_docaPars->SetCleanup(kDeepCleanup); // Without this line it crashes, when one closes the window
    fMain_docaPars->DontCallClose(); // To avoid double deletions

    fMain_docaPars->SetWindowName("Set Doca parameters individually");

    // Map all sub-widows of main frame
    fMain_docaPars->MapSubwindows();
    // Initialize the layout algorithm
    fMain_docaPars->Resize(fMain_docaPars->GetDefaultSize());
    // Map main frame
    fMain_docaPars->MapWindow();

}

void SegDisplay::CloseManualDOCAPars() {
    fMain_docaPars->CloseWindow();
    fMain_docaPars = NULL;
}

SegDisplay::SegDisplay(const SegDisplay & orig) {
}

SegDisplay::~SegDisplay() {
}

