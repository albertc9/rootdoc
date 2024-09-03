#include <TCanvas.h>
#include <TH1D.h>
#include <TRandom.h>
#include <TLegend.h>

// 定义生成样本均值分布的函数
void generateAndDrawMeans(int n, int numExperiments, TH1D* hist) {
    TRandom randGen; // 随机数生成器
    for (int i = 0; i < numExperiments; ++i) {
        double sum = 0;
        for (int j = 0; j < n; ++j) {
            sum += randGen.Exp(1); // 生成负指数分布随机变量，平均值为1
        }
        double mean = sum / n;
        hist->Fill(mean); // 将样本均值填入直方图
    }
}

// 主函数
void task1_2() {
    // 创建画布
    TCanvas* canvas = new TCanvas("canvas", "Central Limit Theorem Demonstration", 800, 1000);


    // 定义样本数量和实验次数
    const int numExperiments = 10000;
    int nValues[4] = {100, 1000, 10000, 100000};

    // 创建直方图
    TH1D* histograms[4];
    histograms[0] = new TH1D("hist_10^2", "Sample Means Distribution;Mean;Frequency", 100, 0.6, 1.4);
    histograms[1] = new TH1D("hist_10^3", "Sample Means Distribution;Mean;Frequency", 1000, 0.6, 1.4);
    histograms[2] = new TH1D("hist_10^4", "Sample Means Distribution;Mean;Frequency", 10000, 0.6, 1.4);
    histograms[3] = new TH1D("hist_10^5", "Sample Means Distribution;Mean;Frequency", 50000, 0.6, 1.4);

    // 生成样本均值分布并填充直方图
    for (int i = 0; i < 4; ++i) {
        generateAndDrawMeans(nValues[i], numExperiments, histograms[i]);
    }

    // 设置不同的颜色用于区分曲线
    histograms[0]->SetLineColor(kRed);
    histograms[1]->SetLineColor(kBlue);
    histograms[2]->SetLineColor(kGreen);
    histograms[3]->SetLineColor(kMagenta);
    
    histograms[0]->SetMaximum(500);
    histograms[1]->SetMaximum(500);
    histograms[2]->SetMaximum(500);
    histograms[3]->SetMaximum(500);
    // 绘制直方图
    histograms[0]->Draw("HIST");
    histograms[1]->Draw("HIST SAME");
    histograms[2]->Draw("HIST SAME");
    histograms[3]->Draw("HIST SAME");

    // 添加图例
    TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(histograms[0], "n = 10^2", "l");
    legend->AddEntry(histograms[1], "n = 10^3", "l");
    legend->AddEntry(histograms[2], "n = 10^4", "l");
    legend->AddEntry(histograms[3], "n = 10^5", "l");
    legend->Draw();

    // 显示画布
    canvas->Update();
}