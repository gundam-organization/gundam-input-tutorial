

void buildInputRootFile(){

  auto* generatedInputFile = TFile::Open("./inputs/fitter/likelihood/datasets/input.root", "RECREATE");



  generatedInputFile->Close();

}


