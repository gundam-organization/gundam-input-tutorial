
int nEvents{int(1E6)};

struct Event{
  int reaction{-1};
  int topology{-1};
  double enu{std::nan("unset")};
  double pmu{std::nan("unset")};
  double pp{std::nan("unset")};
  double asimovWeight{0.05};
};

void buildInputRootFile(){

  gRandom = new TRandom3(202404); // Initialize with a UUID;

  auto* file = TFile::Open("./inputs/fitter/likelihood/datasets/mydataset.root", "RECREATE");
  auto* tree = new TTree("events", "events");

  Event e{};

  tree->Branch("enu", &e.enu);
  tree->Branch("pmu", &e.pmu);
  tree->Branch("pp", &e.pp);
  tree->Branch("reaction", &e.reaction);
  tree->Branch("topology", &e.topology);
  tree->Branch("asimovWeight", &e.asimovWeight);

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
      if     ( buffer < 0.8  ){ e.topology =  1; }
      else if( buffer < 0.85 ){ e.topology =  2; }
      else if( buffer < 0.95 ){ e.topology =  3; }
      else                    { e.topology = -1; } // invisible
    }
    else {
      e.enu = fShape2->GetRandom(gRandom);
      e.pmu = std::max( 0., e.enu*(0.8 + 0.1*gRandom->Gaus(0, 1)) - 0.2 );
      e.pp = 0.1*fShape2->GetRandom(gRandom);

      buffer = gRandom->Rndm();
      if     ( buffer < 0.15 ){ e.topology =  1; }
      else if( buffer < 0.2  ){ e.topology =  2; }
      else if( buffer < 0.85 ){ e.topology =  3; }
      else if( buffer < 0.95 ){ e.topology =  4; }
      else                    { e.topology = -1; }
    }

    tree->Fill();
  }


  file->WriteObject(tree, "events");
  file->Close();
  std::cout << "File writen in " << file->GetPath() << std::endl;

  exit(0);
}


