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
#include <TLine.h>
#include <TCanvas.h>
#include <TGFrame.h>
#include <TGWindow.h>
#include <TGFileDialog.h>
#include <TRootEmbeddedCanvas.h>

#include "DCObjects.h"

// ===== Hipo headers =====
#include <reader.h>
#include <dictionary.h>

class SegDisplay {
private:

    RQ_OBJECT("SegDisplay");

    void InitSettings(); // Init some settings
    bool InitFile(std::string);

    // ====== Processors =========
    bool ProcessDCSeg(hipo::event &); // DC SegFinderProcessor 

    hipo::reader fReader; // The Hipo Reader
    std::string fInpFileName; // Name of the input hipo file
    hipo::event fEvent; // Hipo event
    hipo::dictionary fFactory; // Hipo factory
    hipo::bank fBDCtdc; // Bank DC::tdc

    DCConstants DCConsts;
    
    bool IsFileOpened;
    bool IsSingleEvent;

    /**
     * *************  GUI Objects, frames, buttons etc
     */

    const TGWindow *p_wind;
    TRootEmbeddedCanvas *fECanvas;

    /**
     * GUI frames
     */

    TGMainFrame *fMain;
    TGVerticalFrame *vframe;

    /**
     * GUI buttons
     */
    TGTextButton *but_next;
    TGTextButton *but_Exit;
    TGTextButton *but_Open;


    /**
     * File fino
     */

    TGFileInfo fInpDataFileInfo;
    TGFileDialog *fDialog;

    // ===============================
    std::string fHipoDir;

    /**
     * ************* Miscellaneous Root Objects
     */

    TLine lineRawFit;
    TArc circRawDoca;
    TCanvas *c_DCHits;
    TPad *p_c_DCHits_[6]; // one pad for each sector
    
    /**
     * Histograms
     */


public:

    SegDisplay(const TGWindow *p, UInt_t w, UInt_t h);
    SegDisplay(const SegDisplay& orig);
    virtual ~SegDisplay();

    void Next();
    void CloseApp();
    bool RunEvents(int nev); // Run nev events. Returns true, if file is still readable, and false otherwise, e.g. end of file is reached, or even file is not opened yet
    void OpenInpFile(); // creates a file dialog, to open the file
    void popupMSG(std::string);
    void MouseZoom(int, int, int, TObject *);
};

#endif /* SEGDISPLAY_H */