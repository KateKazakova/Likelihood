#include <cstdlib>
#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <TLorentzVector.h>

#include "AtlasUtils.C"
#include "AtlasLabels.C"
#include "AtlasStyle.C"

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include "TROOT.h"

   // for signal files we use seletion on mc_ph_type [13, 16]
   // for background we don't use this seletion

  const char *fname[104] = {
    "/home/katet/Programs/Znunugamma/ForDataSamples/user.akurova.MxAOD_Znunugamma2jEWK_MC16a.root",
    "/home/katet/Programs/Znunugamma/ForDataSamples/user.akurova.MxAOD_Znunugamma2jEWK_MC16d.root",
    "/home/katet/Programs/Znunugamma/ForDataSamples/user.akurova.MxAOD_Znunugamma2jEWK_MC16e.root",
    "/home/katet/Programs/Znunugamma/ForDataSamples/user.akurova.MxAOD_Znunugamma2jQCD_MC16a.root",
    "/home/katet/Programs/Znunugamma/ForDataSamples/user.akurova.MxAOD_Znunugamma2jQCD_MC16d.root",
    "/home/katet/Programs/Znunugamma/ForDataSamples/user.akurova.MxAOD_Znunugamma2jQCD_MC16e.root",
    // "/home/katet/Programs/Znunugamma/ForDataSamples/user.akurova.MxAOD_Znunugamma2jQCD_Pythia_MC16a.root",
    // "/home/katet/Programs/Znunugamma/ForDataSamples/user.akurova.MxAOD_Znunugamma2jQCD_Pythia_MC16d.root",
    // "/home/katet/Programs/Znunugamma/ForDataSamples/user.akurova.MxAOD_Znunugamma2jQCD_Pythia_MC16e.root",
    // "/home/katet/Programs/Znunugamma/ForDataSamples/user.akurova.MxAOD_Znunugamma2jEWK_Herwig_MC16a.root",
    // "/home/katet/Programs/Znunugamma/ForDataSamples/user.akurova.MxAOD_Znunugamma2jEWK_Herwig_MC16d.root",
    // "/home/katet/Programs/Znunugamma/ForDataSamples/user.akurova.MxAOD_Znunugamma2jEWK_Herwig_MC16e.root",
  };


 void SignalHist(){
   //SetAtlasStyle();

   //
   // TObjArray Hist(0);

   int NBins = 12;

   double Range_L = -2.5, Range_R = 2.5;

   double sum_A = 0, sum_B = 0, sum_C = 0, sum_D = 0;
   double sum_BE = 0, sum_E = 0, sum_DF = 0, sum_F = 0;
   double sum_err_A = 0, sum_err_B = 0, sum_err_C = 0, sum_err_D = 0;

   int Loose = 0x27fc01;

   double c_A, c_B, c_C, c_D;
   double err_c_A, err_c_B, err_c_C, err_c_D;
   double c_BE, c_E, c_DF, c_F;
   double Leakage;

   double MinCut = 2.45, MediumCut = 2.45 + 2.0, MaxCut = 25.45;
   // double MinCut = 2.45, MediumCut = 4.45, MaxCut = 29.45;

   TH1D *hist_A = new TH1D ("sig_A", "sig_A", NBins, Range_L, Range_R);
   TH1D *hist_B = new TH1D ("sig_B", "sig_B", NBins, Range_L, Range_R);
   TH1D *hist_C = new TH1D ("sig_C", "sig_C", NBins, Range_L, Range_R);
   TH1D *hist_D = new TH1D ("sig_D", "sig_D", NBins, Range_L, Range_R);


   for(int i = 0; i<3+3; i++){

     char ftempname[104]{};
     sprintf( ftempname, "%s", fname[i] );
     TFile *file = new TFile(ftempname, "READ");
     cout<<ftempname<<endl;


   double sum_of_error_A = 0, sum_of_error_B = 0, sum_of_error_C = 0, sum_of_error_D = 0;
   double sum_of_weights_bk_xAOD, sumw_MC16 = 0, weight, sum = 0, ph_pt, sum_koef = 0, koef;
   double ph_iso_pt, ph_iso_et40, ph_z_point, metTST_pt, ph_iso_et20, ph_phi, ph_eta;
   UInt_t ph_isem, n_ph, n_mu, n_e_medium, n_jet;
   Int_t mc_ph_type;
   double jet_lead_phi, jet_sublead_phi, metTST_phi, metTSTsignif, soft_term_pt;
   double jet_lead_eta, jet_lead_pt, jet_lead_E,jet_sublead_pt, jet_sublead_eta, jet_sublead_E;
   TLorentzVector met, ph, jet, jet2;
   TTree *tree_MC_sw = (TTree*)file->Get("output_tree_sw");
   TTree *tree = (TTree*)file->Get("output_tree");
   TTree *tree_norm = (TTree*)file->Get("norm_tree");
   tree_MC_sw->SetBranchAddress("sum_of_weights_bk_xAOD",&sum_of_weights_bk_xAOD);
   tree->SetBranchAddress("weight",&weight);
   tree->SetBranchAddress("ph_pt",&ph_pt);

   tree->SetBranchAddress("ph_phi",&ph_phi);
   tree->SetBranchAddress("ph_eta",&ph_eta);

   tree->SetBranchAddress("jet_lead_pt", &jet_lead_pt);  //leading jet p_x
   tree->SetBranchAddress("jet_lead_eta", &jet_lead_eta);  //p_y
   tree->SetBranchAddress("jet_lead_phi", &jet_lead_phi);  //p_z
   tree->SetBranchAddress("jet_lead_E", &jet_lead_E);    //E

   tree->SetBranchAddress("jet_sublead_pt", &jet_sublead_pt);  //leading jet p_x
   tree->SetBranchAddress("jet_sublead_eta", &jet_sublead_eta);  //p_y
   tree->SetBranchAddress("jet_sublead_phi", &jet_sublead_phi);  //p_z
   tree->SetBranchAddress("jet_sublead_E", &jet_sublead_E);    //E

   tree->SetBranchAddress("metTST_pt", &metTST_pt);  //MET p_x
   tree->SetBranchAddress("metTST_phi", &metTST_phi);  //p_y

   tree->SetBranchAddress("ph_iso_et40", &ph_iso_et40);
   tree->SetBranchAddress("ph_iso_et20", &ph_iso_et20);
   tree->SetBranchAddress("ph_iso_pt", &ph_iso_pt);
   tree->SetBranchAddress("weight", &weight);
   tree->SetBranchAddress("n_ph", &n_ph);
   tree->SetBranchAddress("n_jet", &n_jet);
   tree->SetBranchAddress("n_mu", &n_mu);
   tree->SetBranchAddress("metTST_pt", &metTST_pt);
   tree->SetBranchAddress("n_e_looseBL", &n_e_medium);
   tree->SetBranchAddress("ph_isem", &ph_isem);
   tree->SetBranchAddress("ph_z_point", &ph_z_point);
   tree->SetBranchAddress("mc_ph_type", &mc_ph_type);
   tree_norm->SetBranchAddress("koef",&koef);
   tree->SetBranchAddress("metTSTsignif", &metTSTsignif);
   tree->SetBranchAddress("soft_term_pt", &soft_term_pt);

   int entry = (int)tree_MC_sw->GetEntries();
   int N = (int)tree->GetEntries();
   int N_koef = (int)tree_norm->GetEntries();
   for (int i=0; i<entry; i++) {
    tree_MC_sw->GetEntry(i);
    sumw_MC16 += sum_of_weights_bk_xAOD;
   }

    for(int i = 0; i < 1; i++){
      tree_norm->GetEntry(i);
      sum_koef = koef;
    }


   //LoosePrime2 = ph_isem & 0x27fc01;
   //LoosePrime3 = ph_isem & 0x25fc01;
   //LoosePrime4 = ph_isem & 0x5fc01;
   //LoosePrime5 = ph_isem & 0x1fc01;

   Double_t lumi_mc16a = 36214.96;
   Double_t lumi_mc16d = 44307.4;
   Double_t lumi_mc16e = 58450.1;


  for(int i = 0; i < N; i++){

     tree->GetEntry(i);

     jet.SetPtEtaPhiE(jet_lead_pt,jet_lead_eta,jet_lead_phi,jet_lead_E);
     jet2.SetPtEtaPhiE(jet_sublead_pt,jet_sublead_eta,jet_sublead_phi,jet_sublead_E);
     met.SetPtEtaPhiM(metTST_pt,0,metTST_phi,0);
     ph.SetPtEtaPhiE(ph_pt,ph_eta,ph_phi,ph_iso_et40);
     //
     if(fabs(ph_z_point)>=250 || fabs(weight)>=100) continue;
     if(mc_ph_type < 13 || mc_ph_type > 16) continue;
     if(ph_pt <= 150) continue;
     if(n_jet < 2) continue;
     if(n_ph !=1 || n_mu !=0 || n_e_medium != 0) continue;

     //Inclusive
     if(metTST_pt <= 130) continue;
     if(metTSTsignif <= 11) continue;
     if(fabs(met.DeltaPhi(jet)) <= 0.4 ) continue;
     if(fabs(met.DeltaPhi(jet2)) <= 0.3 ) continue;
     if(fabs(met.DeltaPhi(ph)) <= 0.7) continue;

     //EWK
     // if(metTST_pt <= 120) continue;
     // if(metTSTsignif <= 12) continue;
     // if(fabs(met.DeltaPhi(jet)) <= 0.3 ) continue;
     // if(fabs(met.DeltaPhi(jet2)) <= 0.3 ) continue;
     // if(fabs(met.DeltaPhi(ph)) <= 0.4) continue;
     // if(soft_term_pt >= 16) continue;


     TString new_ftempname = TString(ftempname);
     if(new_ftempname.Contains("MC16a")){
             if ((ph_iso_et40 - 0.022*ph_pt) < MinCut && ph_isem == 0 && ph_iso_pt/ph_pt < 0.05) hist_A->Fill(ph_eta, lumi_mc16a*sum_koef*weight/(sumw_MC16));
             else if((ph_iso_et40 - 0.022*ph_pt) > MediumCut && (ph_iso_et40 - 0.022*ph_pt) < MaxCut && ph_isem == 0 && ph_iso_pt/ph_pt > 0.05) hist_B->Fill(ph_eta, lumi_mc16a*sum_koef*weight/(sumw_MC16));
             else if((ph_iso_et40 - 0.022*ph_pt) < MinCut && (ph_isem != 0 && (ph_isem & Loose) == 0 ) && ph_iso_pt/ph_pt < 0.05) hist_C->Fill(ph_eta, lumi_mc16a*sum_koef*weight/(sumw_MC16));
             else if((ph_iso_et40 - 0.022*ph_pt) > MediumCut && (ph_iso_et40 - 0.022*ph_pt) < MaxCut && (ph_isem != 0 && (ph_isem & Loose) == 0 ) && ph_iso_pt/ph_pt > 0.05) hist_D->Fill(ph_eta, lumi_mc16a*sum_koef*weight/(sumw_MC16));
     }
    else if(new_ftempname.Contains("MC16d")){
            if ((ph_iso_et40 - 0.022*ph_pt) < MinCut && ph_isem == 0 && ph_iso_pt/ph_pt < 0.05) hist_A->Fill(ph_eta, lumi_mc16d*sum_koef*weight/(sumw_MC16));
            else if((ph_iso_et40 - 0.022*ph_pt) > MediumCut && (ph_iso_et40 - 0.022*ph_pt) < MaxCut && ph_isem == 0 && ph_iso_pt/ph_pt > 0.05) hist_B->Fill(ph_eta, lumi_mc16d*sum_koef*weight/(sumw_MC16));
            else if((ph_iso_et40 - 0.022*ph_pt) < MinCut && (ph_isem != 0 && (ph_isem & Loose) == 0 ) && ph_iso_pt/ph_pt < 0.05) hist_C->Fill(ph_eta, lumi_mc16d*sum_koef*weight/(sumw_MC16));
            else if((ph_iso_et40 - 0.022*ph_pt) > MediumCut && (ph_iso_et40 - 0.022*ph_pt) < MaxCut && (ph_isem != 0 && (ph_isem & Loose) == 0 ) && ph_iso_pt/ph_pt > 0.05) hist_D->Fill(ph_eta, lumi_mc16d*sum_koef*weight/(sumw_MC16));
     }
    else if(new_ftempname.Contains("MC16e")){
            if ((ph_iso_et40 - 0.022*ph_pt) < MinCut && ph_isem == 0 && ph_iso_pt/ph_pt < 0.05) hist_A->Fill(ph_eta, lumi_mc16e*sum_koef*weight/(sumw_MC16));
            else if((ph_iso_et40 - 0.022*ph_pt) > MediumCut && (ph_iso_et40 - 0.022*ph_pt) < MaxCut && ph_isem == 0 && ph_iso_pt/ph_pt > 0.05) hist_B->Fill(ph_eta, lumi_mc16e*sum_koef*weight/(sumw_MC16));
            else if((ph_iso_et40 - 0.022*ph_pt) < MinCut && (ph_isem != 0 && (ph_isem & Loose) == 0 ) && ph_iso_pt/ph_pt < 0.05) hist_C->Fill(ph_eta, lumi_mc16e*sum_koef*weight/(sumw_MC16));
            else if((ph_iso_et40 - 0.022*ph_pt) > MediumCut && (ph_iso_et40 - 0.022*ph_pt) < MaxCut && (ph_isem != 0 && (ph_isem & Loose) == 0 ) && ph_iso_pt/ph_pt > 0.05) hist_D->Fill(ph_eta, lumi_mc16e*sum_koef*weight/(sumw_MC16));
    }

  }


     Double_t errA, errB, errC, errD;


     double N_A = hist_A->IntegralAndError(1, hist_A->GetNbinsX(), errA, "");
     double N_B = hist_B->IntegralAndError(1, hist_B->GetNbinsX(), errB, "");
     double N_C = hist_C->IntegralAndError(1, hist_C->GetNbinsX(), errC, "");
     double N_D = hist_D->IntegralAndError(1, hist_D->GetNbinsX(), errD, "");


     cout<<"N_A = "<<N_A<<" +- "<<errA<<endl;
     cout<<"N_B = "<<N_B<<" +- "<<errB<<endl;
     cout<<"N_C = "<<N_C<<" +- "<<errC<<endl;
     cout<<"N_D = "<<N_D<<" +- "<<errD<<endl;


     /// couting sum of events with weights
     sum_A = N_A;
     sum_B = N_B;
     sum_C = N_C;
     sum_D = N_D;

     sum_err_A += errA*errA;
     sum_err_B += errB*errB;
     sum_err_C += errC*errC;
     sum_err_D += errD*errD;

     cout<<"loose'2:"<<endl;
     cout<<"-----Result-----"<<endl;
     cout<<"Sum in region A = "<<sum_A<<" +- "<<sqrt(sum_err_A)<<endl;
     cout<<"Sum in region B = "<<sum_B<<" +- "<<sqrt(sum_err_B)<<endl;
     cout<<"Sum in region C = "<<sum_C<<" +- "<<sqrt(sum_err_C)<<endl;
     cout<<"Sum in region D = "<<sum_D<<" +- "<<sqrt(sum_err_D)<<endl;

   file->Close();

  }

  const char *fOutName = "/home/katet/Programs/ABCD/TRooFit/demo/Liklehood/Liklehood_Signal.root  ";
  TFile *fOut = new TFile(fOutName, "RECREATE");
  hist_A->Write();
  hist_B->Write();
  hist_C->Write();
  hist_D->Write();
  fOut->Close();

 }
