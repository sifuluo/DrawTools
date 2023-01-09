#include <TROOT.h>
#include <TFile.h>
#include <TH1.h>
#include <THStack.h>
#include <TCanvas.h>
#include <TString.h>

#include <vector>
#include <map>
#include <iostream>

#include "../../DrawTools/HistManager.cc"

using namespace std;

void DrawPlots(int isy_ = 3, int itr_ = 1){
  const vector<string> SampleTypes = Constants::SampleTypes;
  const vector<string> Triggers = Constants::Triggers;
  DatasetsLib *lib = new DatasetsLib();
  string SampleYear = Constants::SampleYears[isy_];
  string SampleTrigger = Triggers[itr_];

  vector<string> RunningTypes{"SingleMuon","ttbar"};
  RunningTypes = SampleTypes;

  string plotsname = "plots/" + SampleYear + "_" + SampleTrigger + ".root";
  TFile *f = new TFile(plotsname.c_str(), "READ");
  if (!f) cout << "Trying to Read from " << plotsname << ", but failed" <<endl;
  else cout << "Reading from file " << plotsname <<endl;

  vector<TString> Variables{"LeptonPt","LeptonEta","METPt","nJets","JetPt","JetEta"};
  vector<TString> VariableText{"Lepton p_{T} [GeV]", "Lepton #eta", "MET p_{T} [GeV]", "nJets", "Jet p_{T} [GeV]", "Jet #eta"};
  Variables = vector<TString>{"nJets"};
  VariableText = vector<TString>{"nJets"};
  setTDRStyle();
  TCanvas *c1 = new TCanvas();

  for (unsigned iv = 0; iv < Variables.size(); ++iv) {
    TString titlex = VariableText[iv];
    TString titley = "Number of Entries";
    HistManager *hm = new HistManager(lib, c1);
    for (unsigned i = 0; i < SampleTypes.size(); ++i) {
      if (i + itr_ == 1) continue;
      if (find(RunningTypes.begin(), RunningTypes.end(), SampleTypes[i]) == RunningTypes.end()) continue;
      TString pn = SampleTypes[i] + "_" + (string)Variables[iv];
      TH1F* hist = (TH1F*) f->Get(pn);
      if (!hist) {
        cout << "Trying to get histogram " << pn << ", but failed" << endl;
        throw runtime_error("Faild to read histogram");
      }
      string samplename = SampleTypes[i];
      // if (hm->GetType(samplename) == 2) {
      //   hist->Scale(1000.);
      //   cout << "Scaling " << samplename <<endl;
      //   samplename = samplename + "*1000";
      // }
      hm->AddHist(samplename, hist);
    }
    hm->NormToLumi(isy_);
    hm->RebinHists(-100);
    hm->DrawPlot(titlex, titley, Variables[iv], isy_);
  }
}
