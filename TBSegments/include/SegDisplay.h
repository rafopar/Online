/* 
 * File:   SegDisplay.h
 * Author: rafopar
 *
 * Created on August 26, 2020, 8:10 PM
 */

#ifndef SEGDISPLAY_H
#define SEGDISPLAY_H

#include <TQObject.h>
#include <RQ_OBJECT.h>

#include <TArc.h>
#include <TH2F.h>
#include <TGTab.h>
#include <TLine.h>
#include <TCanvas.h>
#include <TGFrame.h>
#include <TGWindow.h>
#include <TGFileDialog.h>
#include <TGNumberEntry.h>
#include <TGDoubleSlider.h>
#include <TRootEmbeddedCanvas.h>

#include "DCObjects.h"

// ===== Hipo headers =====
#include <reader.h>
#include <dictionary.h>

class SegDisplay {
private:

    RQ_OBJECT("SegDisplay");

    static const int nSec = 6;
    static const int nSL = 6; // The number of SuperLayers
    static const double tiltAngle;

    void InitSettings(); // Init some settings
    bool InitFile(std::string);
    void ReadDocaPars();


    double CalcDocaError(double); // Calculates the DOCA error. According to the DC recon code // referred there as Mac's formula

    std::string onlineDir;
    std::string docaParFilename;

    // ====== Processors =========
    bool ProcessRawDCSeg(hipo::event &); // DC SegFinderProcessor
    bool ProcessHBHits(hipo::event &); // Processing hit based DC hits
    bool ProcessTBHits(hipo::event &); // Processing time based DC hits
    bool ProcessTBSegs(hipo::event &); // Processing time based DC Segments

    hipo::reader fReader; // The Hipo Reader
    std::string fInpFileName; // Name of the input hipo file
    hipo::event fEvent; // Hipo event
    hipo::dictionary fFactory; // Hipo factory
    hipo::bank fBDCtdc; // Bank DC::tdc
    hipo::bank fBHBHits; // Bank HitBasedTrkg::HBHits
    hipo::bank fBTBHits; // Bank TimeBasedTrkg::TBHits
    hipo::bank fBTBSegs; // Bank TimeBasedTrkg::TBSegments



    float tMin[nSL];
    float tMax[nSL];
    float DMax[nSL];

    DCConstants DCConsts;

    bool IsFileOpened;
    bool IsSingleEvent;

        
    /**
     * *************  GUI Objects, frames, buttons etc
     */

    const TGWindow *p_wind;
    TRootEmbeddedCanvas *fEC_DCHits[nSec];
    TRootEmbeddedCanvas *fEC_Chi2Seg;
    TRootEmbeddedCanvas *fEC_TB_Hit_t0; // Here by t_0 we mean "Online t_0" which is 't_start + t_0 + f_flight + t_prop + t_walk'

    /**
     * GUI frames
     */

    TGMainFrame *fMain;
    TGVerticalFrame *vframe;
    TGTab *fTab;
    TGHorizontalFrame *hframe;

    /**
     * GUI buttons
     */
    TGTextButton *but_next;
    TGTextButton *but_Exit;
    TGTextButton *but_Open;

    TGCheckButton *but_rawHits;
    TGCheckButton *but_rawSegs;
    TGCheckButton *but_HBHits;
    TGCheckButton *but_TBHits;
    TGCheckButton *but_TBHitsFit;
    TGCheckButton *but_TBSegs;
    
    /**
     * GUI NumberEntries
     */

    TGNumberEntry *nmb_nev2Run;
    /**
     * File fino
     */

    TGFileInfo fInpDataFileInfo;
    TGFileDialog *fDialog;

    /**
     * Group Frames
     */

    TGGroupFrame *fG_RunControls;
    TGGroupFrame *fG_docaControls;
    TGGroupFrame *fG_DisplayControls;


    /**
     * T0 Controls
     */

    TGDoubleHSlider *sl_TMinMax[nSL];

    // ===============================
    std::string fHipoDir;

    /**
     * ************* Miscellaneous Root Objects
     */

    TLine lineRawFit;
    TLine lineTBSeg;
    TArc circRawDoca;
    TArc circHBTrkDoca;
    TArc circTBDoca;
    TArc circTBTrkDoca;
    TCanvas *c_DCHits[nSec];
    TCanvas *c_Chi2Seg;
    TCanvas *c_TBT0; // Time based T0

    TH2F *h_DC_Segments_[nSec];

    int ixClick, iyClick, ixRelease, iyRelease; // Coordinates of mouse click and release
    double x0, y0, x1, y1; // Final coordinates in units of histogram axis

    /**
     * Histograms
     */

    TH1D *h_Chi2_SegFit_[nSec];
    TH1D *h_TBT0_[nSec][nSL];   // T0 of time based hits. Here by t_0 we mean "Online t_0" which is 't_start + t_0 + f_flight + t_prop + t_walk'



    /**
     * Functions
     */

    void ConvertDifferential(TH1*);

    /**
     * Built in type variables
     */

    unsigned int fNev; // # of events to process

    static const int nChi2Bins = 100;
    double chi2Bins[nChi2Bins + 1];

public:

    SegDisplay(const TGWindow *p, UInt_t w, UInt_t h);
    SegDisplay(const SegDisplay& orig);
    virtual ~SegDisplay();

    void Next(); // Go Run the next evet, mostly will show  DCsegments in a single event
    void AnalyzeNEvents(); // Analyze next N events
    void CloseApp();
    bool RunEvents(int nev); // Run nev events. Returns true, if file is still readable, and false otherwise, e.g. end of file is reached, or even file is not opened yet
    void OpenInpFile(); // creates a file dialog, to open the file
    void popupMSG(std::string);
    void DoTab(Int_t);
    void UpdateEvent(); // Intended to be used, when some parameter is changed, Using this it will display segments with updated pars
    void ResetDoca(); // Will reset DOCA parameters to initial values, and update the viewer
    void UnzoomAll(); // Will Unzoom views on all screens
    void MouseAction(Int_t, Int_t, Int_t, TObject *);
};

#endif /* SEGDISPLAY_H */