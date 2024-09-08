#include "/home/albert/rootdoc/actask3/style.h"

void task1_3(){
    
    MyStyle(); // 头文件中的内容

    TFile *file = TFile::Open("result.root"); // 打开 result.root
    TH2D *r_strip = (TH2D*)file ->Get("r_strip"); // 获取 r_strip

    TCanvas *c0 = new TCanvas("c0", "r_strip_output", 600, 800);
    r_strip -> Draw();

    gStyle -> SetOptStat(1111);
    r_strip -> SetStats(1);

    c0 -> SaveAs("r_strip_output.png");
    delete c0;

    TH1D *h1a = r_strip -> ProjectionY("TemTitle1", 1, 5);
    TH1D *h1b = r_strip -> ProjectionY("TemTitle2", 1, 5);
    TH1D *h1c = r_strip -> ProjectionY("TemTitle3", 1, 5);

    /*
    * 调用h1a -> Fit("vavilov", "R");之后，ROOT会创建一个新的TF1对象并附加到h1a中。因此，需要使用h1a -> GetFunction("vavilov")来获取这个新的拟合对象。
	* 修改后的代码首先执行拟合操作，然后通过GetFunction()获取实际的拟合对象，再对这个对象设置参数名和其他属性。
    */

    TF1 *fitResultVavilov = new TF1("vavilov", "[0]*TMath::Gaus(x, [1], [2]) + [3]*TMath::Landau(x, [4], [5])", 0, 200); 
    fitResultVavilov -> SetParameters(1, 20, 5, 1, 10, 2);  // 初始参数
    h1a -> Fit("vavilov", "R"); // Vavilov分布拟合

    // 获取实际的拟合函数对象
    TF1 *fitResultVavilovFitted = h1a -> GetFunction("vavilov");
    fitResultVavilovFitted -> SetParName(0, "Gauss_Amplitude");
    fitResultVavilovFitted -> SetParName(1, "Gauss_Mean");
    fitResultVavilovFitted -> SetParName(2, "Gauss_Sigma");
    fitResultVavilovFitted -> SetParName(3, "Landau_Amplitude");
    fitResultVavilovFitted -> SetParName(4, "Landau_Mean");
    fitResultVavilovFitted -> SetParName(5, "Landau_Sigma");
    fitResultVavilovFitted -> SetLineColor(kBlue);

    h1b -> Fit("landau");
    TF1 *fitResultLandau = h1b -> GetFunction("landau");
    fitResultLandau -> SetLineColor(kRed);

    h1c -> Fit("gaus", "q");
    TF1 *fitResult = h1c -> GetFunction("gaus");
    fitResult -> SetLineColor(kGreen);


    // 创建 root 文件, 后续再输出图像文件
    TFile *outFile = new TFile("output1_3.root", "RECREATE"); 
    h1a -> Write();
    h1b -> Write();
    h1c -> Write();
    fitResult -> Write("gauseFit");
    fitResultLandau -> Write("landauFit");
    fitResultVavilov -> Write("vavilovFit");
    outFile -> Close();
    file -> Close();


    // 接下来通过 output1_3.root 生成输出的图片
    TFile *inFile = TFile::Open("output1_3.root");

    TH1D *h2 = (TH1D*)inFile -> Get("TemTitle1");
    TF1 *fitResult2 = (TF1*)inFile -> Get("gauseFit");
    TF1 *fitResultLandau2 = (TF1*)inFile -> Get("landauFit");
    TF1 *fitResultVavilov2 = (TF1*)inFile -> Get("vavilovFit");

    TColor *colorGray = new TColor(2000, 145/255.0, 146/255.0, 171/255.0);
    TColor *colorBlue = new TColor(2001, 99/255.0, 178/255.0, 238/255.0);
    TColor *colorRed = new TColor(2002, 248/255.0, 149/255.0, 136/255.0);
    TColor *colorGreen = new TColor(2003, 118/255.0, 218/255.0, 145/255.0);
    h2 -> SetLineColor(2000);
    fitResult2 -> SetLineColor(2001);
    fitResultLandau2 -> SetLineColor(2002);
    fitResultVavilov2 -> SetLineColor(2003);
    
    TCanvas *c1 = new TCanvas("c1", "Histogram", 800, 600);
    h2 -> GetXaxis() -> SetLabelSize(0.04);
    h2 -> GetYaxis() -> SetLabelSize(0.04);
    h2 -> Draw();
    fitResult2 -> Draw("same");
    fitResultLandau2 -> Draw("same");
    fitResultVavilov2 -> Draw("same");

    TLegend *legend = new TLegend(0.67, 0.5, 0.92, 0.63);
    legend -> AddEntry(h2, "Origin Data", "l");
    legend -> AddEntry(fitResult2, "Gaussian Fit", "l");
    legend -> AddEntry(fitResultLandau2, "Landau Fit", "l");
    legend -> AddEntry(fitResultVavilov2, "Vavilov Fit", "l");
    legend -> SetTextSize(0.03);
    legend -> Draw();

    gStyle-> SetOptStat("eMRSK");
    gStyle-> SetOptFit();
    h2 -> SetStats(1);

    TPaveStats *stats = (TPaveStats*)h2 -> FindObject("stats");
        stats -> SetX1NDC(0.65);
        stats -> SetY1NDC(0.65);
        stats -> SetX2NDC(0.9);
        stats -> SetY2NDC(0.9);

    TLatex *title = new TLatex();
    title -> SetTextAlign(22);
    title -> SetTextSize(0.032);
    title -> DrawLatexNDC(0.495, 0.047, "Fig 1. First 5-bin Y-projection and Fitting of Particle Signal Distribution in a 300 #mum Silicon Strip Detector"); 
    gPad -> Update();

    c1 -> SaveAs("task1_3_output.png");
    c1 -> SaveAs("task1_3_output.pdf");


    inFile -> Close();
    delete inFile;

    // 创建并打开txt文件，准备写入统计值
    ofstream outfile("fit_statistics.txt");

    outfile << "Gaussian Fit Results:\n";
    outfile << "Amplitude: " << fitResult2->GetParameter(0) << " ± " << fitResult2->GetParError(0) << "\n";
    outfile << "Mean: " << fitResult2->GetParameter(1) << " ± " << fitResult2->GetParError(1) << "\n";
    outfile << "Sigma: " << fitResult2->GetParameter(2) << " ± " << fitResult2->GetParError(2) << "\n";
    outfile << "Chi-square: " << fitResult2->GetChisquare() << "\n";
    outfile << "NDF: " << fitResult2->GetNDF() << "\n\n";

    outfile << "Landau Fit Results:\n";
    outfile << "Amplitude: " << fitResultLandau2->GetParameter(0) << " ± " << fitResultLandau2->GetParError(0) << "\n";
    outfile << "MPV: " << fitResultLandau2->GetParameter(1) << " ± " << fitResultLandau2->GetParError(1) << "\n";
    outfile << "Sigma: " << fitResultLandau2->GetParameter(2) << " ± " << fitResultLandau2->GetParError(2) << "\n";
    outfile << "Chi-square: " << fitResultLandau2->GetChisquare() << "\n";
    outfile << "NDF: " << fitResultLandau2->GetNDF() << "\n";

    outfile.close();
}
