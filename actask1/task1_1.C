#include <TFile.h>
#include <TH1F.h>
#include <TRandom3.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TPaveStats.h> // 添加此行以使用TPaveStats
#include <iostream>

void generate_and_fit_gaussian(int N, int nBins) {
    TRandom3 rnd(0);
    TH1F *hist = new TH1F("hist", Form("Gaussian distribution (N = %d, nBins = %d)", N, nBins), nBins, 0, 100);
    for (int i = 0; i < N; ++i) {
        double value = rnd.Gaus(50, 10);
        hist->Fill(value);
    }

    TF1 *gausFit = new TF1("gausFit", "gaus", 0, 100);
    hist->Fit(gausFit, "Q");

    TCanvas *c1 = new TCanvas("c1", "Gaussian Fit", 800, 600);
    hist->Draw();

    // 设置统计框和拟合参数的显示
    gStyle->SetOptStat(1111); // 显示统计信息
    gStyle->SetOptFit(1111);  // 显示拟合参数，包括 chi^2, ndf, 参数值和不确定性

    // 获取统计框对象并设置其位置
    c1->Update(); // 更新画布以确保统计框已经创建
    TPaveStats *stats = (TPaveStats*)hist->FindObject("stats");
    if (stats) {
        stats->SetX1NDC(0.6); 
        stats->SetX2NDC(0.9); 
        stats->SetY1NDC(0.6); 
        stats->SetY2NDC(0.9); 
    }

    c1->SaveAs(Form("/home/albert/rootdoc/ACtask1/GaussianFit_N%d_nBins%d.png", N, nBins));

    TFile *outputFile = new TFile(Form("/home/albert/rootdoc/ACtask1/GaussianData_N%d_nBins%d.root", N, nBins), "RECREATE");
    
    hist->Write();
    outputFile->Close();
}

void task1_1() {
    int Ns[] = {1000, 10000, 100000};
    int binSizes[] = {50, 100, 200};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            generate_and_fit_gaussian(Ns[i], binSizes[j]);
        }
    }
}