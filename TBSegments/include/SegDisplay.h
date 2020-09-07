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

    void InitSettings(); // Init some settings
    bool InitFile(std::string);
    void ReadDocaPars();

    std::string onlineDir;
    std::string docaParFilename;

    // ====== Processors =========
    bool ProcessDCSeg(hipo::event &); // DC SegFinderProcessor

    hipo::reader fReader; // The Hipo Reader
    std::string fInpFileName; // Name of the input hipo file
    hipo::event fEvent; // Hipo event
    hipo::dictionary fFactory; // Hipo factory
    hipo::bank fBDCtdc; // Bank DC::tdc



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
    TRootEmbeddedCanvas *fEC_DCHits;
    TRootEmbeddedCanvas *fEC_Chi2Seg;

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
    TArc circRawDoca;
    TCanvas *c_DCHits;
    TCanvas *c_Chi2Seg;
    TPad *p_c_DCHits_[nSec]; // one pad for each sector

    TH2F *h_DC_Segments_[nSec];

    /**
     * Histograms
     */

    TH1D *h_Chi2_SegFit_[nSec];



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
    void MouseZoom(int, int, int, TObject *);
    void DoTab(Int_t);
    void UpdateEvent(); // Intended to be used, when some parameter is changed, Using this it will display segments with updated pars
    void ResetDoca();   // Will reset DOCA parameters to initial values, and update the viewer
};

#endif /* SEGDISPLAY_H */