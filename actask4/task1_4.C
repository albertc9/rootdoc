void task1_4() {
    TFile *file = TFile::Open("va.root", "READ");
    TFile *outFile = new TFile("fit_results.root","RECREATE");

    TH2D *gainMap = new TH2D("gainMap", "GainCoefficients; A; B", 192, 0, 192, 6, 0, 6);
    for (int A=0; A < 192; ++A){
        for (int B=0; B < 6; ++B){
            TString histName = TString::Format("%d_%d", A, B);
            TH1D *hist = (TH1D*)file -> Get(histName);

            TF1Convolution *fconv = new TF1Convolution("landau", "gaus", 35, 150, true); // true 应该是用来归一化的
            fconv -> SetNofPointsFFT(1000);

            TF1 *landauGausFit = new TF1("landauGausFit", fconv, 35, 150, fconv -> GetNpar());
            landauGausFit -> SetParameters(1000, 50, 5);
            landauGausFit -> SetParNames("Constant", "MPV", "Sigma");

            hist -> Fit(landauGausFit, "Q");

            double gainCoefficient = landauGausFit -> GetParameter("MPV");

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