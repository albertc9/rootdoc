#include "TStyle.h"
#include "TColor.h"
#include "TGaxis.h"

double text_size=0.06;

void MyStyle() 
{

//	TGaxis::SetMaxDigits(3);

 const Int_t NRGBs = 5;  
 const Int_t NCont = 256; 
 Double_t stops[NRGBs] = { 0.00, 0.0 , 0.2 ,  0.55, 1.00 };  
 Double_t red[NRGBs]   = { 0.00, 0.10, 0.47, 0.80, 0.51 };  
 Double_t green[NRGBs] = { 0.00, 0.15, 0.95, 0.20, 0.00 };  
 Double_t blue[NRGBs]  = { 0.0 , 0.67, 0.75, 0.50, 0.10 };  
 TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue,NCont); 
 gStyle->SetNumberContours(NCont);


  // use plain black on white colors
  Int_t icol=0; // WHITE
  gStyle->SetFrameBorderMode(icol);
  gStyle->SetFrameFillColor(icol);
  gStyle->SetLegendFillColor(icol);
  gStyle->SetLegendBorderSize(0.0);
  gStyle->SetCanvasBorderMode(icol);
  gStyle->SetCanvasColor(icol);
  gStyle->SetPadBorderMode(icol);
  gStyle->SetPadColor(icol);
  gStyle->SetStatColor(icol);
  //gStyle->SetFillColor(icol); // don't use: white fill color for *all* objects

//	gStyle->SetHistLineWidth(1.5);
  // set the paper & margin sizes
//  gStyle->SetPaperSize(20,26);

  // set margin sizes
  gStyle->SetPadTopMargin(0.08);
  gStyle->SetPadRightMargin(0.07);
  gStyle->SetPadBottomMargin(0.13);
  gStyle->SetPadLeftMargin(0.12);

  // set title offsets (for axis label)
//  gStyle->SetTitleXOffset(1.2);
//  gStyle->SetTitleYOffset(1.2);

  // use large fonts
  //Int_t font=72; // Helvetica italics
  Int_t font=42; // Helvetica
  Double_t tsize=text_size;
  gStyle->SetTextFont(font);

  gStyle->SetTextSize(tsize);
  gStyle->SetLabelFont(font,"x");
  gStyle->SetTitleFont(font,"x");
  gStyle->SetLabelFont(font,"y");
  gStyle->SetTitleFont(font,"y");
  gStyle->SetLabelFont(font,"z");
  gStyle->SetTitleFont(font,"z");
  
  gStyle->SetLabelSize(tsize,"x");
  gStyle->SetTitleSize(tsize,"x");
  gStyle->SetLabelSize(tsize,"y");
  gStyle->SetTitleSize(tsize,"y");
  gStyle->SetLabelSize(tsize,"z");
  gStyle->SetTitleSize(tsize,"z");

  // use bold lines and markers
//  gStyle->SetMarkerStyle(20);
//  gStyle->SetMarkerSize(1.2);
  gStyle->SetHistLineWidth(2.);
  gStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // get rid of X error bars (as recommended in ATLAS figure guidelines)
  gStyle->SetErrorX(0.0001);
  // get rid of error bar caps
  gStyle->SetEndErrorSize(0.);

  // do not display any of the standard histogram decorations
  gStyle->SetOptTitle(0);
  //gStyle->SetOptStat(1111);
  gStyle->SetOptStat(0);
  //gStyle->SetOptFit(1111);
  gStyle->SetOptFit(0);

  // put tick marks on top and RHS of plots
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);


}
