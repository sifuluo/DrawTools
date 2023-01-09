#include "TPad.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"

#include <vector>

#include "CMSStyle.cc"
#include "RatioPlot.cc"
// #include "HistManager.cc"

// void test(){
//   gStyle->SetOptStat(0);
//   TFile* f = new TFile("2018_SM.root");
//   TH1F* h1 = (TH1F*) f->Get("ttbar_1_bjets_1wp_WPrimeMass");
//   TH1F* h2 = (TH1F*) f->Get("ttbar_1_bjets_2wp_WPrimeMass");
//   h2->SetLineColor(2);
//   h1->SetTitle("");
//   TCanvas* c1 = new TCanvas("c1","c1",1000,1000);
//   TPad* p1 = new TPad("p1","p1",0,0.3,1,1);
//
//   c1->cd();
//   p1->Draw();
//
//   p1->cd();
//   h2->Draw();
//   h1->Draw("same");
//   CMSFrame(p1,3);
//   h2->GetYaxis()->SetRangeUser(0,h1->GetMaximum()*1.1);
// }


int test() {
  TFile* f = new TFile("2018_SM.root");
  vector<string> sns{"SingleMuon","ttbar","Private_FL_M500"};
  string param = "1_bjets_1wp_WPrimeMass";
  setTDRStyle();

  DatasetsLib *lib = new DatasetsLib();
  TCanvas* c1 = new TCanvas();
  // HistManager* hm = new HistManager(lib);
  // hm->SetDrawData(true);
  // hm->SetDrawRatio(true);
  // hm->MakeCanvas(param);

  RatioPlot *rp = new RatioPlot(c1);
  for (auto sn : sns) {
    TString pn = sn + "_" + param;
    TH1F* h1 = (TH1F*)f->Get(pn);
    h1->Rebin(50);
    if (lib->GetType(sn) == 0) {
      h1->SetLineColor(1);
      h1->SetFillColor(0);
      rp->AddData(h1);
    }
    else if (lib->GetType(sn) == 1) {
      h1->Scale(lib->GetNormFactor(sn, 3));
      h1->SetLineColor(lib->GetColor(sn));
      h1->SetFillColor(lib->GetColor(sn));
      rp->AddMC(h1,sn);
    }
    else if (lib->GetType(sn) == 2) {
      h1->Scale(lib->GetNormFactor(sn, 3) * 1000);
      h1->SetLineColor(lib->GetColor(sn));
      h1->SetFillColor(0);
      rp->AddSig(h1,sn + " * 1000");
    }
  }
  rp->SetLogy(true);
  rp->Legend(0.65,0.65,0.9,0.9);
  // rp->Legend(0.6,0.7,0.8,0.9);

  rp->Draw("plot.pdf");

  // TH1F* h1 = (TH1F*) f->Get("ttbar_1_bjets_1wp_WPrimeMass");
  // TH1F* h2 = (TH1F*) f->Get("ttbar_1_bjets_2wp_WPrimeMass");
}
