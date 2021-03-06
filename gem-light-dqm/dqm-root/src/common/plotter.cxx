/**
 * A collection of simple ROOT macros.
 *
 * N. Amapane, G. Cerminara, M. Dalchenko
 */

#include <sstream>
#include <iomanip>

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "TProfile.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TVirtualPad.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TPostScript.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TAxis.h"
#include "TGaxis.h"
#include "TMath.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TObject.h"
#include "TH1.h"
#include "TH1F.h"

#include <iostream>

using namespace std;

#endif



TStyle * getStyle(TString name) 
{
    TStyle *theStyle;
    
    if ( name == "ZZ" ) {
        // Style from http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/UserCode/pandolf/CommonTools/DrawBase.C?revision=1.25&view=markup
        theStyle = new TStyle("ZZStyle", "ZZ Style");
        theStyle->SetCanvasColor(0);
        theStyle->SetPadColor(0);
        theStyle->SetFrameFillColor(0);
        theStyle->SetStatColor(0);
        theStyle->SetOptStat(1111);
        theStyle->SetTitleFillColor(0);
        theStyle->SetCanvasBorderMode(0);
        theStyle->SetPadBorderMode(0);
        theStyle->SetFrameBorderMode(0);
        theStyle->SetPadBottomMargin(0.12);
        theStyle->SetPadLeftMargin(0.12);
        theStyle->cd();
        
        // For the canvas:
        theStyle->SetCanvasBorderMode(0);
        theStyle->SetCanvasColor(kWhite);
        theStyle->SetCanvasDefH(600); //Height of canvas
        theStyle->SetCanvasDefW(600); //Width of canvas
        theStyle->SetCanvasDefX(0);   //POsition on screen
        theStyle->SetCanvasDefY(0);
        
        // For the Pad:
        theStyle->SetPadBorderMode(0);
        theStyle->SetPadColor(kWhite);
        theStyle->SetPadGridX(false);
        theStyle->SetPadGridY(false);
        theStyle->SetGridColor(0);
        theStyle->SetGridStyle(3);
        theStyle->SetGridWidth(1);
        
        // For the frame:
        theStyle->SetFrameBorderMode(0);
        theStyle->SetFrameBorderSize(1);
        theStyle->SetFrameFillColor(0);
        theStyle->SetFrameFillStyle(0);
        theStyle->SetFrameLineColor(1);
        theStyle->SetFrameLineStyle(1);
        theStyle->SetFrameLineWidth(1);
        
        // Margins:
        theStyle->SetPadTopMargin(0.05);
        theStyle->SetPadBottomMargin(0.15);//0.13);
        theStyle->SetPadLeftMargin(0.2);//0.16);
        theStyle->SetPadRightMargin(0.15);//0.02);
        
        // For the Global title:
        theStyle->SetOptTitle(0);
        theStyle->SetTitleFont(42);
        theStyle->SetTitleColor(1);
        theStyle->SetTitleTextColor(1);
        theStyle->SetTitleFillColor(10);
        theStyle->SetTitleFontSize(0.05);
        
        // For the axis titles:
        theStyle->SetTitleColor(1, "XYZ");
        theStyle->SetTitleFont(42, "XYZ");
        theStyle->SetTitleSize(0.05, "XYZ");
        theStyle->SetTitleXOffset(1.25);//0.9);
        theStyle->SetTitleYOffset(1.65); // => 1.15 if exponents
        
        // For the axis labels:
        theStyle->SetLabelColor(1, "XYZ");
        theStyle->SetLabelFont(42, "XYZ");
        theStyle->SetLabelOffset(0.007, "XYZ");
        theStyle->SetLabelSize(0.045, "XYZ");
        
        // For the axis:
        theStyle->SetAxisColor(1, "XYZ");
        theStyle->SetStripDecimals(kTRUE);
        theStyle->SetTickLength(0.03, "XYZ");
        theStyle->SetNdivisions(10, "XYZ");
        theStyle->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
        theStyle->SetPadTickY(1);
        TGaxis::SetMaxDigits(3);
    }else if ( name == "myStyle" ) {
        theStyle = new TStyle("myStyle", "myStyle");
        //    theStyle->SetOptStat(0);
        theStyle->SetPadBorderMode(0);
        theStyle->SetCanvasBorderMode(0);
        theStyle->SetPadColor(0);
        theStyle->SetCanvasColor(0);
        theStyle->SetMarkerStyle(8);
        theStyle->SetMarkerSize(0.7);
        theStyle->SetStatH(0.3);
        theStyle->SetStatW(0.15);
        //   theStyle->SetTextFont(132);
        //   theStyle->SetTitleFont(132);
        theStyle->SetTitleBorderSize(1);
        theStyle->SetPalette(1);
        
    }else if( name == "tdr" ) {
        theStyle = new TStyle("tdrStyle","Style for P-TDR");
        
        // For the canvas:
        theStyle->SetCanvasBorderMode(0);
        theStyle->SetCanvasColor(kWhite);
        theStyle->SetCanvasDefH(600); //Height of canvas
        theStyle->SetCanvasDefW(600); //Width of canvas
        theStyle->SetCanvasDefX(0);   //POsition on screen
        theStyle->SetCanvasDefY(0);
        
        // For the Pad:
        theStyle->SetPadBorderMode(0);
        // theStyle->SetPadBorderSize(Width_t size = 1);
        theStyle->SetPadColor(kWhite);
        theStyle->SetPadGridX(true);
        theStyle->SetPadGridY(true);
        theStyle->SetGridColor(0);
        theStyle->SetGridStyle(3);
        theStyle->SetGridWidth(1);
        
        // For the frame:
        theStyle->SetFrameBorderMode(0);
        theStyle->SetFrameBorderSize(1);
        theStyle->SetFrameFillColor(0);
        theStyle->SetFrameFillStyle(0);
        theStyle->SetFrameLineColor(1);
        theStyle->SetFrameLineStyle(1);
        theStyle->SetFrameLineWidth(1);
        
        // For the histo:
        theStyle->SetHistLineColor(1);
        theStyle->SetHistLineStyle(0);
        theStyle->SetHistLineWidth(1);
        
        
        theStyle->SetEndErrorSize(2);
        
        theStyle->SetMarkerStyle(20);
        theStyle->SetMarkerSize(0.5);
        
        
        //For the fit/function:
        theStyle->SetOptFit(1);
        theStyle->SetFitFormat("5.4g");
        theStyle->SetFuncColor(2);
        theStyle->SetFuncStyle(1);
        theStyle->SetFuncWidth(1);
        
        //For the date:
        theStyle->SetOptDate(0);
        
        // For the statistics box:
        theStyle->SetOptFile(0);
        
        theStyle->SetOptStat("e");
        theStyle->SetStatColor(kWhite);
        theStyle->SetStatTextColor(1);
        theStyle->SetStatFormat("6.4g");
        theStyle->SetStatBorderSize(1);
        
        // Margins:
         theStyle->SetPadTopMargin(0.1);
         theStyle->SetPadBottomMargin(0.1);
         theStyle->SetPadLeftMargin(0.1);
         theStyle->SetPadRightMargin(0.05);
        
        // For the Global title:
        
        // Uncomment to remove title
        //    theStyle->SetOptTitle(0); 
        theStyle->SetTitleFont(42);
        theStyle->SetTitleColor(1);
        theStyle->SetTitleTextColor(1);
        theStyle->SetTitleFillColor(10);
        theStyle->SetTitleFontSize(0.05);
        // theStyle->SetTitleH(0); // Set the height of the title box
        // theStyle->SetTitleW(0); // Set the width of the title box
        // theStyle->SetTitleX(0); // Set the position of the title box
        theStyle->SetTitleY(1); // Set the position of the title box
        theStyle->SetTitleStyle(1001);
        // theStyle->SetTitleBorderSize(2);
        
        // For the axis titles:
        
        theStyle->SetTitleColor(1, "XYZ");
        theStyle->SetTitleFont(42, "XYZ");
        theStyle->SetTitleSize(0.05, "XYZ");
        theStyle->SetTitleXOffset(0.9);
        theStyle->SetTitleYOffset(1.25);
        
        // For the axis labels:
        
        theStyle->SetLabelColor(1, "XYZ");
        
        theStyle->SetLabelFont(42, "XYZ");
        
        theStyle->SetLabelOffset(0.007, "XYZ");
        
        theStyle->SetLabelSize(0.045, "XYZ");
        
        // For the axis:
        
        theStyle->SetAxisColor(1, "XYZ");
        theStyle->SetStripDecimals(kTRUE);
        theStyle->SetTickLength(0.03, "XYZ");
        theStyle->SetNdivisions(510, "XYZ");
        theStyle->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
        theStyle->SetPadTickY(1);
        
        // Change for log plots:
        theStyle->SetOptLogx(0);
        theStyle->SetOptLogy(0);
        theStyle->SetOptLogz(0);
        
        // Postscript options:
        theStyle->SetPaperSize(20.,20.);
        // theStyle->SetLineScalePS(Float_t scale = 3);
        // theStyle->SetLineStyleString(Int_t i, const char* text);
        // theStyle->SetHeaderPS(const char* header);
        // theStyle->SetTitlePS(const char* pstitle);
        
        // theStyle->SetBarOffset(Float_t baroff = 0.5);
        // theStyle->SetBarWidth(Float_t barwidth = 0.5);
        // theStyle->SetPaintTextFormat(const char* format = "g");
        // theStyle->SetPalette(Int_t ncolors = 0, Int_t* colors = 0);
        // theStyle->SetTimeOffset(Double_t toffset);
        // theStyle->SetHistMinimumZero(kTRUE);
        theStyle->SetTextSize(0.045);
        theStyle->SetTextFont(42);
        
        //   style->SetOptFit(101);
        //   style->SetOptStat(1111111); 
        
    }else {
        // Avoid modifying the default style!
        theStyle = gStyle;
    }
    return theStyle;
}

