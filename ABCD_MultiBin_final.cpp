{
  long int FunctionCalls = 500000000000000;
  ROOT::Math::MinimizerOptions::SetDefaultMaxFunctionCalls(FunctionCalls);
  gStyle->SetOptStat(0);
  int nBins = 20;
  double Left = -3.14, Right = 3.14;
  double MAX = 1000;
  const char* labels[4] = {"A", "B", "C", "D"};
  const char* SHnames[4] = {"sig_A", "sig_B", "sig_C", "sig_D"};
  const char* DMBHnames[4] = {"data_A", "data_B", "data_C", "data_D"};
  const char* OnlyBKGnames[4] = {"bkg_A", "bkg_B", "bkg_C", "bkg_D"};

  /*Reading files to extract signal, background that I want to
    estimate and data minus other background histograms in 4 regions.
    Further in the comments I would call data minus background just data
    and the background that I want to estimate just backround!!!*/
  const char* fname_signal = "/home/katet/Programs/ABCD/TRooFit/demo/Liklehood/Liklehood_Signal.root";
  const char* fname_databkg = "/home/katet/Programs/ABCD/TRooFit/demo/Liklehood/Liklehood_DMB.root";
  //const char* fname_onlybkg = "ABCD_OnlyBKG.root";
  TFile *SIGfile = new TFile(fname_signal, "READ");
  TFile *DMBfile = new TFile(fname_databkg, "READ");
  //TFile *BKGfile = new TFile(fname_onlybkg, "READ");

  //TH1D* BHist[4];
  TH1D* SHist[4];
  TH1D* DMBHist[4];

  /*Filling arrays with histograms in 4 regions*/
  for (int j=0; j<4; j++) {
    SHist[j]   = (TH1D*)SIGfile -> Get(SHnames[j]);
    DMBHist[j] = (TH1D*)DMBfile -> Get(DMBHnames[j]);
    //BHist[j]   = (TH1D*)BKGfile -> Get(OnlyBKGnames[j]);
  }

  /*SRooHist - the array that would contain signal histograms in four regions,
    BRooHist - the array that would contain background histograms in four regions*/
  TRooH1D* SRooHist[4];
  TRooH1D* BRooHist[4];
  RooRealVar mu("mu", "mu", 1, 0, 100);
  RooRealVar x("#phi^{#gamma}", "#phi^{#gamma}", Left, Right);

  /*Creating empty TRooH1D histograms and filling SRooHist and BRooHist arrays*/
  for (int j=0; j<4; j++) {
    SRooHist[j] = new TRooH1D(Form("s%s", labels[j]), Form("s%s", labels[j]), x, nBins);
    BRooHist[j] = new TRooH1D(Form("b%s", labels[j]), Form("b%s", labels[j]), x, nBins);
    SRooHist[j] -> SetFillColor(kRed);
    BRooHist[j] -> SetFillColor(kCyan);
    cout << labels[j] << "           : " << SHist[j] -> Integral() << endl;
    cout << labels[j] << " DMB       : " << DMBHist[j] -> Integral() << endl;
  }

  /*Filling SRooH1D with signal events*/
  for (int j=0; j<4; j++)  {
    for (int i=1; i<=nBins; i++)  {
        double binContent;
        binContent = SHist[j] -> GetBinContent(i);
        SRooHist[j] -> SetBinContent(i, binContent);
    }
  }

  /*Adding NormFactor*/
  for (int j=0; j<4; j++) SRooHist[j]->addNormFactor(mu);

  /*Adding lumi parameter just like in the example (What is this parameter?)*/
//  RooRealVar lumi("lumi", "lumi", 1, 0, 2);
//  lumi.setStringAttribute("constraintType", "gaussian(1, 0.1)");
//  for(int j=0; j<4; j++)  SRooHist[j] -> addNormFactor(lumi);

  /*Filling BRooHist with ones.*/
  for (int j=0; j<4; j++) {
    for (int i=1; i<=nBins; i++)  BRooHist[j] -> SetBinContent(i, 1);
  }

  /*Adding shape factors to each bin.*/
  for (int i=1; i<=nBins; i++) {
      RooRealVar* bkgA = new RooRealVar(Form("bkgA_bin%d",i), Form("Background in region A, bin %d",i), 1, 0, 100);
      RooRealVar* bkgC = new RooRealVar(Form("bkgC_bin%d",i), Form("Background in region C, bin %d",i), 1, 0, 100);
      RooRealVar* TFactor = new RooRealVar(Form("TFactor_bin%d",i), Form("Transfer factor (A->B), bin %d",i), 1, 0, 1000);

      BRooHist[0]->addShapeFactor(i, *bkgA);
      BRooHist[1]->addShapeFactor(i, *TFactor);
      BRooHist[2]->addShapeFactor(i, *bkgC);
      BRooHist[3]->addShapeFactor(i, *TFactor);
  }

  RooRealVar* bkgANorm = new RooRealVar("bkgANorm", "BkgNorm in region A", 1, 0, 100);
  RooRealVar* bkgCNorm = new RooRealVar("bkgCNorm", "BkgNorm in region C", 1, 0, 100);
  RooRealVar* bkgBDNorm = new RooRealVar("bkgDNorm", "BkgNorm in region D", 1, 0, 100);

  BRooHist[0]->addNormFactor(*bkgANorm);
  BRooHist[1]->addNormFactor(*bkgBDNorm);
  BRooHist[2]->addNormFactor(*bkgCNorm);
  BRooHist[3]->addNormFactor(*bkgBDNorm);

  BRooHist[1]->addNormFactor(*BRooHist[0]);
  BRooHist[3]->addNormFactor(*BRooHist[2]);

  /*Drawing prefit*/
  TCanvas c1("Prefit", "Prefit", 354,82, 1217, 807);
  c1.Divide(2,2);

  TLegend* Legend[4];
  for(int i = 0; i < 4; i++){
    Legend[i] = new TLegend(0.6787551, 0.7095307, 0.8930898, 0.8665123, NULL, "brNDC");
    Legend[i] -> SetShadowColor(10);
    Legend[i] -> SetBorderSize(0);
    Legend[i] -> SetTextSize(0.052);
    Legend[i] -> SetTextColor(1);
    Legend[i] -> SetFillStyle(1002);
    Legend[i] -> SetFillColor(10);
    Legend[i] -> SetTextFont(42);
  }

  /*Drawing data on prefit.*/
  TRooHStack* hist[4];
  for(int j=0;j<4;j++) {
      hist[j] = new TRooHStack(Form("hist_%s", labels[j]), Form("signal+background in region %s",labels[j]));
      hist[j] -> Add(BRooHist[j]);
      hist[j] -> Add(SRooHist[j]);

      c1.cd(j+1);
      hist[j] -> SetMinimum(0);
      hist[j] -> SetMaximum(MAX);
      hist[j] -> Draw("e3005");

      TText t;
      t.DrawTextNDC(0.14, 0.82, Form("Region %s", labels[j]));
      TText t1;
      t1.DrawTextNDC(0.14, 0.77, Form("Pre-fit", labels[j]));

      hist[j] -> GetXaxis() -> SetTitle("#phi^{#gamma}");
      hist[j] -> GetXaxis() -> SetTitleOffset(0.93);
      hist[j] -> GetXaxis() ->SetTitleSize(0.045);
  }


  RooCategory cat("region", "region");
  for(int j=0; j<4; j++) cat.defineType(labels[j]);


  /*What is weight parameter?*/
  RooRealVar w("weight", "weight", 1);
  /*Creating dataset*/
  RooDataSet data("data", "data", RooArgSet(x, w, cat), "weight");

  /*Filling dataset*/
  for (int i=1; i<=nBins; i++) {
      x = DMBHist[0] -> GetBinCenter(i);

      cat.setLabel("A"); data.add(RooArgSet(x, cat), DMBHist[0] -> GetBinContent(i));
      cat.setLabel("B"); data.add(RooArgSet(x, cat), DMBHist[1] -> GetBinContent(i));
      cat.setLabel("C"); data.add(RooArgSet(x, cat), DMBHist[2] -> GetBinContent(i));
      cat.setLabel("D"); data.add(RooArgSet(x, cat), DMBHist[3] -> GetBinContent(i));
  }

  /*Creationg models for the fit*/
  RooArgList models;
  for(int j=0; j<4; j++)    models.add(*TRooFit::BuildModel(*hist[j], data) );

  /*Creating a model. Performing a fit.*/
  RooSimultaneous model("model", "model", models, cat);

  RooAbsReal *nll = model.createNLL(data);
  RooMinimizer m(*nll);

  m.setMaxFunctionCalls(FunctionCalls);
  m.setMaxIterations(FunctionCalls);
  // m.setEps(1);
  m.migrad();
  model.getParameters(x)->Print("s");
  m.hesse();
  // //m.minos();




  //
  // RooFitResult* r = model.fitTo(data, RooFit::Save());

  /*Drawing postfit*/
  TCanvas c2("Postfit", "Postfit", 354, 82, 1217, 807);
  c2.Divide(2,2);


  for(int j=0; j<4; j++)  {
      c2.cd(j+1);
      //SRooHist[j] -> SetMaximum(3000);
      //SRooHist[j] -> Draw("e3005");
      //SRooHist[j] -> GetXaxis() -> SetTitle("#phi^{#gamma}");
      //SRooHist[j] -> GetXaxis() -> SetTitleOffset(0.93);
      //SRooHist[j] -> GetXaxis() ->SetTitleSize(0.045);

      hist[j] -> SetMaximum(MAX);
      hist[j]->Draw("e3005");
      hist[j] -> GetXaxis() -> SetTitle("#phi^{#gamma}");
      hist[j] -> GetXaxis() -> SetTitleOffset(0.93);
      hist[j] -> GetXaxis() ->SetTitleSize(0.045);
      TH1* data_hist = (TH1*)BRooHist[0] -> GetHist(0) -> Clone(Form("data_%s", labels[j]));
      data_hist -> Reset();
      data.fillHistogram(data_hist, x, Form("region==%d", j) );
      data_hist ->SetMarkerStyle(20);

    //I have to recalculate errors this like this because it doesnt work in loop.
    { data_hist->SetBinError(1, DMBHist[j]->GetBinError(1));
      data_hist->SetBinError(2, DMBHist[j]->GetBinError(2));
      data_hist->SetBinError(3, DMBHist[j]->GetBinError(3));
      data_hist->SetBinError(4, DMBHist[j]->GetBinError(4));
      data_hist->SetBinError(5, DMBHist[j]->GetBinError(5));
      data_hist->SetBinError(6, DMBHist[j]->GetBinError(6));
      data_hist->SetBinError(7, DMBHist[j]->GetBinError(7));
      data_hist->SetBinError(8, DMBHist[j]->GetBinError(8));
      data_hist->SetBinError(9, DMBHist[j]->GetBinError(9));
      data_hist->SetBinError(10, DMBHist[j]->GetBinError(10));
      data_hist->SetBinError(11, DMBHist[j]->GetBinError(11));
      data_hist->SetBinError(12, DMBHist[j]->GetBinError(12));
      data_hist->SetBinError(13, DMBHist[j]->GetBinError(13));
      data_hist->SetBinError(14, DMBHist[j]->GetBinError(14));
      data_hist->SetBinError(15, DMBHist[j]->GetBinError(15));
      data_hist->SetBinError(16, DMBHist[j]->GetBinError(16));
      data_hist->SetBinError(17, DMBHist[j]->GetBinError(17));
      data_hist->SetBinError(18, DMBHist[j]->GetBinError(18));
      data_hist->SetBinError(19, DMBHist[j]->GetBinError(19));
      data_hist->SetBinError(20, DMBHist[j]->GetBinError(20));
      data_hist->SetBinError(21, DMBHist[j]->GetBinError(21));
      data_hist->SetBinError(22, DMBHist[j]->GetBinError(22));
      data_hist->SetBinError(23, DMBHist[j]->GetBinError(23));
      data_hist->SetBinError(24, DMBHist[j]->GetBinError(24));
      data_hist->SetBinError(25, DMBHist[j]->GetBinError(25));
      data_hist->SetBinError(26, DMBHist[j]->GetBinError(26));
      data_hist->SetBinError(27, DMBHist[j]->GetBinError(27));
      data_hist->SetBinError(28, DMBHist[j]->GetBinError(28));
      data_hist->SetBinError(29, DMBHist[j]->GetBinError(29));
      data_hist->SetBinError(30, DMBHist[j]->GetBinError(30));
      data_hist->SetBinError(31, DMBHist[j]->GetBinError(31));
      data_hist->SetBinError(32, DMBHist[j]->GetBinError(32));
      data_hist->SetBinError(33, DMBHist[j]->GetBinError(33));
      data_hist->SetBinError(34, DMBHist[j]->GetBinError(34));
      data_hist->SetBinError(35, DMBHist[j]->GetBinError(35));
      data_hist->SetBinError(36, DMBHist[j]->GetBinError(36));
      data_hist->SetBinError(37, DMBHist[j]->GetBinError(37));
      data_hist->SetBinError(38, DMBHist[j]->GetBinError(38));
      data_hist->SetBinError(39, DMBHist[j]->GetBinError(39));
      data_hist->SetBinError(40, DMBHist[j]->GetBinError(40));
      data_hist->SetBinError(32, DMBHist[j]->GetBinError(32));
      data_hist->SetBinError(33, DMBHist[j]->GetBinError(33));
      data_hist->SetBinError(34, DMBHist[j]->GetBinError(34));
      data_hist->SetBinError(35, DMBHist[j]->GetBinError(35));
      data_hist->SetBinError(36, DMBHist[j]->GetBinError(36));
      data_hist->SetBinError(37, DMBHist[j]->GetBinError(37));
      data_hist->SetBinError(38, DMBHist[j]->GetBinError(38));
      data_hist->SetBinError(39, DMBHist[j]->GetBinError(39));
      data_hist->SetBinError(40, DMBHist[j]->GetBinError(40));

      data_hist->SetBinError(41, DMBHist[j]->GetBinError(41));
      data_hist->SetBinError(42, DMBHist[j]->GetBinError(42));
      data_hist->SetBinError(43, DMBHist[j]->GetBinError(43));
      data_hist->SetBinError(44, DMBHist[j]->GetBinError(44));
      data_hist->SetBinError(45, DMBHist[j]->GetBinError(45));
      data_hist->SetBinError(46, DMBHist[j]->GetBinError(46));
      data_hist->SetBinError(47, DMBHist[j]->GetBinError(47));
      data_hist->SetBinError(48, DMBHist[j]->GetBinError(48));
      data_hist->SetBinError(49, DMBHist[j]->GetBinError(49));
      data_hist->SetBinError(50, DMBHist[j]->GetBinError(50));}

      TText t;
      t.DrawTextNDC(0.14, 0.82, Form("Region %s", labels[j]));
      TText t1;
      t1.DrawTextNDC(0.14, 0.77, Form("Post-fit", labels[j]));
      TText t2;
      t2.DrawTextNDC(0.14, 0.72, Form("20 bins, N(Zj) = 1193 +- 137", labels[j]));

      data_hist -> GetXaxis() -> SetTitle("#phi^{#gamma}");
      data_hist -> GetXaxis() -> SetTitleOffset(0.93);
      data_hist -> GetXaxis() ->SetTitleSize(0.045);

      Legend[j] -> AddEntry(data_hist, "Data-bkg", "lp");
      Legend[j] -> AddEntry(SRooHist[j], "Signal", "f");
      Legend[j] -> AddEntry(BRooHist[j], "Zj, Multijet", "f");
      Legend[j] -> Draw();

      data_hist -> Draw("SAME");

      c1.cd(j+1);
      data_hist -> GetXaxis() -> SetTitle("#phi^{#gamma}");
      data_hist -> GetXaxis() -> SetLabelSize(0.25);

      data_hist -> Draw("SAME");
      Legend[j] -> Draw();
  }

  /*Drawing fit parameters. What does this graph mean? Why only lumi is shown?*/
  //TCanvas cc("Pull", "Nuisance Parameter Fit Pulls");
  //TRooFitResult fr(r);
  //fr.Draw();

  /*Calculating and printing out errors for the estimates and estimates of signal and background.*/
  double bkg_err = 0, sig_err = 0;
  for(int i=1; i<=nBins; i++)	{
    bkg_err += (BRooHist[0] -> GetBinError(i)) * (BRooHist[0] -> GetBinError(i));
    sig_err += (SRooHist[0] -> GetBinError(i)) * (SRooHist[0] -> GetBinError(i));
  }

  bkg_err = sqrt(bkg_err);
  sig_err = sqrt(sig_err);

  std::cout << "nBins: " << nBins << endl;
  std::cout << "bkg in signal region:" << BRooHist[0]->Integral() << " +/- " << bkg_err << std::endl;
  std::cout << "signal in region:" << SRooHist[0]->Integral() << " +/- " << sig_err << std::endl;
  std::cout << "signal in region B:" << SRooHist[1]->Integral() << std::endl;
  std::cout << "signal in region C:" << SRooHist[2]->Integral() << std::endl;
  std::cout << "signal in region D:" << SRooHist[3]->Integral() << std::endl;
  std::cout << " -------------------" << std::endl;
  std::cout << "R-factor: " << BRooHist[0]->Integral()*BRooHist[3]->Integral()/(BRooHist[1]->Integral()*BRooHist[2]->Integral()) << endl;
}
