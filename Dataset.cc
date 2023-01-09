#ifndef DATASET_CC
#define DATASET_CC

#include "TString.h"

#include <vector>
#include <map>
#include <string>
#include <iostream>

using namespace std;

namespace Constants {
  const vector<string> SampleYears{"2016apv","2016","2017","2018"};
  const vector<string> SampleTypes{"SingleElectron","SingleMuon", // 0,1
  "ttbar", // 2,
  "wjets_HT_70_100", "wjets_HT_100_200", "wjets_HT_200_400", "wjets_HT_400_600",
  // 3                  4                     5                    6
  "wjets_HT_600_800", "wjets_HT_800_1200", "wjets_HT_1200_2500", "wjets_HT_2500_inf",
  // 7                  8                     9                    10
  "single_antitop_tchan","single_antitop_tw","single_top_schan","single_top_tchan","single_top_tw",
  // 11                        12                  13                 14                 15
  "Private_FL_M500"};
  // 16
  const vector<string> Triggers{"SE","SM"};
  const vector<double> CMSLumiYears{19.52, 16.81, 41.48, 59.83};
}

struct Dataset {
  string Name;
  int Index;
  int Type; // 0: Data, 1: MC, 2: Signal
  int Color;
  double CrossSection; // Unit in fb
  vector<double> Size;
  string GroupName;

  int GetIndex() {
    for (unsigned i = 0; i < Constants::SampleTypes.size(); ++i) {
      if (Name == Constants::SampleTypes[i]) {
        Index = i;
        return Index;
      }
      if (i == Constants::SampleTypes.size() - 1) {
        cout << "Cannot find Sample Index for " << Name << " in SampleTypes list." <<endl;
      }
    }
    return -1;
  }

  string GetName() {
    Name = Constants::SampleTypes[Index];
    return Name;
  }

  void Print() {
    vector<string> types{"Data","MC","Signal"};
    vector<string> colors{"white","black","red","green","blue","yellow","magneta","cyan","lush green","purple"};
    //                       0       1      2      3      4       5         6        7        8          9
    cout << "Dataset: " << Name << " ,Index = " << Index << ", (" << types[Type] << ")" <<endl;
    cout << "         CrossSection = " << CrossSection << ", Sample Size  = ";
    for (unsigned i = 0; i < Size.size(); ++i) cout << Size[i] << ", ";
    if (Size.size() != 4) cout << "Not well defined";
    cout << endl;
    cout << "         Plot group is " << GroupName << ", Colored " << Color;
    if (Color < colors.size()) cout << " (" << colors[Color] << ") ";
    cout << endl;
  }
};

