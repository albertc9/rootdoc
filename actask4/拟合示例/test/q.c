#include "style.h"
#include "langaus.c"
void hist_scale(TH1D* h,int flag = 0){ 
  float scale=1;
  if(flag==1) h->Scale(scale/h->GetMaximum());
  else h->Scale(scale/h->Integral());
}

void hist_style1(TH1D* h){ 

//  h->SetLineColor(2);
//  h->SetLineStyle(5);
  h->SetFillStyle(3005);
  h->SetFillColor(4);
  h->SetLineWidth(1.5);
}

void hist_style2(TH1D* h){ 

  h->SetLineColor(2);
//  h->SetLineStyle(5);
  h->SetLineWidth(1.5);


}

void deal_hist(TH1D* h){

  Int_t font=42; // Helvetica
  Double_t tsize=0.05;
  gStyle->SetTextFont(font);

  h->SetLabelFont(font,"x");
  h->SetTitleFont(font,"x");
  h->SetLabelFont(font,"y");
  h->SetTitleFont(font,"y");
  h->SetLabelFont(font,"z");
  h->SetTitleFont(font,"z");
  h->SetLabelSize(tsize,"x");
  h->SetTitleSize(tsize,"x");
  h->SetLabelSize(tsize,"y");
  h->SetTitleSize(tsize,"y");
  h->SetLabelSize(tsize,"z");
  h->SetTitleSize(tsize,"z");

}


void deal_hist(TH2D* h){ 

  Int_t font=42; // Helvetica
  Double_t tsize=0.05;
  gStyle->SetTextFont(font);

  h->SetLabelFont(font,"x");
  h->SetTitleFont(font,"x");
  h->SetLabelFont(font,"y");
  h->SetTitleFont(font,"y");
  h->SetLabelFont(font,"z");
  h->SetTitleFont(font,"z");
  h->SetLabelSize(tsize,"x");
  h->SetTitleSize(tsize,"x");
  h->SetLabelSize(tsize,"y");
  h->SetTitleSize(tsize,"y");
  h->SetLabelSize(tsize,"z");
  h->SetTitleSize(tsize,"z");

}

void hist_draw(char* name,TH2D* sim,TH2D* fly){
int sta_num[4]={10,20,30,50};
int flag=0;
//cout<<sim->GetNbinsX()<<"  "<<sim->GetXaxis()->GetBinLowEdge(1)<<"  "<<sim->GetXaxis()->GetBinUpEdge(sim->GetNbinsX())<<endl;
TH1D* temp_all =new TH1D(Form("temp_%s" ,name),"",sim->GetNbinsY(),sim->GetYaxis()->GetBinLowEdge(1),sim->GetYaxis()->GetBinUpEdge(sim->GetNbinsY()));
TH1D* temp_all2 =new TH1D(Form("temp2_%s" ,name),"",fly->GetNbinsY(),fly->GetYaxis()->GetBinLowEdge(1),fly->GetYaxis()->GetBinUpEdge(fly->GetNbinsY()));
//TH1D* temp_all2=new TH1D(Form("temp2_%s" ,name),"",fly->GetNbinsX(),fly->GetXaxis()->GetXbins()->GetArray()); 

TH1D* temp_h;
TH1D* temp_h2;

TCanvas* c[4];
TH1D* sim_final[4];
TH1D* fly_final[4];


for(int i=1;i<52;++i){
  temp_h=sim->ProjectionY(Form("sim_%d",i),i,i);
  temp_h2=fly->ProjectionY(Form("fly_%d",i),i,i);


  if(i>sta_num[flag] || temp_h->GetEntries()<100 || temp_h2->GetEntries()<100){
    c[flag]=new TCanvas(Form("%s_%d",name,flag),"",640,480);

    sim_final[flag]=(TH1D*)temp_all->Clone();
    fly_final[flag]=(TH1D*)temp_all2->Clone();

		TLegend* leg1=new TLegend(0.65,0.65,0.8,0.8);
		leg1->SetTextSize(0.05);
		leg1->SetBorderSize(0.0);
		leg1->AddEntry((TObject*)0, Form("%d^{#circ} < #theta <%d^{#circ}",flag*10+0,flag*10+10), "");
		leg1->AddEntry(fly_final[flag],"Data","l");
		leg1->AddEntry(sim_final[flag],"Allpix^{2}","l");
		leg1->SetFillColor(0);


    hist_style1(sim_final[flag]);
    hist_style2(fly_final[flag]);

		hist_scale(sim_final[flag]);
		hist_scale(fly_final[flag]);

    sim_final[flag]->SetTitle(";ADC counts;Events (norm.)");
    sim_final[flag]->GetYaxis()->SetTitleOffset(1.4);
    sim_final[flag]->Draw();
    fly_final[flag]->Draw("same");
    leg1->Draw("same");

    temp_all->Clear();
    temp_all2->Clear();

    c[flag]->Print(Form("%s_%d.pdf",name,flag));

    flag++;
    if(flag==4)break;
  }
  hist_scale(temp_h);
  hist_scale(temp_h2);
  temp_all->Add(temp_h);
  temp_all2->Add(temp_h2);

}


}

double cal_rms(TH1D* h){
 
	double mean=h->GetMean();
	double p[2]={0.0013,0.9987};
	double q[2];
	h->GetQuantiles(2,q,p);

	TH1D* temp=(TH1D*)h->Clone();
	temp->Clear();

	for(int i=0;i<temp->GetNbinsX();++i){
		if(temp->GetBinCenter(i+1)>q[0] && temp->GetBinCenter(i+1)<q[1])temp->SetBinContent(i+1,h->GetBinContent(i+1));
	}


//	new TCanvas;
//	h->Draw();
//	new TCanvas;
//	temp->Draw();
	return temp->GetRMS();
}

void q(){

MyStyle();

TFile* f1=new TFile("result.root","r");

TH2D* sim=(TH2D*)f1->Get("r_strip");

TH1D* fly_r=sim->ProjectionY("sim_r",0,10);



   double fr[2];
   double sv[4], pllo[4], plhi[4], fp[4], fpe[4];
   fr[0]=40;
   fr[1]=80;

   pllo[0]=0.5; pllo[1]=5.0; pllo[2]=1.0; pllo[3]=0.4;
   plhi[0]=10.0; plhi[1]=100.0; plhi[2]=100000000.0; plhi[3]=10.0;
   sv[0]=3; sv[1]=60.0; sv[2]=5000000.0; sv[3]=5.0;

   double chisqr;
   int    ndf;
   TF1 *fitsnr = langaufit(fly_r,fr,sv,pllo,plhi,fp,fpe,&chisqr,&ndf);

   double SNRPeak, SNRFWHM;
   langaupro(fp,SNRPeak,SNRFWHM);
  new TCanvas;
  fly_r->Draw();
  fitsnr->Draw("same");
double temp_1=SNRPeak;
cout<<"SNRPeak: "<<SNRPeak<<endl;
TLine* line1=new TLine(SNRPeak,0,SNRPeak,fly_r->GetMaximum());
line1->Draw("same");
}
