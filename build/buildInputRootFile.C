

void buildInputRootFile(){

  auto* file = TFile::Open("./inputs/fitter/likelihood/datasets/input.root", "RECREATE");
  auto* tree = TTree::TTree("events", "events");


  generatedInputFile->Close();

}


