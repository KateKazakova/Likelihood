{

    gStyle->SetOptStat(0);
    int nBins = 12;
    double Left = -2.5, Right = 2.5;

    const char* labels[4] = {"A", "B", "C", "D"};
    const char* BHnames[4] = {"bkg_A", "bkg_B", "bkg_C", "bkg_D"};
    const char* SHnames[4] = {"sig_A", "sig_B", "sig_C", "sig_D"};
    const char* DHnames[4] = {"data_A", "data_B", "data_C", "data_D"};
    const char* DMBHnames[4] = {"data_A", "data_B", "data_C", "data_D"};
    const char* OnlyBKGnames[4] = {"bkg_A", "bkg_B", "bkg_C", "bkg_D"};

    const char* fname_signal = "/home/katet/Programs/ABCD/TRooFit/demo/Liklehood/Liklehood_Signal.root";
    //const char* fname_databkg = "/home/katet/Programs/ABCD/ABCD_DataBkgWOEtoGam.root";
    const char* fname_databkg = "/home/katet/Programs/ABCD/TRooFit/demo/Liklehood/Liklehood_DMB.root";
    const char* fname_onlybkg = "/home/katet/Programs/ABCD/ABCD_OnlyBKG.root";
    TFile *file = new TFile(fname_signal, "READ");
    TFile *DMBfile = new TFile(fname_databkg, "READ");
    TFile *BKGfile = new TFile(fname_onlybkg, "READ");

    TH1D* BHist[4];
    TH1D* SHist[4];
    //TH1D* DHist[4];
    TH1D* DMBHist[4];

    for (int j=0; j<4; j++) {
        SHist[j]   = (TH1D*)file -> Get(SHnames[j]);
        DMBHist[j] = (TH1D*)DMBfile -> Get(DMBHnames[j]);
        BHist[j] = (TH1D*)BKGfile -> Get(OnlyBKGnames[j]);
    }

    TRooH1D* SRooHist[4];
    TRooH1D* BRooHist[4];
    RooRealVar mu("mu", "mu", 1, 0, 100);
    RooRealVar x("#eta^{#gamma}", "#eta^{#gamma}", Left, Right);

    for (int j=0; j<4; j++) {
        SRooHist[j] = new TRooH1D(Form("s%s", labels[j]), Form("s%s", labels[j]), x, nBins);
        BRooHist[j] = new TRooH1D(Form("b%s", labels[j]), Form("b%s", labels[j]), x, nBins);
        SRooHist[j]->SetFillColor(kRed);
        BRooHist[j]->SetFillColor(kCyan);
	cout << labels[j] << "           : " << SHist[j] -> Integral() << endl;
	cout << labels[j] << " DMB       : " << DMBHist[j] -> Integral() << endl;
    }

    for (int j=0; j<4; j++)  {
        for (int i=1; i<=nBins; i++)  {
            double binContent;
            binContent = SHist[j] -> GetBinContent(i);
            SRooHist[j] -> SetBinContent(i, binContent);
        }
    }

    for (int j=0; j<4; j++) SRooHist[j]->addNormFactor(mu);

    RooRealVar lumi("lumi", "lumi", 1, 0, 2);
    lumi.setStringAttribute("constraintType", "gaussian(1, 0.1)");
    for(int j=0; j<4; j++)  SRooHist[j] -> addNormFactor(lumi);

    for (int j=0; j<4; j++) {
        for (int i=1; i<=nBins; i++)    BRooHist[j] -> SetBinContent(i, 1);
    }

    for (int i=1; i<=nBins; i++) {
        // double ABinContent = DMBHist[0] -> GetBinContent(i);
        // double BBinContent = DMBHist[1] -> GetBinContent(i);
        // double CBinContent = DMBHist[2] -> GetBinContent(i);
        // double DBinContent = DMBHist[3] -> GetBinContent(i);
        //
        // BRooHist[0]->SetBinContent(i, ABinContent);
        // BRooHist[1]->SetBinContent(i, BBinContent);
        // BRooHist[2]->SetBinContent(i, CBinContent);
        // BRooHist[3]->SetBinContent(i, DBinContent);

        RooRealVar* bkgA = new RooRealVar(Form("bkgA_bin%d",i), Form("Background in region A, bin %d",i), 1, 0, 100);
        RooRealVar* bkgC = new RooRealVar(Form("bkgC_bin%d",i), Form("Background in region C, bin %d",i), 1, 0, 100);
        RooRealVar* TFactor = new RooRealVar(Form("TFactor_bin%d",i), Form("Transfer factor (A->B), bin %d",i), 1, 0, 1000);

        BRooHist[0]->addShapeFactor(i, *bkgA);
        BRooHist[1]->addShapeFactor(i, *TFactor);
        BRooHist[2]->addShapeFactor(i, *bkgC);
        BRooHist[3]->addShapeFactor(i, *TFactor);
    }

    BRooHist[1]->addNormFactor(*BRooHist[0]);
    BRooHist[3]->addNormFactor(*BRooHist[2]);

    TCanvas c1("Prefit","Prefit");
    c1.Divide(2,2);

    TLegend* Legend[4];
    for(int i = 0; i < 4; i++){
      Legend[i] = new TLegend(0.6787551,0.7095307,0.8930898,0.8665123,NULL,"brNDC");
      Legend[i]->SetShadowColor(10);
      Legend[i]->SetBorderSize(0);    /// without borders
      Legend[i]->SetTextSize(0.052);
      Legend[i]->SetTextColor(1);
      Legend[i]->SetFillStyle(1002);
      Legend[i]->SetFillColor(10);
      //Legend[i]->SetMarkerSize(2);
      Legend[i]->SetTextFont(42);   /// white color
    }

    TRooHStack* hist[4];
    for(int j=0;j<4;j++) {
        hist[j] = new TRooHStack(Form("hist_%s", labels[j]), Form("signal+background in region %s",labels[j]));
        hist[j] -> Add(BRooHist[j]);
        hist[j] -> Add(SRooHist[j]);

        c1.cd(j+1);
        hist[j] -> SetMinimum(0);
        hist[j] -> SetMaximum(300);

        hist[j] -> Draw("e3005");

        TText t;
        t.DrawTextNDC(0.14, 0.82, Form("Region %s", labels[j]));
        TText t1;
        t1.DrawTextNDC(0.14, 0.77, Form("Pre-fit", labels[j]));

        hist[j] -> GetXaxis() -> SetTitle("#eta^{#gamma}");
        hist[j] -> GetXaxis() -> SetTitleOffset(0.93);
        hist[j] -> GetXaxis() ->SetTitleSize(0.045);
        // Legend[j]->AddEntry(BRooHist[j],"Data-bkg","f");
        // Legend[j]->AddEntry(SRooHist[j],"Signal","f");
        // Legend[j] -> Draw();
    }


    RooCategory cat("region", "region");
    for(int j=0; j<4; j++) cat.defineType(labels[j]);


    RooRealVar w("weight", "weight", 1);
    RooDataSet data("data", "data", RooArgSet(x, w, cat), "weight");

    for (int i=1; i<=nBins; i++) {
        x = DMBHist[0] -> GetBinCenter(i);

        cat.setLabel("A"); data.add(RooArgSet(x, cat), DMBHist[0] -> GetBinContent(i));
        cat.setLabel("B"); data.add(RooArgSet(x, cat), DMBHist[1] -> GetBinContent(i));
        cat.setLabel("C"); data.add(RooArgSet(x, cat), DMBHist[2] -> GetBinContent(i));
        cat.setLabel("D"); data.add(RooArgSet(x, cat), DMBHist[3] -> GetBinContent(i));
        // x = 0;

        // cat.setLabel("A"); data.add(RooArgSet(x, cat), 110.09);
        // cat.setLabel("B"); data.add(RooArgSet(x, cat), 43.5);
        // cat.setLabel("C"); data.add(RooArgSet(x, cat), 22.97);
        // cat.setLabel("D"); data.add(RooArgSet(x, cat), 28.08);
    }

    RooArgList models;
    for(int j=0; j<4; j++)    models.add(*TRooFit::BuildModel(*hist[j], data) );

    RooSimultaneous model("model", "model", models, cat);
    RooFitResult* r = model.fitTo(data, RooFit::Save());

    TCanvas c2("Postfit","Postfit");
    c2.Divide(2,2);

    for(int j=0; j<4; j++)  {
        c2.cd(j+1);
        hist[j]->Draw("e3005");
        hist[j] -> GetXaxis() -> SetTitle("#eta^{#gamma}");
        hist[j] -> GetXaxis() -> SetTitleOffset(0.93);
        hist[j] -> GetXaxis() ->SetTitleSize(0.045);
        TH1* data_hist = (TH1*)BRooHist[0] -> GetHist(0) -> Clone(Form("data_%s", labels[j]));
        data_hist -> Reset();
        data.fillHistogram(data_hist, x, Form("region==%d", j) );
        data_hist ->SetMarkerStyle(20);

        TText t;
        t.DrawTextNDC(0.14, 0.82, Form("Region %s", labels[j]));
        TText t1;
        t1.DrawTextNDC(0.14, 0.77, Form("Post-fit", labels[j]));

        data_hist -> GetXaxis() -> SetTitle("#eta^{#gamma}");
        data_hist -> GetXaxis() -> SetTitleOffset(0.93);
        data_hist -> GetXaxis() ->SetTitleSize(0.045);

        Legend[j]->AddEntry(data_hist,"Data-bkg","lp");
        Legend[j]->AddEntry(SRooHist[j],"Signal","f");
        Legend[j]->AddEntry(BRooHist[j],"Zj, Multijet","f");
        Legend[j] -> Draw();

        data_hist -> Draw("SAME");


        c1.cd(j+1);
        data_hist -> GetXaxis() -> SetTitle("#eta^{#gamma}");
        data_hist -> GetXaxis() -> SetLabelSize(0.25);

        data_hist -> Draw("SAME");
        // Legend[j]->AddEntry(data_hist,"Data-bkg","lp");
        // Legend[j]->AddEntry(SRooHist[j],"Signal","f");
        // Legend[j]->AddEntry(BRooHist[j],"Zj, Multijet","f");
        Legend[j] -> Draw();


    }

    TCanvas cc("Pull", "Nuisance Parameter Fit Pulls");
    TRooFitResult fr(r);
    fr.Draw();

    double bkg_err = 0, sig_err = 0;

    for(int i=1; i<=nBins; i++)	{
	bkg_err += (BRooHist[0] -> GetBinError(i)) * (BRooHist[0] -> GetBinError(i));
	sig_err += (SRooHist[0] -> GetBinError(i)) * (SRooHist[0] -> GetBinError(i));
    }

    bkg_err = sqrt(bkg_err);
    sig_err = sqrt(sig_err);

    std::cout << "bkg in signal region:" << BRooHist[0]->Integral() << " +/- " << bkg_err << std::endl;
    std::cout << "signal in region:" << SRooHist[0]->Integral() << " +/- " << sig_err << std::endl;

}
