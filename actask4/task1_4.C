// 有一个需要注意的点：在ROOT中，直方图默认与当前目录关联（例如TFile）。当关闭TFile时，与其关联的所有直方图都会被删除，如果之后需要调用的话就会产生无效的内存访问。


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

    std::ofstream outputFile("chi2Ndf_and_gainCoefficients.txt");

    TH2D *gainMap = new TH2D("gainMap", "GainCoefficients; A; B", 192, 0, 192, 6, 0, 6);
    TH2D *chi2Map = new TH2D("chi2Map", "ChiSquared/NDF; A; B", 192, 0, 192, 6, 0, 6);

    double totalMPV = 0;
    int numDetectors = 0;

    std::vector<double> mpvList; // 存储每个探测器的MPV
    std::vector<TH1D*> correctedHists; // 用于存储输出的值方图
    std::vector<TH1D*> originalHists; // 由于出现了程序崩溃，存储一个原始直方图

    for (int A=0; A < 192; ++A){
        for (int B=0; B < 6; ++B){
            TString histName = TString::Format("%d_%d", A, B);
            TH1D *hist = (TH1D*)file -> Get(histName);

            TH1D *histClone = (TH1D*)hist -> Clone();
            histClone -> SetDirectory(0); // 克隆并解除关联
            originalHists.push_back((TH1D*)hist -> Clone());

            TF1 *landauGausFit = new TF1("landauGausFit", langaufun, 35, 150, 4);
            landauGausFit -> SetParameters(1.8, 50, 50000, 3);
            landauGausFit -> SetParNames("Width", "MP", "Area", "Sigma");

            landauGausFit -> SetNpx(1000); // 由于初步拟合时发现峰值附近的拟合函数看起来太尖锐 

            hist -> Fit(landauGausFit, "Q");

            double gainCoefficient = landauGausFit -> GetMaximumX(35, 150);

            double chi2 = landauGausFit -> GetChisquare();
            int ndf = landauGausFit -> GetNDF();
            double chi2NDF = chi2 / ndf;

            gainMap -> SetBinContent(A + 1, B + 1, gainCoefficient);
            chi2Map -> SetBinContent(A + 1, B + 1, chi2NDF);

            outputFile << A << "_" << B << " " << chi2NDF << " " << gainCoefficient << "\n";

            mpvList.push_back(gainCoefficient);
            totalMPV += gainCoefficient;
            numDetectors ++;

            hist -> Write();

        }
    }

    double targetMPV = totalMPV / numDetectors;

    for (int A = 0; A < 192; ++A){
        for (int B = 0; B < 6; ++B){

            TH1D *hist = originalHists[A * 6 + B];
            double gainCoefficient = mpvList[A * 6 + B];
            double correctionFactor = targetMPV / gainCoefficient;

            TH1D * correctedHist = (TH1D*)hist -> Clone(TString::Format("corrected_%d_%d", A, B));
            correctedHist -> SetDirectory(0);
            correctedHist -> Scale(correctionFactor);
            correctedHists.push_back(correctedHist);

            correctedHist -> Write();
        }
    }

    TH1D *mergedHist = (TH1D*)correctedHists[0] -> Clone("merged_hist");
    mergedHist -> Reset();
    mergedHist -> SetDirectory(0);

    for (size_t i = 0; i < correctedHists.size(); ++i){
        mergedHist -> Add(correctedHists[i]);
    }

    mergedHist -> Write();
    gainMap -> Write();
    chi2Map -> Write();
    
    file -> Close();
    outFile -> Close();
    outputFile.close();

    
    TFile *inFile = TFile::Open("fit_results.root", "READ");

    TH2D *gainHist = (TH2D*)inFile -> Get("gainMap");
    TH2D *chi2Hist = (TH2D*)inFile -> Get("chi2Map");
    TH1D *mergedHistFromFile = (TH1D*)inFile -> Get("merged_hist");

    TCanvas *c1 = new TCanvas("c1", "Gain Coefficient Heatmap", 800, 600);
    gStyle -> SetOptStat(0);
    gainHist -> Draw("COLZ");

    c1 -> SaveAs("gain_heatmap.png");

    TCanvas *c2 = new TCanvas("c2", "ChiSquared/NDF Heatmap", 800, 600);
    chi2Hist -> Draw("COLZ");

    c2 -> SaveAs("chi2Ndf_heatmap.png");

    TCanvas *c3 = new TCanvas("c3", "Merged Spectrum", 800, 600);
    mergedHistFromFile -> Draw();
    
    c3 -> SaveAs("merged_spectrum.png");

    inFile -> Close(0);
}