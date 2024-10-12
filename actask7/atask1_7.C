/*
* 1. 这块代码整理了1_6及之前关于1152硅微条探测器的代码，只保留了输出校准后的图像、输出的文本文件，以及校准前后图像的对比。
* 2. 在ROOT中，直方图默认与当前目录关联（例如TFile）。当关闭TFile时，与其关联的所有直方图都会被删除，如果之后需要调用的话就会产生无效的内存访问。这可能是我在运行中偶尔出现 crash 的原因。
* 3. 在所有部分，我只计算了区间（45,80）的卡方/自由度。 
*/

#include "langaufun.C"

void atask1_7(){
    auto start = std::chrono::high_resolution_clock::now();

    TVirtualFitter::SetDefaultFitter("Minuit");

    TFile *inFile = TFile::Open("va.root");
    TFile *outFile = new TFile("fit_result.root", "RECREATE");
    std::ofstream outputFile("chi2NDF_and_gainCoefficients.txt");

    double totalMPV = 0;
    int numDetector = 0;

    std::vector<double> mpvList;
    std::vector<TH1D*> correctedHists;
    std::vector<TH1D*> originalHists;

    int lefFit = 45, riFit = 75, lefMergedFit = 45, riMergedFit = 75;

    for (int A = 0; A < 192; ++A){
        for (int B = 0; B < 6; ++B){
            TString histName = TString::Format("%d_%d", A, B);
            TH1D *hist = (TH1D*)inFile -> Get(histName);
            TH1D *histClone = (TH1D*)hist -> Clone();

            hist -> SetDirectory(0);
            histClone -> SetDirectory(0);
            // histClone -> Sumw2();
            originalHists.push_back(histClone);
            originalHists.push_back((TH1D*)hist -> Clone());

            TF1 *landauGausFit = new TF1("landauGausFit", langaufun, lefFit, riFit, 4);
            landauGausFit -> SetParameters(1.8, 50, 50000, 3);
            landauGausFit -> SetRange(lefFit, riFit);
            landauGausFit -> SetParNames("Width", "MPV", "Area", "Sigma");
            landauGausFit -> SetNpx(1000);
            hist -> Fit(landauGausFit, "Q", "", lefFit, riFit);

            double gainCoefficient = landauGausFit -> GetMaximumX(lefFit, riFit);

            double chi2 = 0;
            int ndf = 0;
            for (int bin = hist -> FindBin(45);bin <= hist -> FindBin(90); bin++){
                double observed = hist -> GetBinContent(bin);
                double expected = landauGausFit -> Eval(hist -> GetBinCenter(bin));
                if (expected > 0){
                    chi2 += (observed - expected) * (observed - expected) / expected;
                    ndf++;
                }
            }
            ndf -=4;

            double chi2NDF = chi2 / ndf;

            outputFile << A << "_" << B << " " << chi2NDF << " " << gainCoefficient << "\n";

            mpvList.push_back(gainCoefficient);
            totalMPV += gainCoefficient;
            numDetector ++;

            hist -> Write();
        }
    }

    // ！！！我检查过第一次拟合后的内容，以上部分应该没有独立的问题。完成了对所有直方图的拟合，并且输出到root文件中了


    double targetMPV = totalMPV / numDetector;

    for (int A = 0; A < 192; ++A){
        for (int B = 0; B < 6; ++B){
            TH1D *hist = originalHists[A * 6 + B];
            double gainCoefficient = mpvList[A * 6 + B];
            double correctionFactor = gainCoefficient / targetMPV;

            TH1D *correctedHist = (TH1D*)hist -> Clone(TString::Format("corrected_%d_%d", A, B));
            correctedHist -> SetDirectory(0);

            int nbins = hist -> GetNbinsX();
            double xlow = hist -> GetXaxis() -> GetXmin() * correctionFactor;
            double xup = hist -> GetXaxis() -> GetXmax() * correctionFactor;
            correctedHist -> GetXaxis() -> Set(nbins, xlow, xup);

            // correctedHist -> Sumw2();

            correctedHists.push_back(correctedHist);
            correctedHist -> Write();
        }
    }

// ！！！ 这块对齐应该也没有问题。root文件中的直方图表明已经完成了缩放（对齐），等待相加

/*
    TH1D *mergedHistUncorrected = nullptr; 
    for (int A = 0; A < 192; ++A) {
        for (int B = 0; B < 6; ++B) {
            TString histName = TString::Format("%d_%d", A, B);
            TH1D *histFromFile = (TH1D*)inFile -> Get(histName);
            
            if (A == 0 && B == 0) { 
                mergedHistUncorrected = (TH1D*)histFromFile -> Clone("merged_hist_uncorrected");
                mergedHistUncorrected->Reset();
                mergedHistUncorrected->SetDirectory(0); // 解除关联
                // mergedHistUncorrected->Sumw2();
            }
            mergedHistUncorrected->Add(histFromFile); // 合并数据
        }
    }

*/

    TH1D *mergedHistUncorrected = nullptr; 

    TString firstHistName = TString::Format("%d_%d", 0, 0);
    TH1D *firstHist = (TH1D*)inFile -> Get(firstHistName);

    if (firstHist) {
        mergedHistUncorrected = (TH1D*)firstHist->Clone("merged_hist_uncorrected");
        mergedHistUncorrected->Reset();
        mergedHistUncorrected->SetDirectory(0);
    }

    for (int A = 0; A < 192; ++A) {
        for (int B = 0; B < 6; ++B) {
            TString histName = TString::Format("%d_%d", A, B);
            TH1D *histFromFile = (TH1D*)inFile->Get(histName);
            
            mergedHistUncorrected -> Add(histFromFile);
        }
    }
        mergedHistUncorrected->Scale(1.0 / 1152);

// ！！！这里应该也没有问题。我原来以为是对齐前的相加出了问题，后来发现是对齐后的相加除了问题，因为fitted_merged_spectrum也随着出现了问题（应该是只显示第一个图像）。以下部分的问题应该是最大的。

/*
    TH1D *mergedHist = (TH1D*)correctedHists[0] -> Clone("merged_hist");
    mergedHist -> Reset();
    mergedHist -> SetDirectory(0);
    // mergedHist -> Sumw2();
    for (size_t i = 0; i < correctedHists.size(); ++i){
        mergedHist -> Add(correctedHists[i]);
        // mergedHistUncorrected -> Add(originalHists[i]);
    }
*/

// 初始化 mergedHist 并克隆 correctedHists[0] 作为合并直方图的结构模板
TH1D *mergedHist = nullptr; 

// 获取 correctedHists 中的第一个直方图

    mergedHist = (TH1D*)correctedHists[0]->Clone("merged_hist");
    mergedHist->Reset();
    mergedHist->SetDirectory(0); 



for (size_t i = 0; i < correctedHists.size(); ++i) {
        mergedHist->Add(correctedHists[i]);

}



    TF1 *mergedFit = new TF1("mergedFit", langaufun, lefMergedFit, riMergedFit, 4);
    mergedFit -> SetParameters(1.8, 50, 50000, 3);
    mergedFit -> SetRange(lefMergedFit, riMergedFit);
    mergedFit -> SetParNames("Width", "MPV", "Area", "Sigma");
    mergedFit -> SetNpx(1000);
    mergedHist -> Fit(mergedFit, "Q", "", lefMergedFit, riMergedFit);

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

    outFile -> cd();
    mergedHistUncorrected -> Write();
    mergedHist -> Write();
    mergedFit -> Write();

    inFile -> Close();
    outFile -> Close();
    outputFile.close();
    
    delete inFile;
    delete outFile;


    TFile *rootFile = TFile::Open("fit_result.root", "READ");

    TH1D *mergedHistUncorrectedFromFile = (TH1D*)rootFile -> Get("merged_hist_uncorrected");
    TH1D *mergedHistFromFile = (TH1D*)rootFile -> Get("merged_hist");
    TF1 *mergedFitFromFile = (TF1*)rootFile -> Get("mergedFit");
    TH1D *gainHist1D = new TH1D("gainHist1D", "Gain Coefficients; Detectors; Detector ID; Gain Coefficient", 1152, 1, 1153);

    TCanvas *c1 = new TCanvas("c1", "Merged Spectrum Comparison", 800, 600);
    mergedHistUncorrectedFromFile -> SetLineColor(kBlue);
    mergedHistFromFile -> SetLineColor(kRed);
    
    mergedHistUncorrectedFromFile -> Draw("HIST");
    double meanUncorrected = mergedHistUncorrectedFromFile -> GetMean();
    double rmsUncorrected = mergedHistUncorrectedFromFile -> GetRMS();
    TPaveText *statsUncorrected = new TPaveText(0.6, 0.75, 0.88, 0.85, "NDC");
    statsUncorrected -> SetFillColor(0);
    statsUncorrected -> AddText("Uncorrected Histogram Stats:");
    statsUncorrected -> AddText(TString::Format("Mean = %.2f", meanUncorrected));
    statsUncorrected -> AddText(TString::Format("RMS = %.2f", rmsUncorrected));
    statsUncorrected -> Draw("SAMS");

    mergedHistFromFile -> Draw("HIST SAME");
    double meanCorrected = mergedHistFromFile -> GetMean();
    double rmsCorrected = mergedHistFromFile -> GetRMS();
    TPaveText *statsCorrected = new TPaveText(0.6, 0.6, 0.88, 0.7, "NDC");
    statsCorrected -> SetFillColor(0);
    statsCorrected -> AddText("Corrected Histogram Stats:");
    statsCorrected -> AddText(TString::Format("Mean = %.2f", meanCorrected));
    statsCorrected -> AddText(TString::Format("RMS = %.2f", rmsUncorrected));
    statsCorrected -> Draw("SAME");

    c1 -> SaveAs("merged_spectrum_comparison.pdf");

    TCanvas *c2 = new TCanvas("c2", "Fitted Merged Spectrum", 800, 600);
    gStyle -> SetOptFit(1111); 
    mergedHistFromFile -> Draw("E1Y0");
    mergedFitFromFile -> SetLineColor(kBlue);
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

    c2 -> SaveAs("fitted_merged_spectrum.pdf");

    TCanvas *c3 = new TCanvas("c5", "Gain Coefficients 2D plot", 800, 600);
    for (int i = 0; i < 1152; ++i){
        gainHist1D -> SetBinContent(i+1, mpvList[i]);
    }
    gainHist1D -> Draw("HIST");

    c3 -> SaveAs("gain_coefficients_1D_plot.pdf");

    rootFile -> Close(0);
    c1 -> Close(0);
    c2 -> Close(0);
    c3 -> Close(0);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Total Execution Time: " << elapsed.count() << " second." << std::endl;
}