/*
* 1. 这块代码整理了1_6及之前关于1152硅微条探测器的代码，只保留了输出校准后的图像、输出的文本文件，以及校准前后图像的对比。
* 2. 在ROOT中，直方图默认与当前目录关联（例如TFile）。当关闭TFile时，与其关联的所有直方图都会被删除，如果之后需要调用的话就会产生无效的内存访问。这可能是我在运行中偶尔出现 crash 的原因。
* 3. 在所有部分，我只计算了区间（45,80）的卡方/自由度。 
*/

#include "langaufun.C"

void atask1_7(){
    auto start = std::chrono::high_resolution_clock::now();

    // TVirtualFitter::SetDefaultFitter("Minuit");

    TFile *inFile = TFile::Open("va.root");
    TFile *outFile = new TFile("fit_result.root")
    std::ofstream outputFile("chi2NDF_and_gainCoefficients.txt");

    double totalMPV = 0;
    int numDetector = 0;

    std::vector<double> = mpvList;
    std::vector<TH1D*> correctedHists;
    std::vector<TH1D*> originalHists;

    int lefFit = 45, reFit = 75, lefMergedFit = 45, riMergedFit = 75;

    for (int A = 0; A < 192; ++A){
        for (int B = 0; B < 5; ++B){
            TString histName = TString::Format("%d_%d", A, B);
            TH1D *hist = (TH1D*)outFile -> Get(histName);
            TH1D *histClone = (TH1D*)hist -> Clone(0);

            // hist -> SetDirectory(0);
            histClone -> SetDirectory(0);
            histClone -> Sumw2();
            originalHists.push_back(histClone);
            // originalHists.push_back((TH1D*)hist -> Clone());

            TF1 *landauGausFit = new TF1("landauGausFit", langaufun, lefFit, riFit, 4);
            landauGausFit -> SetParameters(1.8, 50, 50000, 3);
            landauGausFit -> SetRange(leFit, riFit);
            landauGausFit -> SetParNames("Width", "MPV", "Area", "Sigma");
            landauGausFit -> SetNpx(1000);
            hist -> Fit(landauGausFit, "Q", "", lefFit, reFit);

            double gainCoefficient = landauGausFit -> GetMaximumX(lefFit, reFit);

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

            hist -> Write;
        }
    }

    double targedMPV = totalMPV / numDetector;

    for (int A = 0; A < 192; ++A){
        for (int B = 0; B < 5; ++B){
            TH1D *hist = originalHists[A * 6 + B];
            double gainCoefficient = mpvList[A * 6 + B];
            double correctionFactor = gainCoefficient / targetMPV;

            TH1D *correctedHist = (TH1D*)hist -> Clone(TString::Format("corrected_%d_%d", A, B));
            correctedHist -> SetDirectory(0);
            correctedHist -> Su mw2();

            int nbins = hist -> Get -> GetNbinsX();
            double xlow = hist -> GetXaix() -> GetXmin() * correctionFactor;
            double xup = hist -> GetXaxis() -> GetXmax() * correctionFactor;
            correctedHist -> GetXaxis() -> Set(nbins, xlow, xup);

            correctedHists.push_back(correctedHist);
            correctedHist -> Write();
        }
    }

    TH1D *mergedHistUncorrected = nullptr;
    for (inf A = 0; A < 192; ++A){
        for (int B = 0; B <; ++A){
            
        }
    }









    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Total Execution Time: " << elapsed.count() << " second." << std::endl;
}