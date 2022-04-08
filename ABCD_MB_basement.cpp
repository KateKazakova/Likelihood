{
  using namespace RooFit;
  long int FunctionCalls = 5000000000000000000;
  ROOT::Math::MinimizerOptions::SetDefaultMaxFunctionCalls(FunctionCalls);
  gStyle->SetOptStat(0);
  gStyle->SetEndErrorSize(0);
  int nBins = 5;
  double Left = -2.5, Right = 2.5;
  double MAX = 15000;
  const char* labels[4] = {"A", "B", "C", "D"};
  double HistMaximums[4] = {10000, 4000, 1500, 2000};
  double xbins[5] = {-2.5, -2, -1, 0.5, 2.5};

  const char* fname_mcbkg = "/home/katet/Programs/ABCD/TRooFit/demo/Liklehood/Liklehood_BACKGROUND.root";
  const char* fname_signal = "/home/katet/Programs/ABCD/TRooFit/demo/Liklehood/Liklehood_SIGNAL.root";
  const char* fname_data = "/home/katet/Programs/ABCD/TRooFit/demo/Liklehood/Liklehood_DATA.root";
  const char* fname_znunu = "/home/katet/Programs/ABCD/TRooFit/demo/Liklehood/Liklehood_ZNUNU.root";

  TFile *MCBFile = new TFile(fname_mcbkg, "READ");
  TFile *SFile = new TFile(fname_signal, "READ");
  TFile *DFile = new TFile(fname_data, "READ");
  TFile *ZnunuFile = new TFile(fname_znunu, "READ");

  TH1D* MCBHist[4];
  TH1D* SHist[4];
  TH1D* DHist[4];
  TH1D* ZnunuHist[4];

  for (int j=0; j<4; j++) {
    MCBHist[j] = (TH1D*)MCBFile -> Get(labels[j]);
    SHist[j]   = (TH1D*)SFile   -> Get(labels[j]);
    DHist[j]   = (TH1D*)DFile   -> Get(labels[j]);
    ZnunuHist[j]   = (TH1D*)ZnunuFile   -> Get(labels[j]);
  }

  TRooH1D* MCBRooHist[4];
  TRooH1D* SRooHist[4];
  TRooH1D* BRooHist[4];


  RooRealVar mu("mu", "mu", 1, 0, 100000);
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
    for (int i=1; i<=nBins; i++){
        BRooHist[j] -> SetBinContent(i, ZnunuHist[j]->GetBinContent(i));
        BRooHist[j] -> SetBinError(i, ZnunuHist[j]->GetBinError(i));
        cout<<"Znunu["<<j<<"] = "<<ZnunuHist[j]->GetBinContent(i)<<endl;
      }
  }


  for (int i=1; i<=nBins; i++) {
      RooRealVar* bkgA = new RooRealVar(Form("bkgA_bin%d",i), Form("Background in region A, bin %d",i), 1, 0, 100000);
      RooRealVar* bkgC = new RooRealVar(Form("bkgC_bin%d",i), Form("Background in region C, bin %d",i), 1, 0, 100000);
      RooRealVar* TFactor = new RooRealVar(Form("TFactor_bin%d",i), Form("Transfer factor (A->B), bin %d",i), 1, 0, 100000);

      BRooHist[0]->addShapeFactor(i, *bkgA);
      BRooHist[1]->addShapeFactor(i, *TFactor);
      BRooHist[2]->addShapeFactor(i, *bkgC);
      BRooHist[3]->addShapeFactor(i, *TFactor);
  }

  TCanvas c1("Prefit", "Prefit", 354,82, 1217, 807);
  c1.Divide(2,2);

  TLegend* Legend[4];
  for(int i = 0; i < 4; i++){
    Legend[i] = new TLegend(0.5964506,0.6297095,0.8862311,0.8505481, NULL, "brNDC");
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
      hist[j] -> SetMaximum(HistMaximums[j]);
      hist[j] -> Draw("e3005");

      TText t;
      t.DrawTextNDC(0.14, 0.82, Form("Region %s", labels[j]));
      TText t1;
      t1.DrawTextNDC(0.14, 0.77, Form("Pre-fit", labels[j]));

      hist[j] -> GetXaxis() -> SetTitle(/*"p_{T}, [GeV]"*/"#eta_{#gamma}"/*"#phi_{#gamma}, [Rad]"*/);
      hist[j] -> GetXaxis() -> SetTitleOffset(0.93);
      hist[j] -> GetXaxis() ->SetTitleSize(0.045);
  }

  BRooHist[1] -> addNormFactor(*BRooHist[0]);
  BRooHist[3] -> addNormFactor(*BRooHist[2]);

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
  //RooAbsData* data1 = data.reduce("region!=0");

  RooArgList models;
  for(int j=0; j<4; j++) models.add(*TRooFit::BuildModel(*hist[j], data));

  RooSimultaneous model("model", "model", models, cat);

  RooAbsReal *nll = model.createNLL(data); ///likelihood function
  RooMinimizer m(*nll);

  m.setMaxFunctionCalls(FunctionCalls);
  m.setMaxIterations(FunctionCalls);
  //m.setEps(0.0000001);
  m.migrad(); //optimization
  m.hesse(); //errors couting
  //m.minos(); //errors couting

 // RooSimultaneous model("model", "model", cat);
 // for(int j = 0; j < 4; j++){
 //   model.addPdf(*hist[j], labels[j]);
 //
 // }
 // x.setRange("Range", Left, Right);
 // hist[0]->setBlindRange("Range") ;
 // RooAbsData *blindedData = data.reduce("region!=0");
 // RooFitResult *fitResult = model.fitTo(*blindedData, RooFit::Save());
 // hist[0]->setBlindRange ("");
 //
  TCanvas c2("Postfit", "Postfit", 337,40,1313,1000);
  c2.cd();
  double leftMarg=0.02, rightMarg=0.02, topMarg=0.005, botMarg=0.005;
  double hOffset=0.00001, vOffset=0.00001, ratOffset=0.005, divHeight=0.13;

  TPad *padH1 = new TPad("padH1", "padH1", 0.0+leftMarg, 0.5+vOffset+divHeight+ratOffset, 0.5-hOffset, 1.0-topMarg);
  TPad *padH2 = new TPad("padH2", "padH2", 0.5+hOffset, 0.5+vOffset+divHeight+ratOffset, 1.0-rightMarg, 1.0-topMarg);

  TPad *padR1 = new TPad("padR1", "padR1", 0.0+leftMarg, 0.5+vOffset, 0.5-hOffset, 0.5+vOffset+divHeight);
  TPad *padR2 = new TPad("padR2", "padR2", 0.5+hOffset, 0.5+vOffset, 1.0-rightMarg, 0.5+vOffset+divHeight);

  TPad *padH3 = new TPad("padH3", "padH3", 0.0+leftMarg, 0.0+botMarg+divHeight+ratOffset, 0.5-hOffset, 0.5-vOffset);
  TPad *padH4 = new TPad("padH4", "padH4", 0.5+hOffset, 0.0+botMarg+divHeight+ratOffset, 1.0-rightMarg, 0.5-vOffset);

  TPad *padR3 = new TPad("padR3", "padR3", 0.0+leftMarg, 0.0+botMarg, 0.5-hOffset, 0.0+botMarg+divHeight);
  TPad *padR4 = new TPad("padR4", "padR4", 0.5+hOffset, 0.0+botMarg, 1.0-rightMarg, 0.0+botMarg+divHeight);

  padR1 -> SetTopMargin(0);
  c2.cd(); padH1->Draw();
  c2.cd(); padR1->Draw();
  c2.cd(); padH2->Draw();
  c2.cd(); padR2->Draw();
  c2.cd(); padH3->Draw();
  c2.cd(); padR3->Draw();
  c2.cd(); padH4->Draw();
  c2.cd(); padR4->Draw();

  TPad* hPads[4] = {padH1, padH2, padH3, padH4};
  TPad* rPads[4] = {padR1, padR2, padR3, padR4};
  TH1D* stackCloneH[4];

  for(int j=0; j<4; j++) {
    // hPads[j] -> SetTopMargin(0.01);
    hPads[j] -> SetBottomMargin(0.003);
    // hPads[j] -> SetLeftMargin(0.01);
    // hPads[j] -> SetRightMargin(0.01);

    rPads[j] -> SetTopMargin(0.0001);
    rPads[j] -> SetBottomMargin(0.3);
    // rPads[j] -> SetLeftMargin(0.01);
    // rPads[j] -> SetRightMargin(0.01);
  }

  for(int j=0; j<4; j++)  {
    stackCloneH[j] = new TH1D("", "", nBins, Left, Right);
    for(int i=1; i<=nBins; i++) {
      stackCloneH[j] -> SetBinContent(i, hist[j]->GetBinContent(i));
      stackCloneH[j] -> SetBinError(i, hist[j]->GetBinError(i));
    }
  }

  TH1D *podrelH[4];
  TH1D *negrelH[4];

  for(int j=0; j<4; j++)  {
      hPads[j] -> cd();

      hist[j] -> SetMaximum(HistMaximums[j]);
      hist[j]->Draw("e3005");
      hist[j] -> GetXaxis() -> SetTitle(/*"p_{T}, [GeV]"*/"#eta_{#gamma}"/*"#phi_{#gamma}, [Rad]"*/);
      hist[j] -> GetXaxis() -> SetTitleOffset(0.93);
      hist[j] -> GetYaxis() -> SetTitleOffset(1.35);
      hist[j] -> GetXaxis() ->SetTitleSize(0.05);
      hist[j] -> GetYaxis() ->SetTitleSize(0.042);
      hist[j] -> GetYaxis() ->SetLabelSize(0.042);
      TH1* data_hist = (TH1*)BRooHist[0] -> GetHist(0) -> Clone(Form("data_%s", labels[j]));
      data_hist -> Reset();
      data.fillHistogram(data_hist, x, Form("region==%d", j) );
      data_hist ->SetMarkerStyle(20);


      TText t;
      t.DrawTextNDC(0.14, 0.82, Form("Region %s", labels[j]));
      TText t1;
      t1.DrawTextNDC(0.14, 0.77, Form("Post-fit", labels[j]));
      // TText t2;
      // t2.DrawTextNDC(0.14, 0.72, Form("N(Zj) = 1359 +- 2", labels[j]));

      data_hist -> GetXaxis() -> SetTitle(/*"p_{T}, [GeV]"*/"#eta_{#gamma}"/*"#phi_{#gamma}, [Rad]"*/);
      data_hist -> GetXaxis() -> SetTitleOffset(0.93);
      data_hist -> GetXaxis() ->SetTitleSize(0.045);

      if(j) Legend[j] -> AddEntry(data_hist, "Data", "lp");
      Legend[j] -> AddEntry(SRooHist[j], "Signal", "f");
      Legend[j] -> AddEntry(MCBRooHist[j], "Bkg. w/o jet#rightarrow#gamma", "f");
      Legend[j] -> AddEntry(BRooHist[j], "jet#rightarrow#gamma", "f");
      Legend[j] -> Draw();

      //if(j) data_hist -> Draw("SAME");
      data_hist -> Draw("SAME");

      c1.cd(j+1);
      data_hist -> GetXaxis() -> SetTitle(/*"p_{T}, [GeV]"*/"#eta_{#gamma}"/*"#phi_{#gamma}, [Rad]"*/);
      data_hist -> GetXaxis() -> SetLabelSize(0.25);

      //if(j) data_hist -> Draw("SAME");
      data_hist -> Draw("SAME");
      Legend[j] -> Draw();

      // if(j != 0) continue;
      // double W1 = data_hist -> Integral();
      // double W2 = hist[j] -> Integral();
      // double w1, w2, s1sq, s2sq;
      //
      //
      // double chi2 = 0;
      // for (int i=1; i <= nBins; i++)  {
      //   w1 = data_hist -> GetBinContent(i);
      //   w2 = hist[j] -> GetBinContent(i);
      //   s1sq = w1;
      //   s2sq = pow(hist[j]->GetBinError(i), 2);
      //
      //   chi2 += (W1*w2 - W2*w1)*(W1*w2 - W2*w1)/(W1*W1*s2sq + W2*W2*s1sq);
      // }
      // cout << "Chi-2 = " << chi2 << endl;

      podrelH[j] = new TH1D("", "", nBins, Left, Right);
      negrelH[j] = new TH1D("", "", nBins, Left, Right);
      double xcent;
      for(int i=1; i<=nBins; i++) {
        podrelH[j] -> SetBinContent(i, 1+(stackCloneH[j] -> GetBinError(i))/(stackCloneH[j] -> GetBinContent(i)));
        negrelH[j] -> SetBinContent(i, 1-(stackCloneH[j] -> GetBinError(i))/(stackCloneH[j] -> GetBinContent(i)));
      }


      rPads[j] -> cd();
      TH1D *rat = (TH1D*)stackCloneH[j]->Clone("hnew");


      rat->GetXaxis()->SetTitleOffset(0.85);
      rat->GetXaxis()->SetLabelOffset(0.02);
      rat->GetXaxis()->SetTitleSize(0.15);
      rat->GetXaxis()->SetTitle("#eta_{#gamma}");
      rat -> Divide(DHist[j], stackCloneH[j]);
      rat->SetLineWidth(2.5);
        rat->SetLineColor(kBlue+2);
        rat->SetMarkerSize(1.2);
        rat->SetMarkerColor(kBlue+2);
        rat->SetMarkerStyle(20);
      rat->GetXaxis()->SetLabelSize(0.11);
      rat->GetYaxis()->SetLabelSize(0.11);
      rat->GetYaxis()->SetRangeUser(0.6999, 1.29999);
      rat->GetXaxis()->SetTickLength(0.15);
    rat->GetYaxis()->SetTickLength(0.03);


      podrelH[j] -> SetFillStyle(3004);
      negrelH[j] -> SetFillStyle(3001);

      podrelH[j] -> SetLineWidth(0);
      negrelH[j] -> SetLineWidth(0);

      podrelH[j] -> SetFillColor(4);
      negrelH[j] -> SetFillColor(10);

      rat -> Draw("P E1 SAME");
      podrelH[j] -> Draw("SAME");
      negrelH[j] -> Draw("SAME");
      rat -> Draw("P E1 SAME");

      TLine *l = new TLine(rat->GetXaxis()->GetXmin(), 1., rat->GetXaxis()->GetXmax(), 1.);

      l->SetLineColor(kBlack);
      l->SetLineStyle(9);
      l->SetLineWidth(1);
      l->Draw("SAME");


      // if(j != 0) continue;
      // double N = data_hist -> Integral();
      // double W = hist[j] -> Integral();
      // double n, ssq, p, w;

      // double chi2 = 0;
      // for (int i=1; i <= nBins; i++) {
      // n = data_hist -> GetBinContent(i);
      // w = hist[j] -> GetBinContent(i);
      // ssq = pow(hist[j]->GetBinError(i), 2);

      // p = (W*w-N*ssq+pow(((W*w-N*ssq)*(W*w-N*ssq)+4*W*W*ssq*n), 0.5))/(2*W*W);

      // chi2 += (n-N*p)*(n-N*p)/N*p + (w-W*p)*(w-W*p)/ssq;
      // }
      // cout<<"Chi2 = "<<chi2<<endl;
      // cout<<"Chi2/ndof = "<<chi2/(nBins-1)<<endl;
  }


  double bkg_err = 0, sig_err = 0, R_err = 0;
  for(int i=1; i<=nBins; i++)	{
    bkg_err += (BRooHist[0] -> GetBinError(i)) * (BRooHist[0] -> GetBinError(i));
    sig_err += (SRooHist[0] -> GetBinError(i)) * (SRooHist[0] -> GetBinError(i));
    R_err = sqrt( pow(((BRooHist[0] -> GetBinError(i))*BRooHist[3]->Integral())/ (BRooHist[1]->Integral()*BRooHist[2]->Integral()), 2) +
                  pow(((BRooHist[3] -> GetBinError(i))*BRooHist[0]->Integral())/ (BRooHist[1]->Integral()*BRooHist[2]->Integral()), 2) +
                  pow(( (BRooHist[1] -> GetBinError(i)) * (BRooHist[3]->Integral()) *(BRooHist[0]->Integral()) )/ ( (BRooHist[1]->Integral() ) * (BRooHist[2]->Integral()) * (BRooHist[1]->Integral()) ), 2) +
                  pow(( (BRooHist[2] -> GetBinError(i)) * (BRooHist[3]->Integral()) *(BRooHist[0]->Integral()) )/ ( (BRooHist[1]->Integral() ) * (BRooHist[2]->Integral()) * (BRooHist[2]->Integral()) ), 2) );
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
  std::cout << "R-factor: " << BRooHist[0]->Integral()*BRooHist[3]->Integral()/(BRooHist[1]->Integral()*BRooHist[2]->Integral()) <<" +- "<<R_err<<endl;
}
