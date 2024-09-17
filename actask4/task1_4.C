gSystem -> Load("libMathCore");

// 根据指导网页 https://root.cern/doc/master/langaus_8C.html 中的教程，定义朗道卷积高斯函数
double langaufun(double *x, double *par) {

   // Fit parameters:
   // par[0]=Width (scale) parameter of Landau density
   // par[1]=Most Probable (MP, location) parameter of Landau density
   // par[2]=Total area (integral -inf to inf, normalization constant)
   // par[3]=Width (sigma) of convoluted Gaussian function

   // Numeric constants
   double invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
   double mpshift  = -0.22278298;       // Landau maximum location

   // Control constants
   double np = 100.0;      // number of convolution steps
   double sc = 5.0;      // convolution extends to +-sc Gaussian sigmas

   // Variables
    double xx;
    double mpc;
    double fland;
    double sum = 0.0;
    double xlow,xupp;
    double step;
    double i;

   // MP shift correction
   mpc = par[1] - mpshift * par[0];

   // Range of convolution integral
   xlow = x[0] - sc * par[3];
   xupp = x[0] + sc * par[3];
   
   step = (xupp-xlow) / np;

   // Convolution integral of Landau and Gaussian by sum
   for(i=1.0; i<=np/2; i++) {
      xx = xlow + (i-.5) * step;
      fland = TMath::Landau(xx,mpc,par[0]) / par[0];
      sum += fland * TMath::Gaus(x[0],xx,par[3]);
 
      xx = xupp - (i-.5) * step;
      fland = TMath::Landau(xx,mpc,par[0]) / par[0];
      sum += fland * TMath::Gaus(x[0],xx,par[3]);
   }
 
   return (par[2] * step * sum * invsq2pi / par[3]);
}


void task1_4() {
    TVirtualFitter::SetDefaultFitter("Minuit");
    TFile *file = TFile::Open("va.root", "READ");
    TFile *outFile = new TFile("fit_results.root","RECREATE");

    TH2D *gainMap = new TH2D("gainMap", "GainCoefficients; A; B", 192, 0, 192, 6, 0, 6);
    for (int A=0; A < 192; ++A){
        for (int B=0; B < 6; ++B){
            TString histName = TString::Format("%d_%d", A, B);
            TH1D *hist = (TH1D*)file -> Get(histName);

            TF1 *landauGausFit = new TF1("landauGausFit", langaufun, 35, 150, 4);
            landauGausFit -> SetParameters(1.8, 50, 50000, 3);
            landauGausFit -> SetParNames("Width", "MP", "Area", "Sigma"); 

            hist -> Fit(landauGausFit, "Q");

            double gainCoefficient = landauGausFit -> GetParameter(1);

            gainMap -> SetBinContent(A + 1, B + 1, gainCoefficient);

            hist -> Write();
            
        }
    }

    gainMap -> Write();
    
    file -> Close();
    outFile -> Close();

    
    TFile *inFile = TFile::Open("fit_results.root", "READ");

    TH2D *gainHist = (TH2D*)inFile -> Get("gainMap");

    TCanvas *c1 = new TCanvas("c1", "Gain Coefficient Heatmap", 800, 600);
    gStyle -> SetOptStat(0);
    gainHist -> Draw("COLZ");

    c1 -> SaveAs("gain_heatmap.png");

    inFile -> Close(0);
    delete c1;
}