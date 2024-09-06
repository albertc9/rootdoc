#include "/home/albert/rootdoc/actask3/style.h"

void task1_3(){
    
    MyStyle(); // 头文件中的内容

    TFile *file = TFile::Open("result.root"); // 打开 result.root
    TH2D *r_strip = (TH2D*)file ->Get("r_strip"); // 获取 r_strip

    TCanvas *c0 = new TCanvas("c0", "r_strip output", 600, 800);
    r_strip -> Draw();

    gStyle -> SetOptStat(1111);
    r_strip -> SetStats(1);

    c0 -> SaveAs("r_strip output.png");
    delete c0;

    TH1D *h1 = r_strip -> ProjectionY("ShouldDelete", 1, 5); // 从第1个bin到第5个bin进行投影

    h1 -> Fit("gaus");
    TF1 *fitResult = h1 -> GetFunction("gaus");
    // fitResult -> SetLineColor(kBlue);

    h1 -> Fit("landau");
    TF1 *fitResultLandau = h1 -> GetFunction("landau");
    // fitResultLandau -> SetLineColor(kRed);


    // 创建 root 文件, 后续再输出图像文件
    TFile *outFile = new TFile("output1_3.root", "RECREATE"); 
    h1 -> Write();
    fitResult -> Write("gauseFit");
    fitResultLandau -> Write("landauFit");
    outFile -> Close();
    file -> Close();


    // 接下来通过 output1_3.root 生成输出的图片
    TFile *inFile = TFile::Open("output1_3.root");

    TH1D *h2 = (TH1D*)inFile -> Get("ShouldDelete");
    TF1 *fitResult2 = (TF1*)inFile -> Get("gauseFit");
    TF1 *fitResultLandau2 = (TF1*)inFile -> Get("landauFit");

    fitResult2 -> SetLineColor(kBlue);
    fitResultLandau2 -> SetLineColor(kRed);
    fitResult2 -> Draw("same");
    fitResultLandau2 -> Draw("same");

    TCanvas *c1 = new TCanvas("c1", "Histogram", 800, 600);
    h2 -> GetXaxis() -> SetLabelSize(0.04);
    h2 -> GetYaxis() -> SetLabelSize(0.04);
    h2 -> Draw();

    gStyle-> SetOptStat("eMRSK");
    gStyle-> SetOptFit(0111);
    h2 -> SetStats(1);

    TPaveStats *stats = (TPaveStats*)h2 -> FindObject("stats");
    if (stats){
        stats -> SetX1NDC(0.65);
        stats -> SetY1NDC(0.65);
        stats -> SetX2NDC(0.9);
        stats -> SetY2NDC(0.9);
    }
    TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend -> AddEntry(fitResult2, "Gaussian Fit", "l");
    legend -> AddEntry(fitResultLandau2, "Landau Fit", "l");

    TLatex *title = new TLatex();
    title -> SetTextAlign(22);
    title -> SetTextSize(0.033);
    title -> DrawLatexNDC(0.5, 0.047, "First 5-bin Y-projection and Fitting of Particle Signal Distribution in a 300 #mum Silicon Strip Detector"); 

    c1 -> SaveAs("task1_3_output.png");

    delete c1;
    inFile -> Close();
    delete inFile;

}