class DatasetsLib {
public:
  DatasetsLib() {
    Debug = false;
    Init();
  };
  void Init() {
    Datasets.clear();
    DatasetNames.clear();
    AddDataset_NGTCXS("SingleElectron"         ,""              , 0, 1 , 1., {1,1,1,1}); // 0
    AddDataset_NGTCXS("SingleMuon"             ,""              , 0, 1 , 1., {1,1,1,1}); // 1

    AddDataset_NGTCXS("ttbar"                  ,"ttbar"         , 1, 2 , 365974.4, {1, 144722000, 346052000, 476408000}); // 2

    AddDataset_NGTCXS("wjets_HT_70_100"        ,"wjets"         , 1, 3 , 1353000, {1, 19439931, 44576510, 66220256}); // 3
    AddDataset_NGTCXS("wjets_HT_100_200"       ,"wjets"         , 1, 3 , 1627450, {1, 19753958, 47424468, 51408967}); // 4
    AddDataset_NGTCXS("wjets_HT_200_400"       ,"wjets"         , 1, 3 , 435237,  {1, 15067621, 42281979, 58225632}); // 5
    AddDataset_NGTCXS("wjets_HT_400_600"       ,"wjets"         , 1, 3 , 59181,   {1, 2115509, 5468473, 7444030}); // 6
    AddDataset_NGTCXS("wjets_HT_600_800"       ,"wjets"         , 1, 3 , 14581,   {1, 2251807, 5545298, 7718765}); // 7
    AddDataset_NGTCXS("wjets_HT_800_1200"      ,"wjets"         , 1, 3 , 6656,    {1, 2132228, 5088483, 7306187}); // 8
    AddDataset_NGTCXS("wjets_HT_1200_2500"     ,"wjets"         , 1, 3 , 1608,    {1, 2090561, 4752118, 6481518}); // 9
    AddDataset_NGTCXS("wjets_HT_2500_inf"      ,"wjets"         , 1, 3 , 39,      {1, 709514, 1185699, 2097648}); // 10

    AddDataset_NGTCXS("single_antitop_tchan"   ,"single_top"    , 1, 4 , 69090,  {1,30609000,69793000,395627000}); // 11 // 9562700 events
    AddDataset_NGTCXS("single_antitop_tw"      ,"single_top"    , 1, 4 , 34970,  {1,3654510,8433998,10949620}); // 12
    AddDataset_NGTCXS("single_top_schan"       ,"single_top"    , 1, 4 , 3740,   {1,5471000,13620000,19365999}); // 13
    AddDataset_NGTCXS("single_top_tchan"       ,"single_top"    , 1, 4 , 115300, {1,63073000,129903000,178336000}); // 14
    AddDataset_NGTCXS("single_top_tw"          ,"single_top"    , 1, 4 , 34910,  {1,3368375,8507203,11270430}); // 15

    // AddDataset_NGTCXS("Private_FL_M500"        ,""              , 2, 7 , 161.1,  {1,1,1,94878});
    AddDataset_NGTCXS("Private_FL_M500"        ,""              , 2, 7 , 161.1,  {1,1,1,189291});

  }
  // Adding Dataset with parameters as Name, Group name, Type (0:Data,1:MC,2:Signal), Color, Xsection, SampleSize
  void AddDataset_NGTCXS(string name, string gname, int type, int color, double xsec, vector<double> size) {
    Dataset ds;
    ds.Name = name;
    ds.GetIndex();
    ds.Type = type;
    ds.Color = color;
    ds.CrossSection = xsec;
    ds.Size = size;
    if (gname == "") ds.GroupName = name;
    else ds.GroupName = gname;
    DatasetNames.push_back(name);
    Datasets[name] = ds;
  }

  void AddDataset(Dataset ds) {
    DatasetNames.push_back(ds.Name);
    Datasets[ds.Name] = ds;
  }

  bool CheckExist(string ds, string funcname = "") {
    if (!Debug) return true;
    bool found = (Datasets.find(ds) != Datasets.end());
    if (!found) {
      string msg =  "Dataset " + ds + " no found while running function " + funcname;
      cout << msg <<endl;
      throw runtime_error(msg);
    }
    return found;
  }

  Dataset& GetDataset(string ds) {
    CheckExist(ds,"GetDataset");
    return Datasets[ds];
  }

  int GetIndex(string ds) {
    CheckExist(ds,"GetIndex");
    return Datasets[ds].Index;
  }

  string GetGroup(string ds) {
    CheckExist(ds,"GetGroup");
    return Datasets[ds].GroupName;
  }

  int GetType(string ds) {
    CheckExist(ds, "GetType");
    return Datasets[ds].Type;
  }

  int GetColor(string ds) {
    CheckExist(ds, "GetColor");
    return Datasets[ds].Color;
  }

  double GetXsec(string ds) {
    CheckExist(ds, "GetXsec");
    return Datasets[ds].CrossSection;
  }

  double GetSize(string ds, int y) {
    CheckExist(ds, "GetSize");
    return Datasets[ds].Size[y];
  }

  double GetNormFactor(string ds, int y) {
    if (GetType(ds) == 0) return 1;
    return Constants::CMSLumiYears[y] * GetXsec(ds) / GetSize(ds,y);
  }


  bool Debug;
  int SampleYear;
  vector<string> DatasetNames;
  map<string,Dataset> Datasets;

};

#endif
