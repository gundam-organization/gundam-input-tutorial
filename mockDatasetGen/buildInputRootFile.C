int nEvents{int(1E6)};
constexpr int nKnots = 7;

struct Event{
  int reaction{-1};
  int selection{-1};
  double enu{std::nan("unset")};
  double pmu{std::nan("unset")};
  double pp{std::nan("unset")};
  double asimovWeight{0.05};
  TClonesArray par1{ "TGraph", 1 };
  TClonesArray par2{ "TGraph", 1 };
  TClonesArray par3{ "TGraph", 1 };
};

void fillRandomResponse(double y[nKnots])
{
  for(int i = 0; i < 7; ++i){
    int k = gRandom->Integer(101); // 0, 1, ..., 100
    y[i] = k / 100.0;              // 0.00, ..., 1.00
  }

  y[3] = 1.0; // x[3] = 0 sigma, nominal response
}

void fillSmoothRandomResponse(const double x[nKnots], double y[nKnots])
{
  double a = gRandom->Gaus(0.0, 0.06);
  double b = gRandom->Gaus(0.0, 0.015);

  for(int i = 0; i < nKnots; ++i){

    double dx = x[i];
    y[i] = std::exp(a * dx + b * dx * dx);

    // to avoid extreme weights.
    if(y[i] < 0.05) y[i] = 0.05;
    if(y[i] > 3.00) y[i] = 3.00;
  }
  y[3] = 1.0; // x = 0 sigma
}

void buildInputRootFile(){

  gRandom = new TRandom3(202404); // Initialize with a UUID;

  auto* file = TFile::Open("./mydataset.root", "RECREATE");
  auto* tree = new TTree("events", "events");

  Event e{};

  tree->Branch("enu", &e.enu);
  tree->Branch("pmu", &e.pmu);
  tree->Branch("pp", &e.pp);
  tree->Branch("reaction", &e.reaction);
  tree->Branch("selection", &e.selection);
  tree->Branch("asimovWeight", &e.asimovWeight);
  tree->Branch("par1_TGraph", &e.par1, 32000, -1);   //tree->Branch(branchName, address, bufferSize, splitLevel);
  tree->Branch("par2_TGraph", &e.par2, 32000, -1);
  tree->Branch("par3_TGraph", &e.par3, 32000, -1);

  double par_x[nKnots]  = { -3,   -2,   -1,   0,    1,    2,    3   };
  double par1_y[nKnots];
  double par2_y[nKnots];
  double par3_y[nKnots];

  double fraction{0.1}; // shape2/shape1
  auto fShape1 = std::make_unique<TF1>("fShape1", "ROOT::Math::chisquared_pdf(x, 4)", 0, 15);
  auto fShape2 = std::make_unique<TF1>("fShape2", "ROOT::Math::chisquared_pdf(x, 5)", 0, 15);

  double buffer;
  for( int iEvent = 0 ; iEvent < nEvents ; iEvent++ ){

    // let's have an interaction with a given reaction:
    buffer = gRandom->Rndm();
    if     ( buffer < 0.6  ){ e.reaction = 1; }
    else if( buffer < 0.8  ){ e.reaction = 2; }
    else if( buffer < 0.95 ){ e.reaction = 3; }
    else if( buffer < 0.98 ){ e.reaction = 4; }
    else if( buffer < 0.99 ){ e.reaction = 5; }
    else                    { e.reaction = 6; }

    // reaction 1 is the most probable
    if(e.reaction == 1){
      // create the kinematics
      e.enu = fShape1->GetRandom(gRandom);
      e.pmu = std::max( 0., e.enu*(0.95 + 0.05*gRandom->Gaus(0, 1)) - 0.1 );
      e.pp = 0.1*fShape1->GetRandom(gRandom);

      // how has it been detected?
      buffer = gRandom->Rndm();
      if     ( buffer < 0.8  ){ e.selection =  1; }
      else if( buffer < 0.85 ){ e.selection =  2; }
      else if( buffer < 0.95 ){ e.selection =  3; }
      else                    { e.selection = -1; } // invisible
    }
    else {
      e.enu = fShape2->GetRandom(gRandom);
      e.pmu = std::max( 0., e.enu*(0.8 + 0.1*gRandom->Gaus(0, 1)) - 0.2 );
      e.pp = 0.1*fShape2->GetRandom(gRandom);

      buffer = gRandom->Rndm();
      if     ( buffer < 0.15 ){ e.selection =  1; }
      else if( buffer < 0.2  ){ e.selection =  2; }
      else if( buffer < 0.85 ){ e.selection =  3; }
      else if( buffer < 0.95 ){ e.selection =  4; }
      else                    { e.selection = -1; }
    }
    // Generate random response-function values for this event
    fillSmoothRandomResponse(par_x, par1_y);
    fillSmoothRandomResponse(par_x, par2_y);
    fillRandomResponse(par3_y);

    // Reuse the TClonesArray memory and construct one TGraph per parameter
    e.par1.Clear("C");
    e.par2.Clear("C");
    e.par3.Clear("C");

    auto* gPar1 = new (e.par1[0]) TGraph(nKnots, par_x, par1_y);
    auto* gPar2 = new (e.par2[0]) TGraph(nKnots, par_x, par2_y);
    auto* gPar3 = new (e.par3[0]) TGraph(nKnots, par_x, par3_y);

    gPar1->SetName("par1_response");
    gPar2->SetName("par2_response");
    gPar3->SetName("par3_response");

    tree->Fill();
  }


  file->WriteObject(tree, "events");
  file->Close();
  
  TString fullPath = TString(gSystem->WorkingDirectory()) + "/" + file->GetName();
  fullPath.ReplaceAll("/./", "/");

  std::cout << "File written to: " << fullPath << std::endl;

  exit(0);
}
