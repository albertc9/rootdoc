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

    TH1D *h1a = r_strip -> ProjectionY("TemTitle1", 1, 5);
    TH1D *h1b = r_strip -> ProjectionY("TemTitle2", 1, 5);

    h1a -> Fit("gaus", "q");
    TF1 *fitResult = h1a -> GetFunction("gaus");
    fitResult -> SetLineColor(kGreen);

    h1b -> Fit("landau");
    TF1 *fitResultLandau = h1b -> GetFunction("landau");
    fitResultLandau -> SetLineColor(kRed);


    // 创建 root 文件, 后续再输出图像文件
    TFile *outFile = new TFile("output1_3.root", "RECREATE"); 
    h1a -> Write();
    h1b -> Write();
    fitResult -> Write("gauseFit");
    fitResultLandau -> Write("landauFit");
    outFile -> Close();
    file -> Close();


    // 接下来通过 output1_3.root 生成输出的图片
    TFile *inFile = TFile::Open("output1_3.root");

    TH1D *h2 = (TH1D*)inFile -> Get("TemTitle1");
    TF1 *fitResult2 = (TF1*)inFile -> Get("gauseFit");
    TF1 *fitResultLandau2 = (TF1*)inFile -> Get("landauFit");

    TColor *colorGray = new TColor(2000, 145/255.0, 146/255.0, 171/255.0);
    TColor *colorBlue = new TColor(2001, 99/255.0, 178/255.0, 238/255.0);
    TColor *colorRed = new TColor(2002, 248/255.0, 149/255.0, 136/255.0);
    h2 -> SetLineColor(2000);
    fitResult2 -> SetLineColor(2001);
    fitResultLandau2 -> SetLineColor(2002);
    

    TCanvas *c1 = new TCanvas("c1", "Histogram", 800, 600);
    h2 -> GetXaxis() -> SetLabelSize(0.04);
    h2 -> GetYaxis() -> SetLabelSize(0.04);
    h2 -> Draw();
    fitResult2 -> Draw("same");
    fitResultLandau2 -> Draw("same");

    TLegend *legend = new TLegend(0.67, 0.5, 0.92, 0.63);
    legend -> AddEntry(h2, "Origin Data", "l");
    legend -> AddEntry(fitResult2, "Gaussian Fit", "l");
    legend -> AddEntry(fitResultLandau2, "Landau Fit", "l");
    legend -> SetTextSize(0.03);
    legend -> Draw();

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

    TLatex *title = new TLatex();
    title -> SetTextAlign(22);
    title -> SetTextSize(0.032);
    title -> DrawLatexNDC(0.495, 0.047, "Fig 1. First 5-bin Y-projection and Fitting of Particle Signal Distribution in a 300 #mum Silicon Strip Detector"); 
    gPad -> Update();

    c1 -> SaveAs("task1_3_output.png");
    c1 -> SaveAs("task1_3_output.pdf");

    delete c1;
    inFile -> Close();
    delete inFile;

}
