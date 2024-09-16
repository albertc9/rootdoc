void task1_4() {
    TFile *file = TFile::open("va.root", "READ");
    TFile *outFile = new TFile("fit_results.root","RECREATE");

    TG2D *gainMap = new TH2D("gainMap", "GainCoefficients; A; B", 192, 0, 192, 6, 0, 6);
    for (int A=0; A < 192; ++A){
        for (int B=0; B < 6; ++B){
            TString histName = TString::Format("%d_%d", A, B);

            TH1D *hist = (TH1D*)file -> Get(hisName);

            TF1 *landauGausFit = new TF1("landauGausFit", "landau", hist -> GetrXaxis() -> GetXmin(). hist -> GetXaxis -> GetXmax());

            hist -> Fit(landauGausFit, "Q");

            double gainCoefficient = landauGausFit -> GetParamater(1);

            gainMap -> SetBinContent(A + 1, B + 1, gainCoefficient);

            hist -> Write();
            
        }
    }

    gainMap -> Write();
    
    file -> Close();
    outFile -> Close();


}