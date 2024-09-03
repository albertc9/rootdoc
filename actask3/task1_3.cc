#include "/home/albert/rootdoc/actask3/style.h"

void task1_3(){
    
    MyStyle(); // 头文件中的内容

    TFile *file = TFile::Open("result.root"); // 打开 result.root

    TH2D *r_strip = (TH2D*)file ->Get("r_strip"); // 获取 r_strip

    TH1D *h1 = r_strip -> ProjectionY("h1_projY", 1, 5); // 从第1个bin到第5个bin进行投影

    h1 -> Fit("gaus"); // 假设数据服从高斯分布
    TF1 *fitResult = h1 ->GetFunction("gaus"); // 获得拟合结果

    // 创建 root 文件, 后续再输出图像文件
    TFile *outFile = new TFile("output1_3.root", "RECREATE"); 
    h1 -> Write();
    fitResult -> Write();
    outFile -> Close();
    delete outFile;

    file -> Close();
    delete file;

    // 将直方图数据导出为CSV文件以供Origin使用
    std::ofstream outFileCSV("task1_3_output.csv");
    outFileCSV << "Bin,Value,Error\n";
    for (int i = 1; i <= h1->GetNbinsX(); ++i) {
        outFileCSV << h1->GetBinCenter(i) << ","
                   << h1->GetBinContent(i) << ","
                   << h1->GetBinError(i) << "\n";
    }
    outFileCSV.close();


    // 调试完, 接下来通过 output1_3.root 生成输出的图片
    TFile *inFile = TFile::Open("output1_3.root");

    TH1D *h2 = (TH1D*)inFile -> Get("h1_projY");
    TF1 *fitResult2 = (TF1*)h2 -> GetFunction("gaus");

    TCanvas *c1 = new TCanvas("*c1", "Histogram", 800, 600);

    h2 -> Draw();

    c1 -> SaveAs("task1_3_output.png");

    delete c1;
    inFile -> Close();
    delete inFile;

}
