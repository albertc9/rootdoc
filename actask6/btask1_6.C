void btask1_6(){
    
    int num = 100;

    TH1D *all = new TH1D("all", "All Entries; Bins; Counts", num, 0, num);
    TH1D *sel = new TH1D("sel", "Selected Entries; Bin; Counts", num, 0, num);

    for (int i = 0; i < num; ++i){
        int raw_temp = int(gRandom -> Gaus(0, 10)) + 200;
        all -> SetBinContent(i + 1, raw_temp);

        int temp = (1 - i * 0.007)* gRandom -> Uniform(0.8, 1.0) * raw_temp;
        sel -> SetBinContent(i + 1, temp);
    }

    TFile *originalFile = new TFile("original_data.root", "RECREATE");
    all -> Write();
    sel -> Write();
    originalFile -> Close();
    delete all;
    delete sel;

    TFile *inputFile = new TFile("original_data.root", "READ");
    TH1D *all_hist = (TH1D*)inputFile -> Get("all");
    TH1D *sel_hist = (TH1D*)inputFile -> Get("sel");
    
    TGraph *eff = new TGraph(num);
    eff -> SetTitle("Efficiency vs Bin; Bin; Efficiency");

    for (int i = 0; i < num; ++i){
        double all_count = all -> GetBinContent(i + 1);
        double sel_count = sel -> GetBinContent(i + 1);
        double efficiency = 0;

    if (all_count > 0){
        efficiency = sel_count / all_count;
    }
    eff -> SetPoint(i, i, efficiency);
}
TCanvas *c1 = new TCanvas("c1", "Histogram Analysis", 800, 600);
double maxVal = TMath::Max(all_hist -> GetMaximum(), sel_hist -> GetMaximum());
double minVal = TMath::Min(all_hist -> GetMinimum(), sel_hist -> GetMinimum());
all_hist -> SetMaximum(maxVal * 1.2);
all_hist -> SetMinimum(minVal * 0.8);
all_hist -> SetLineColor(kBlue);
sel_hist -> SetLineColor(kRed);
all_hist -> Draw("HIST");
sel_hist -> Draw("HIST SAME");

gStyle -> SetOptStat(0);
auto legend = new TLegend(0.75, 0.75, 0.90, 0.90);
legend -> AddEntry(all_hist, "All Entries", "l");
legend -> AddEntry(sel_hist, "Selected Entries", "l");
legend -> Draw();

c1 -> SaveAs("histogram_analysis.pdf");

TCanvas *c2 = new TCanvas("c2", "Efficiency Analysis", 800, 600);
eff -> SetMarkerStyle(20);
eff -> SetMarkerSize(0.8);
eff -> SetMarkerColor(kGreen +2);
eff -> Draw("AP");

c2 -> SaveAs("efficiency_analysis.pdf");

inputFile -> Close();
delete inputFile;
delete eff;
delete c1;
delete c2;
}