z// 有一个需要注意的点：在ROOT中，直方图默认与当前目录关联（例如TFile）。当关闭TFile时，与其关联的所有直方图都会被删除，如果之后需要调用的话就会产生无效的内存访问。这可能是我在运行中偶尔出现 crash 的原因。

gSystem -> Load("libMathCore"); // 似乎有时会产生崩溃，并且提示可能问题出在 libMathCore 上。因此添加这行代码防止崩溃（虽然可能是我电脑的问题）。

// 这部分是根据指导网页 https://root.cern/doc/master/langaus_8C.html 中的教程，定义朗道卷积高斯函数。**好像有直接调用的方法？**
#include "langaufun.C"


void task1_5() {
    TVirtualFitter::SetDefaultFitter("Minuit"); // **在上一次程序运行中发现无法使用 Minuit 改用 Minuit2，这一次发现无法使用 Minuit2 改用 Minuit。由于每个A_B都要提示一次（1152次），于是预先在这里声明。但是为什么？**
    TFile *file = TFile::Open("va.root", "READ");
    TFile *outFile = new TFile("fit_results.root","RECREATE");

    std::ofstream outputFile("chi2Ndf_and_gainCoefficients.txt");

    TH2D *gainMap = new TH2D("gainMap", "GainCoefficients; A; B", 192, 0, 192, 6, 0, 6);
    TH2D *chi2Map = new TH2D("chi2Map", "ChiSquared/NDF; A; B", 192, 0, 192, 6, 0, 6);

    double totalMPV = 0; // 用于累加MPV
    int numDetectors = 0; // 探测器数量

    std::vector<double> mpvList; // 每个探测器的MPV
    std::vector<TH1D*> correctedHists; // 输出的（校正后的）值方图
    std::vector<TH1D*> originalHists; // 由于出现了程序崩溃，存储一个原始直方图。应该是程序开头的注释中提到的问题

    for (int A=0; A < 192; ++A){
        for (int B=0; B < 6; ++B){
            TString histName = TString::Format("%d_%d", A, B);
            TH1D *hist = (TH1D*)file -> Get(histName);

            TH1D *histClone = (TH1D*)hist -> Clone();
            histClone -> SetDirectory(0); // 克隆后解除关联（也是为了防止读取空内存）
            histClone -> Sumw2(); // 计算误差，方便后续误差条的显示（虽然并不明显？）
            originalHists.push_back((TH1D*)hist -> Clone()); // 再次存储一个直方图

            TF1 *landauGausFit = new TF1("landauGausFit", langaufun, 35, 150, 4); // 选择35到150的原因在文章中有说明
            landauGausFit -> SetParameters(1.8, 50, 50000, 3);
            landauGausFit -> SetParNames("Width", "MP", "Area", "Sigma");

            landauGausFit -> SetNpx(1000); // 由于初步拟合时发现峰值附近的拟合函数看起来太尖锐，手动增加绘制精度 

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

    // 这是一个用于合并的直方图
    TH1D *mergedHist = (TH1D*)correctedHists[0] -> Clone("merged_hist");
    mergedHist -> Reset();
    mergedHist -> SetDirectory(0); // 也是解除关联
    mergedHist -> Sumw2(); // 计算误差

    for (size_t i = 0; i < correctedHists.size(); ++i){
        mergedHist -> Add(correctedHists[i]);
    }

    TF1 *mergedFit = new TF1("mergedFit", langaufun, 35, 150, 4);
    mergedFit -> SetParameters(1.8, 50, 50000, 3);
    mergedFit -> SetParNames("Width", "MP", "Area", "Sigma");
    mergedFit -> SetNpx(1000);
    mergedHist -> Fit(mergedFit, "Q");

    mergedFit -> Write();
    mergedHist -> Write();
    gainMap -> Write();
    chi2Map -> Write();
    
    file -> Close();
    outFile -> Close();
    outputFile.close();

    
    TFile *inFile = TFile::Open("fit_results.root", "READ");

    TH2D *gainHist = (TH2D*)inFile -> Get("gainMap"); // 增益系数热力图
    TH2D *chi2Hist = (TH2D*)inFile -> Get("chi2Map"); // 卡方分布热力图
    TH1D *mergedHistFromFile = (TH1D*)inFile -> Get("merged_hist"); // 合并后的直方图
    TF1 *mergedFitFromFile = (TF1*)inFile -> Get("mergedFit"); // 拟合后的合并后的直方图

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
    mergedHistFromFile -> Draw("E1");
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
    }

    c4 -> SaveAs("fitted_merged_spectrum.pdf");

    inFile -> Close(0);
    c1 -> Close(0);
    c2 -> Close(0);
    c3 -> Close(0);
    c4 -> Close(0);
}