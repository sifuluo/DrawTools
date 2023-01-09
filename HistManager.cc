#ifndef HISTMANAGER_CC
#define HISTMANAGER_CC

#include "TString.h"
#include "TH1.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLatex.h"
#include "TLine.h"
#include "TBox.h"

#include <vector>
#include <map>
#include <iostream>
#include <string>

#include "Dataset.cc"
#include "CMSStyle.cc"
#include "RatioPlot.cc"
#include "SRPlot.cc"
#include "Tools.cc"

class HistManager{
public:
  HistManager(DatasetsLib* l_, TPad* pad) {
    Pad = pad;
    lib = l_;
    ResetMembers();
    setTDRStyle();
  };

  void SetDrawData(bool d = true) {
    DrawData = d;
    DrawRatio = d;
  }

  void SetDrawRatio(bool d = true) {
    DrawRatio = d;
    if (DrawRatio && !DrawData) cout << "Ratio cannot be drawn without drawing data" <<endl;
  }

  int GetType(string ds) {
    return lib->GetType(ds);
  }

  void SetAutoScaleSignal(bool d = true) {
    AutoScaleSignal = d;
  }

  void AddHist(string ds, TH1F* h_) {
    TH1F* h = (TH1F*)h_->Clone();
    h->SetLineColor(lib->GetColor(ds));
    if (GetType(ds) == 0) {
      h->SetLineStyle(1);
      h->SetLineColor(1);
      h->SetMarkerStyle(20); // 20:filled circle, 21:filled square
      DataHists[ds] = h;
    }
    if (GetType(ds) == 1) {
      h->SetLineStyle(1);
      h->SetLineColor(lib->GetColor(ds));
      h->SetFillColor(lib->GetColor(ds));
      MCHists[ds] = h;
    }
    if (GetType(ds) == 2) {
      h->SetLineStyle(2);
      h->SetLineColor(1);
      SignalHists[ds] = h;
    }
    Hists[ds] = h;
  }

  void NormToLumi(int isy_) {
    for (auto it = Hists.begin(); it != Hists.end(); ++it) {
      auto sn = it->first;
      if (lib->GetType(sn) == 0) continue;
      it->second->Scale(lib->GetNormFactor(sn,isy_));
    }
  }

  void RebinHists(int rb) {
    for (auto it = Hists.begin(); it != Hists.end(); ++it) {
      int rb_ = rb;
      if (rb < 0) {
        rb_ = RebinCalc(it->second->GetNbinsX(),-1 * rb);
        // cout << it->second->GetName() << " has " << it->second->GetNbinsX() << " bins, rebinning with " << rb_ <<endl;
      }
      it->second->Rebin(rb_);
    }
  }

  void SortHists() {
    //Group and order MC dataset groups by their dataset indices
    for (auto it = MCHists.begin(); it != MCHists.end(); ++it) {
      string gp = lib->GetGroup(it->first);
      if (GroupMCHists.find(gp) == GroupMCHists.end()) {
        GroupMCHists[gp] = (TH1F*)it->second->Clone();
        // First dataset of a group determines the index of the group
        // The dataset list should be ordered by group manually in the first place
        GroupMCOrderedByIndices[lib->GetIndex(it->first)] = gp;
      }
      else GroupMCHists[gp]->Add(it->second);
    }
    // Order MC dataset groups by their integrals
    for (auto it = GroupMCHists.begin(); it != GroupMCHists.end(); ++it) {
      double integral = it->second->Integral();
      GroupMCOrderedByIntegral[integral] = it->first;
    }
    // Order Signal dataset by their indices
    for (auto it = SignalHists.begin(); it != SignalHists.end(); ++it) {
      SignalOrderedByIndices[lib->GetIndex(it->first)] = it->first;
    }
  }

  void DrawRatioPlot(TString tx, TString ty, TString fn, int year) {
    RatioPlot *rp = new RatioPlot(Pad);
    rp->SetXTitle(tx);
    rp->SetYTitle(ty);
    if (DataHists.size() == 0) throw runtime_error("No Data histogram provided for RatioPlot");
    rp->AddData(DataHists.begin()->second);
    for (auto it = GroupMCOrderedByIndices.begin(); it != GroupMCOrderedByIndices.end(); ++it) {
      rp->AddMC(it->second, GroupMCHists[it->second]);
    }
    for (auto it = SignalOrderedByIndices.begin(); it != SignalOrderedByIndices.end(); ++it) {
      rp->AddSig(it->second, SignalHists[it->second]);
    }
    rp->SetLogy();
    rp->Legend(0.65,0.65,0.9,0.9);
    rp->DrawPlot(fn, year);
  }

  void DrawSRPlot(TString tx, TString ty, TString fn, int year) {
    SRPlot *srp = new SRPlot(Pad);
  }

  void DrawPlot(TString tx, TString ty, TString fn, int year) {
    SortHists();
    fn = "plots/" + fn + ".pdf";
    if (DrawRatio) {
      DrawRatioPlot(tx,ty,fn,year);
    }
    else DrawSRPlot(tx,ty,fn,year);
  }

  void ResetMembers() {
    DrawData = DrawRatio = true;
    AutoScaleSignal = true;
    Hists.clear();
    DataHists.clear();
    MCHists.clear();
    SignalHists.clear();
    SignalOrderedByIndices.clear();
    GroupMCHists.clear();
    GroupMCOrderedByIndices.clear();
    GroupMCOrderedByIntegral.clear();
  }

  DatasetsLib* lib;
  TPad* Pad;
  bool DrawData, DrawRatio, AutoScaleSignal;
  map<string, TH1F* > Hists;
  map<string, TH1F* > DataHists; // Pair may also do, for there will be only 1 data. But map stays consistent with other containers
  map<string, TH1F* > MCHists;
  map<string, TH1F* > SignalHists;
  map<int,string> SignalOrderedByIndices;

  map<string, TH1F* > GroupMCHists;
  map<int,string> GroupMCOrderedByIndices; // For legend ordering
  map<double,string> GroupMCOrderedByIntegral; // For THStack ordering:
};

#endif