// Make a new canvas setting name, title, x/y divisions, size.
TCanvas * newCanvas(TString title="", Int_t xdiv=1, Int_t ydiv =1,
            Int_t w=600, Int_t h=600)
{
    TCanvas* c= new TCanvas;
    c->SetWindowSize(w,h);
    if (title != "") c->SetTitle(title);
    if (xdiv*ydiv>1) {
        c->Divide(xdiv,ydiv);
        c->cd(1);
    }
    return c;
}

// Print all open canvases in separate files
// Type = "png", "eps", etc.
void printCanvases(TString type)
{
    TIter iter(gROOT->GetListOfCanvases());
    TCanvas *c;
    while( (c = (TCanvas *)iter()) ) {
        TString name =  c->GetTitle();
        c->Print(name+"."+type,type);
    }
}

// Set histogram titles
void setTitles(TH1 *h, TString xtitle, TString ytitle)
{
    h->GetXaxis()->SetTitle(xtitle);
    h->GetYaxis()->SetTitle(ytitle);
}

void setTitles(TH1 *h, TString xtitle, TString ytitle, TString ztitle)
{
    h->GetXaxis()->SetTitle(xtitle);
    h->GetYaxis()->SetTitle(ytitle);
    h->GetZaxis()->SetTitle(ztitle);
}

// Print all histograms in separate files
// Type = "png", "eps", etc.
void printHistograms(TString type, TString prefix="")
{
    TIter next(gDirectory->GetListOfKeys());
    TKey *key;
    while ((key = (TKey*)next())) 
    {
        TClass *cl = gROOT->GetClass(key->GetClassName());
        if (!cl->InheritsFrom("TH1")) continue;
        TH1 *h = (TH1*)key->ReadObj();
        TCanvas *c = newCanvas();
        h->Draw();
        TString name =  h->GetTitle();
        if (prefix!="") gROOT->ProcessLine(".!mkdir -p ./"+prefix);
        c->Print(prefix+name+"."+type,type);
        delete c;
    }
}


