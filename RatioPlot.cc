#ifndef RATIOPLOT_CC
#define RATIOPLOT_CC

#include "TString.h"
#include "TH1.h"
#include "THStack.h"
#include "TPad.h"
#include "TLegend.h"

#include <vector>
#include <string>
#include <iostream>

#include "CMSStyle.cc"
#include "Tools.cc"

using namespace std;

class RatioPlot {
public:
  RatioPlot(TPad* p_) {
    Pad = p_;
    Logy = true;
    Init();
  };

  void SetLogy(bool l = true) {
    Logy = l;
    UPad->SetLogy(l);
  }

  void Init() {
    setTDRStyle();
    Pad->cd();
    TString upname = Pad->GetName() + (TString)"_upper";
    TString lowname = Pad->GetName() + (TString)"_lower";
    UPad = new TPad(upname,upname,0,0.3,1,1);
    UPad->SetTopMargin(gStyle->GetPadTopMargin()/0.7);
    UPad->SetBottomMargin(0.0);
    UPad->SetLogy(Logy);
    UPad->Draw();
    LPad = new TPad(lowname,lowname,0,0,1,0.3);
    LPad->Draw();
    LPad->SetTopMargin(0.1);
    LPad->SetTopMargin(gStyle->GetPadTopMargin()*0.3);
    LPad->SetBottomMargin(gStyle->GetPadBottomMargin()/0.3);
    LPad->SetGridy();
  }

  TPad* GetUpperPad() {
    return UPad;
  }

  TPad* GetLowerPad() {
    return LPad;
  }

  void AddData(TH1F* h_) {
    DataHist = (TH1F*)h_->Clone();
    DataHist->SetLineStyle(1);
    DataHist->SetLineColor(1);
    DataHist->SetMarkerStyle(20);
  }

  void SetXTitle(TString xt) {
    XTitle = xt;
  }

  void SetYTitle(TString yt) {
    YTitle = yt;
  }

  void AddMC(TString n_, TH1F* h_) {
    h_->SetLineStyle(1);
    MCHists.push_back(h_);
    MCNames.push_back(n_);
  }

  void AddSig(TString n_, TH1F* h_) {
    h_->SetLineStyle(2);
    SigHists.push_back(h_);
    SigNames.push_back(n_);
  }

  void AutoLegend(double x = 0.3, double y = 0.2) {
    leg = new TLegend(x,y);
  }

  void Legend(double x1 = 0.65, double y1 = 0.65, double x2 = 0.9, double y2 = 0.9) {
    if (x2 == -1) x2 = 1. - UPad->GetRightMargin();
    if (y2 == -1) y2 = 1. - UPad->GetTopMargin();
    leg = new TLegend(x1,y1,x2,y2,"","NDC");
    // if (x1 == x2 && y1 == y2) leg = new TLegend(x1,y1);
    // else leg = new TLegend(x1,y1,x2,y2); // if x1 = x2  and y1 = y2, it will be placed automatically
    leg->SetBorderSize(1);
    // leg->SetFillStyle(0);
  	// leg->SetFillColor(0);
  	// leg->SetFont(42);
  	// leg->SetTextSize(0.);
  }

  void DrawUPlot(int year, int ScaleSignal = 10) { // ScaleSignal < 0: auto scale; ScaleSignal > 0: Scale by that ; ScaleSignal = 0: do not scale
    TString utitle = ";;" + YTitle;
    TString stackname = Pad->GetName() + (TString)"_MCStack";
    MCStack = new THStack(stackname,utitle);
    for (unsigned ih = 0; ih < MCHists.size(); ++ih) {
      if (ih == 0) MCSummed = (TH1F*) MCHists[0]->Clone();
      else MCSummed->Add(MCHists[ih]);
      MCStack->Add(MCHists[ih]);
      leg->AddEntry(MCHists[ih],MCNames[ih],"f");
    }
    double maximum = (DataHist->GetMaximum() > MCStack->GetMaximum()) ? DataHist->GetMaximum() : MCStack->GetMaximum();

    for (unsigned ih = 0; ih < SigHists.size(); ++ih) {
      TString signame = SigNames[ih];
      if ((SigHists[ih]->GetMaximum() * 10.< maximum && ScaleSignal < 0) || ScaleSignal > 0) {
        double scale = ScaleSignal;
        if (scale < 0) scale = SignalScaleCalc(SigHists[ih]->GetMaximum(), maximum);
        SigHists[ih]->Scale(scale);
        signame = Form("%s*%i",signame.Data(),(int)scale);
      }
      leg->AddEntry(SigHists[ih],signame,"l");
    }

    leg->AddEntry(DataHist,"Data","p");

    MCStack->SetMaximum(maximum * 1.2);
    MCStack->Draw("hist");
    DataHist->SetTitle(utitle);
    DataHist->Draw("E1same");
    for (unsigned ih = 0; ih < SigHists.size(); ++ih) {
      SigHists[ih]->Draw("samehist");
    }
    leg->Draw();

    // DataHist->GetYaxis()->CenterTitle();
    DataHist->GetYaxis()->SetTitleSize(gStyle->GetTitleSize());
    DataHist->GetYaxis()->SetTitleOffset(gStyle->GetTitleOffset());
    DataHist->GetYaxis()->SetLabelSize(gStyle->GetLabelSize());
    DataHist->GetYaxis()->SetLabelOffset(gStyle->GetLabelOffset());

    CMSFrame(UPad,year);
  }

  void DrawLPlot() {
    TString ltitle = ";" + XTitle + ";Data/MC";
    RatioHist = (TH1F*)DataHist->Clone();
    RatioHist->Divide(MCSummed);
    RatioHist->SetTitle(ltitle);
    RatioHist->GetYaxis()->SetRangeUser(0,2);
    RatioHist->GetYaxis()->SetNdivisions(505);

    RatioHist->GetXaxis()->CenterTitle();
    RatioHist->GetXaxis()->SetTitleSize(gStyle->GetTitleSize() / 0.3 * 0.7);
    RatioHist->GetXaxis()->SetTitleOffset(gStyle->GetTitleOffset());
    RatioHist->GetXaxis()->SetLabelSize(gStyle->GetLabelSize() / 0.3 * 0.7);
    RatioHist->GetXaxis()->SetLabelOffset(gStyle->GetLabelOffset());

    RatioHist->GetYaxis()->CenterTitle();
    RatioHist->GetYaxis()->SetTitleSize(gStyle->GetTitleSize() / 0.3*0.7);
    RatioHist->GetYaxis()->SetTitleOffset(gStyle->GetTitleOffset() * 0.5 );
    RatioHist->GetYaxis()->SetLabelSize(gStyle->GetLabelSize()/ 0.3 * 0.7);
    RatioHist->GetYaxis()->SetLabelOffset(gStyle->GetLabelOffset());

    RatioHist->Draw();
  }

  void DrawPlot(TString fn,int year) {
    UPad->cd();
    DrawUPlot(year);
    LPad->cd();
    DrawLPlot();

    if (fn != "") Pad->SaveAs(fn);
  }

  bool Logy;

  TPad* Pad;
  TPad* UPad;
  TPad* LPad;
  TLegend* leg;

  TString XTitle, YTitle;

  TH1F* DataHist;

  vector<TH1F*> MCHists;
  vector<TString> MCNames;
  THStack* MCStack;
  TH1F* MCSummed;
  TH1F* RatioHist;

  vector<TH1F*> SigHists;
  vector<TString> SigNames;

};



#endif
