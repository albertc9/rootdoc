/* **NOTES**
* 代码的主要内容和上一次类似，但是重新整理了内容。
*/

#include "langaufun.C"

void ftask1_6(){
    // **CLOCK**
<<<<<<< Tabnine <<<<<<<
    /**//+
     * @brief This function performs a series of operations on a set of histograms and fits.//+
     *        It reads histograms from a file, performs a Landau-Gaussian fit on each, calculates//+
     *        gain coefficients, aligns the histograms, and fits the merged histogram.//+
     *        The function also writes the results to a new file and generates various plots.//+
     *//+
     * @param void//+
     *//+
     * @return void//+
     *///+
    void ftask1_6() {//+
        // **CLOCK**//+
        auto start = std::chrono::high_resolution_clock::now();//+
    //+
        // "PREWORK"//+
        TVirtualFitter::SetDefaultFitter("Minuit");//+
    //+
        // **VARIABLE DEFINITION**//+
        int A = 0, B = 0; // A_B//+
        double totalMPV = 0; // Accumulated MPV//+
        int numDetectors = 0; // Number of detectors//+
        int lefFit = 45, riFit = 45;//+
        int lefMergedFit = 45, riMergedFit = 75;//+
        double gainCoefficient = 0;//+
        double chi2 = 0;//+
        int ndf = 0;//+
        double observed = 0;//+
        double expected = 0;//+
        int bin = 0;//+
        double chi2NDF = chi2 / ndf;//+
    //+
        TString histName = TString::Format("%d_%d", A, B);//+
    //+
        // **CLASSES DEFINITION**//+
        TFile *file = TFile::Open("va.root", "READ");//+
        TFile *outFile = new TFile("fit_result.root", "WRITE");//+
        TH1D *hist = (TH1D*)file->Get(histName);//+
        TH1D *histClone = (TH1D*)hist->Clone();//+
            histClone->SetDirectory(0);//+
    //+
        std::ofstream outputFile("chi2Ndf_and_gainCoefficient.txt");//+
        std::vector<double> mpvList;//+
        std::vector<TH1D*> correctedHists;//+
        std::vector<TH1D*> originalHists;//+
            originalHists.push_back((TH1D*)hist->Clone());//+
    //+
        // **START**//+
        // ... (remaining code)//+
    }//+
>>>>>>> Tabnine >>>>>>>// {"conversationId":"e97b3506-db9d-4f6b-9438-d830d63dbb15","source":"instruct"}

    auto start = std::chrone::high_resolution_clock()::now;

    // "PREWORK"
    TVirtualFitter::SetDefaultFitter("Minuit");

    

    // **VARIABLE DEFINITION**
    int A = 0, B = 0; // A_B
    double totalMPV = 0; // 累加MPV
    int numDetectors = 0; // 探测器数量
    int lefFit = 45, riFit = 45;
    int lefMergedFit = 45, riMergedFit = 75;
    double gainCoefficient = 0;
    double chi2 = 0;
    int ndf = 0;
    double observed = 0;
    double expected = 0;
    int bin = 0;
    double chi2NDF = chi2 / ndf;
    

    TString histName = TString::Format("%d_%d", A, B);

    // **CLASSES DEFINITION**
    TFile *file = TFile::Open("va.root", "READ");
    TFile *outFile = new TFile*("fit_result.root")
    TH1D *hist = (TH1D*)file -> Get(histName);
    TH1D *histClone = (TH1D*)hist -> Clone();
        histClone -> SetDirectory(0);
    
    std::ofstream outputFile("chi2Ndf_and_gainCoefficient.txt");
    std::vector<double> mpvList;
    std::vector<TH1D*> correctedHists;
    std::vector<TH1D*> originalHists;
        originalHists.push_back((TH1D*)hist -> Clone());
    // **START**

    for (A = 0; A < 192; ++A){
        for (B = 0; B < 192; ++B){
            histClone -> Sumw2();
            TF1 *landauGausFit = new TF1("landauGausFit", langaufun, lefFit, riFit, 4);
                landauGausFit -> SetParameters(1.8, 50, 50000, 3);
                landauGausFit -> SetParNames("Width", "MPV", "Area", "Sigma");
                landauGausFit -> SetNpx(1000);
            
            hist -> Fit(landauGausFit, "Q");
            
            gainCoefficient = landauGausFit -> GetMaximumX(lefFit, riFit);

            for (bin = hist -> FindBin(45); bin <= hist -> FindBin(90); bin ++){
                observed = hist -> GetBinContent(bin);
                expected = landauGausFit -> EVal(hist -> GetBinCenter(bin));
                if (expected > 0){
                    chi2 += (observed - expected)/ expected;
                    ndf++;
                }
            }
            ndf -=4;

            outputFile << "A_B" << " " << "chi2NDF" << " " << "gainCoefficient" << "\n";
            outputFile << A << "_" << B << " " << chi2NDF << " " << gainCoefficient << "\n";

            mpvList.push_back(gainCoefficient); 
            totalMPV += gainCoefficient;
            numDetectors ++;

            hist -> Write();
        }
    }

     double targetMPV = totalMPV / numDetectors; // 计算平均值用来对齐

    for (int A = 0; A < 192; ++A){
        for (int B = 0; B < 6; ++B){

            TH1D *hist = originalHists[A * 6 + B];
            double gainCoefficient = mpvList[A * 6 + B];
            double correctionFactor = targetMPV / gainCoefficient; // 定义一个修正（对齐）因子

            TH1D * correctedHist = (TH1D*)hist -> Clone(TString::Format("corrected_%d_%d", A, B));
            correctedHist -> SetDirectory(0); // 也是解除关联
            correctedHist -> Sumw2();
            correctedHist -> Scale(correctionFactor);
            correctedHists.push_back(correctedHist);

            correctedHist -> Write();
        }
    }

    TH1D *mergedHist = (TH1D*)correctedHists[0] -> Clone("merged_hist");
    mergedHist -> Reset();
    mergedHist -> SetDirectory(0); 
    mergedHist -> Sumw2(); 

    for (size_t i = 0; i < correctedHists.size(); ++i){
        mergedHist -> Add(correctedHists[i]);
    }

    double bestChi2NDF = 1e6;
    int bestLefFit = 40, bestRiFit = 75;

    for (double lower = 40; lower <= 48; lower += 0.5) {
    for (double upper = 60; upper <= 75; upper += 0.5) {

    TF1 *tempFit = new TF1("tempFit", langaufun, lower, upper, 4);
    tempFit->SetParameters(1.8, 50, 50000, 3);
    tempFit->SetParNames("Width", "MP", "Area", "Sigma");
    tempFit->SetNpx(1000);

    mergedHist->Fit(tempFit, "Q");

    double tempChi2 = 0;
    int tempNDF = 0;
    for (int bin = mergedHist->FindBin(45); bin <= mergedHist->FindBin(80); bin++) {
        double observed = mergedHist->GetBinContent(bin);
        double expected = tempFit->Eval(mergedHist->GetBinCenter(bin));
        if (expected > 0) {
            tempChi2 += (observed - expected) * (observed - expected) / expected;
            tempNDF++;
        }
    }
    tempNDF -= 4;
    double tempChi2NDF = tempChi2 / tempNDF;

    // 判断是否为最优
    if (tempChi2NDF < bestChi2NDF) {
        bestChi2NDF = tempChi2NDF;
        bestLefFit = lower;
        bestRiFit = upper;
    }

    delete tempFit;
    }
    }

    TF1 *mergedFit = new TF1("mergedFit", langaufun, bestLefFit, bestRiFit, 4);
    mergedFit -> SetParameters(1.8, 50, 50000, 3);
    mergedFit -> SetParNames("Width", "MP", "Area", "Sigma");
    mergedFit -> SetNpx(1000);
    mergedHist -> Fit(mergedFit, "Q");

    double mergedChi2 = 0;
    int mergedNDF = 0;
    for (int bin = mergedHist -> FindBin(45); bin <= mergedHist -> FindBin(80);bin ++){
        double observed = mergedHist -> GetBinContent(bin);
        double expected = mergedFit -> Eval(mergedHist -> GetBinCenter(bin));
        if (expected > 0){
            mergedChi2 += (observed - expected) * (observed - expected) / expected;
            mergedNDF++;
        }
    }
    mergedNDF -= 4;
    
    double mergedChi2NDF = mergedChi2 / mergedNDF;

    mergedFit -> Write();
    mergedHist -> Write();
    gainMap -> Write();
    chi2Map -> Write();


    // **CLOSE FILES**
    file -> Close();
    outFile -> Close();
    outputFile.close();

    // **READ ROOT FILE**

    TFile *inFile = TFile::Open("fit_results.root", "READ");

    TH2D *gainHist = (TH2D*)inFile -> Get("gainMap"); // 增益系数热力图
    TH2D *chi2Hist = (TH2D*)inFile -> Get("chi2Map"); // 卡方分布热力图
    TH1D *mergedHistFromFile = (TH1D*)inFile -> Get("merged_hist"); // 合并后的直方图
    TF1 *mergedFitFromFile = (TF1*)inFile -> Get("mergedFit"); // 拟合后的合并后的直方图
    TH1D *gainHist1D = new TH1D("gainHist1D", "Gain Coefficients; Detector ID; Gain Coefficient", 1152, 1, 1153); // 1152 bin的直方图，用于绘制增益系数

    TCanvas *c1 = new TCanvas("c1", "Gain Coefficient Heatmap", 800, 600);
    gStyle -> SetOptStat(0);
    gainHist -> Draw("COLZ");

    c1 -> SaveAs("gain_heatmap.pdf");

    TCanvas *c2 = new TCanvas("c2", "ChiSquared/NDF Heatmap", 800, 600);
    gStyle -> SetOptStat(0);
    chi2Hist -> Draw("COLZ");

    c2 -> SaveAs("chi2Ndf_heatmap.pdf");

    TCanvas *c3 = new TCanvas("c3", "Merged Spectrum", 800, 600);
    mergedHistFromFile -> Draw("HIST");
    
    // 这里的 merged_spectrum 实际上是对每个原始直方图进行郎道卷积高斯函数拟合，得到MPV，计算平均MPV后以 correctionFactor = targetMPV / gainCoefficient 作为校正因子对齐得到的合并的直方图
    c3 -> SaveAs("merged_spectrum.pdf");

    TCanvas *c4 = new TCanvas("c4", "Fitted Merged Spectrum", 800, 600);
    gStyle -> SetOptFit(1111); 
    mergedHistFromFile -> Draw("E1Y0");
    mergedFitFromFile -> SetLineColor(kRed);
    mergedFitFromFile -> Draw("SAME");

    gPad -> Update();
    TPaveStats *stats = (TPaveStats*)mergedHistFromFile -> FindObject("stats");
    if (stats){
    stats -> SetX1NDC(0.65);
    stats -> SetY1NDC(0.65);
    stats -> SetX2NDC(0.90);
    stats -> SetY2NDC(0.90);
    stats -> SetBorderSize(1);
    stats -> AddText(TString::Format("Chi2/NDF = %.2f", mergedChi2NDF));
    }

    c4 -> SaveAs("fitted_merged_spectrum.pdf");  
    TCanvas *c5 = new TCanvas("c5", "Gain Coefficients 2D plot", 800, 600);
    for (int i = 0; i < 1152; ++i){
        gainHist1D -> SetBinContent(i+1, mpvList[i]);
    }
    gainHist1D -> Draw("HIST");

    c5 -> SaveAs("gain_coefficients_1D_plot.pdf");

    inFile -> Close(0);
    c1 -> Close(0);
    c2 -> Close(0);
    c3 -> Close(0);
    c4 -> Close(0);
    c5 -> Close(0);

    // **OUTPUT**
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> allTime = end - start;
    std::cout << "Total execution time: " << allTime.count() << " seconds." << std::endl;
    std::cout << "Attention: All Chi2 / NDF are in the range of [45, 80]." << std::endl;
    std::cout << "Total execution time: " << elapsed.count() << " seconds." << std::endl;
}