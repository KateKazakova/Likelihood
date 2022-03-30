{
  using namespace RooFit;
  long int FunctionCalls = 5000000000000000000;
  ROOT::Math::MinimizerOptions::SetDefaultMaxFunctionCalls(FunctionCalls);
  gStyle->SetOptStat(0);
  int nBins = 5;
  double Left = -2.5, Right = 2.5;
  double MAX = 14000;
  const char* labels[4] = {"A", "B", "C", "D"};

  const char* fname_mcbkg = "/home/katet/Programs/ABCD/TRooFit/demo/Liklehood/Liklehood_BACKGROUND.root";
  const char* fname_signal = "/home/katet/Programs/ABCD/TRooFit/demo/Liklehood/Liklehood_SIGNAL.root";
  const char* fname_data = "/home/katet/Programs/ABCD/TRooFit/demo/Liklehood/Liklehood_DATA.root";

  TFile *MCBFile = new TFile(fname_mcbkg, "READ");
  TFile *SFile = new TFile(fname_signal, "READ");
  TFile *DFile = new TFile(fname_data, "READ");

  TH1D* MCBHist[4];
  TH1D* SHist[4];
  TH1D* DHist[4];

  for (int j=0; j<4; j++) {
    MCBHist[j] = (TH1D*)MCBFile -> Get(labels[j]);
    SHist[j]   = (TH1D*)SFile   -> Get(labels[j]);
    DHist[j]   = (TH1D*)DFile   -> Get(labels[j]);
  }

  TRooH1D* MCBRooHist[4];
  TRooH1D* SRooHist[4];
  TRooH1D* BRooHist[4];

  RooRealVar mu("mu", "mu", 1, 0, 10000);
  RooRealVar x("#phi^{#gamma}", "#phi^{#gamma}", Left, Right);

  for (int j=0; j<4; j++) {
    MCBRooHist[j] = new TRooH1D(Form("mc%s", labels[j]), Form("mc%s", labels[j]), x, nBins);
    SRooHist[j] = new TRooH1D(Form("s%s", labels[j]), Form("s%s", labels[j]), x, nBins);
    BRooHist[j] = new TRooH1D(Form("b%s", labels[j]), Form("b%s", labels[j]), x, nBins);
    MCBRooHist[j] -> SetFillColor(kCyan);
    SRooHist[j] -> SetFillColor(kRed);
    BRooHist[j] -> SetFillColor(kMagenta-10);
  }

  for (int j=0; j<4; j++) {
    for (int i=1; i<=nBins; i++)  {
      MCBRooHist[j] -> SetBinContent(i, MCBHist[j] -> GetBinContent(i));
      SRooHist[j]   -> SetBinContent(i, SHist[j]   -> GetBinContent(i));

      MCBRooHist[j] -> SetBinError(i, MCBHist[j] -> GetBinError(i));
      SRooHist[j] -> SetBinError(i, SHist[j] -> GetBinError(i));
    }
  }

  for (int j=0; j<4; j++) SRooHist[j]->addNormFactor(mu);

  for (int j=0; j<4; j++) {
    for (int i=1; i<=nBins; i++)  BRooHist[j] -> SetBinContent(i, 1);
  }

  for (int i=1; i<=nBins; i++) {
      RooRealVar* bkgA = new RooRealVar(Form("bkgA_bin%d",i), Form("Background in region A, bin %d",i), 1, 0, 1000);
      RooRealVar* bkgC = new RooRealVar(Form("bkgC_bin%d",i), Form("Background in region C, bin %d",i), 1, 0, 1000);
      RooRealVar* TFactor = new RooRealVar(Form("TFactor_bin%d",i), Form("Transfer factor (A->B), bin %d",i), 1, 0, 1000);

      BRooHist[0]->addShapeFactor(i, *bkgA);
      BRooHist[1]->addShapeFactor(i, *TFactor);
      BRooHist[2]->addShapeFactor(i, *bkgC);
      BRooHist[3]->addShapeFactor(i, *TFactor);
  }

  BRooHist[1] -> addNormFactor(*BRooHist[0]);
  BRooHist[3] -> addNormFactor(*BRooHist[2]);

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

  TRooHStack* hist[4];
  for(int j=0;j<4;j++) {
      hist[j] = new TRooHStack(Form("hist_%s", labels[j]), Form("signal+background in region %s",labels[j]));
      hist[j] -> Add(BRooHist[j]);
      hist[j] -> Add(SRooHist[j]);
      hist[j] -> Add(MCBRooHist[j]);

      c1.cd(j+1);
      hist[j] -> SetMinimum(0);
      hist[j] -> SetMaximum(MAX);
      hist[j] -> Draw("e3005");

      TText t;
      t.DrawTextNDC(0.14, 0.82, Form("Region %s", labels[j]));
      TText t1;
      t1.DrawTextNDC(0.14, 0.77, Form("Pre-fit", labels[j]));

      hist[j] -> GetXaxis() -> SetTitle(/*"#phi^{#gamma}"*/ "#eta_{#gamma}");
      hist[j] -> GetXaxis() -> SetTitleOffset(0.93);
      hist[j] -> GetXaxis() ->SetTitleSize(0.045);
  }


  RooCategory cat("region", "region");
  for(int j=0; j<4; j++) cat.defineType(labels[j]);

  RooRealVar w("weight", "weight", 1);

  RooDataSet data("data", "data", RooArgSet(x, w, cat), WeightVar("weight"), StoreError(RooArgSet(x, w)));

  for (int i=1; i<=nBins; i++) {
      x = DHist[0] -> GetBinCenter(i);

      cat.setLabel("A"); data.add(RooArgSet(x, cat), DHist[0] -> GetBinContent(i), (DHist[0] -> GetBinError(i)));
      cat.setLabel("B"); data.add(RooArgSet(x, cat), DHist[1] -> GetBinContent(i), (DHist[1] -> GetBinError(i)));
      cat.setLabel("C"); data.add(RooArgSet(x, cat), DHist[2] -> GetBinContent(i), (DHist[2] -> GetBinError(i)));
      cat.setLabel("D"); data.add(RooArgSet(x, cat), DHist[3] -> GetBinContent(i), (DHist[3] -> GetBinError(i)));
  }

  RooArgList models;
  for(int j=0; j<4; j++) models.add(*TRooFit::BuildModel(*hist[j], data));

  RooSimultaneous model("model", "model", models, cat);

  RooAbsReal *nll = model.createNLL(data);
  RooMinimizer m(*nll);

  m.setMaxFunctionCalls(FunctionCalls);
  m.setMaxIterations(FunctionCalls);
  m.setEps(0.1);
  m.migrad();
  m.hesse();
  m.minos();




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
      hist[j] -> GetXaxis() -> SetTitle(/*"#phi^{#gamma}"*/ "#eta_{#gamma}");
      hist[j] -> GetXaxis() -> SetTitleOffset(0.93);
      hist[j] -> GetXaxis() ->SetTitleSize(0.045);
      TH1* data_hist = (TH1*)BRooHist[0] -> GetHist(0) -> Clone(Form("data_%s", labels[j]));
      data_hist -> Reset();
      data.fillHistogram(data_hist, x, Form("region==%d", j) );
      data_hist ->SetMarkerStyle(20);

    // { data_hist->SetBinError(1, DHist[j]->GetBinError(1));
    //   data_hist->SetBinError(2, DHist[j]->GetBinError(2));
    //   data_hist->SetBinError(3, DHist[j]->GetBinError(3));
    //   data_hist->SetBinError(4, DHist[j]->GetBinError(4));
    //   data_hist->SetBinError(5, DHist[j]->GetBinError(5));}

      TText t;
      t.DrawTextNDC(0.14, 0.82, Form("Region %s", labels[j]));
      TText t1;
      t1.DrawTextNDC(0.14, 0.77, Form("Post-fit", labels[j]));
      // TText t2;
      // t2.DrawTextNDC(0.14, 0.72, Form("N(Zj) = 1359 +- 2", labels[j]));

      data_hist -> GetXaxis() -> SetTitle(/*"#phi^{#gamma}"*/ "#eta_{#gamma}");
      data_hist -> GetXaxis() -> SetTitleOffset(0.93);
      data_hist -> GetXaxis() ->SetTitleSize(0.045);

      Legend[j] -> AddEntry(data_hist, "Data", "lp");
      Legend[j] -> AddEntry(SRooHist[j], "Signal", "f");
      Legend[j] -> AddEntry(MCBRooHist[j], "Background", "f");
      Legend[j] -> AddEntry(BRooHist[j], "Zj, Multijet", "f");
      Legend[j] -> Draw();

      data_hist -> Draw("SAME");

      c1.cd(j+1);
      data_hist -> GetXaxis() -> SetTitle(/*"#phi^{#gamma}"*/ "#eta_{#gamma}");
      data_hist -> GetXaxis() -> SetLabelSize(0.25);

      data_hist -> Draw("SAME");
      Legend[j] -> Draw();
  }

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
