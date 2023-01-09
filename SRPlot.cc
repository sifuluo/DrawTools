#ifndef SRPLOT_CC
#define SRPLOT_CC

#include "TString.h"
#include "TH1.h"
#include "THStack.h"
#include "TPad.h"
#include "TLegend.h"

#include <vector>
#include <string>
#include <iostream>

#include "CMSStyle.cc"

class SRPlot {
public:
  SRPlot(TPad* p_) {
    Pad = p_;
    Logy = true;
    Init();
  };

  void SetLogy(bool l = false) {
    Logy = l;
    Pad->SetLogy(l);
  }

  void Init() {
    Pad->cd();
    Pad->SetTopMargin(gStyle->GetPadTopMargin());
    Pad->SetBottomMargin(gStyle->GetPadBottomMargin());
    Pad->SetLogy(Logy);
  }

  void AddMC(TString n_, TH1F* h_) {
    if (MCNames.size() + SigNames.size() == 0) {
      XTitle = h_->GetXaxis()->GetTitle();
      YTitle = h_->GetYaxis()->GetTitle();
    }
    h_->SetLineStyle(1);
    MCHists.push_back(h_);
    MCNames.push_back(n_);
  }

  void SetXTitle(TString xt) {
    XTitle = xt;
  }

  void SetYTitle(TString yt) {
    YTitle = yt;
  }

  void AddSig(TString n_, TH1F* h_) {
    if (MCNames.size() + SigNames.size() == 0) {
      XTitle = h_->GetXaxis()->GetTitle();
      YTitle = h_->GetYaxis()->GetTitle();
    }
    h_->SetLineStyle(2);
    SigHists.push_back(h_);
    SigNames.push_back(n_);
  }

  void Legend(double x1 = 0.65, double y1 = 0.65, double x2 = 0.9, double y2 = 0.9) {
    if (x2 == -1) x2 = 1. - Pad->GetRightMargin();
    if (y2 == -1) y2 = 1. - Pad->GetTopMargin();
    leg = new TLegend(x1,y1,x2,y2,"","NDC");
    leg->SetBorderSize(1);
  }

  void Draw(TString fn, int year) {
    Pad->cd();
    TString utitle = ";;" + YTitle;
    TString stackname = Pad->GetName() + (TString)"_MCStack";
    MCStack = new THStack(stackname,utitle);
    for (unsigned ih = 0; ih < MCHists.size(); ++ih) {
      MCStack->Add(MCHists[ih]);
      leg->AddEntry(MCHists[ih],MCNames[ih],"f");
    }
    for (unsigned ih = 0; ih < SigHists.size(); ++ih) {
      leg->AddEntry(SigHists[ih],SigNames[ih],"l");
    }
    MCStack->Draw("hist");
    for (unsigned ih = 0; ih < SigHists.size(); ++ih) {
      SigHists[ih]->Draw("samehist");
    }
    leg->Draw();
  }


  bool Logy;

  TPad* Pad;

  TLegend* leg;
  TString XTitle, YTitle;

  vector<TH1F*> MCHists;
  vector<TString> MCNames;
  THStack* MCStack;

  vector<TH1F*> SigHists;
  vector<TString> SigNames;
};

#endif
