void drawResponseFunctions(
    const std::string& fileName = "../mydataset.root",
    int nEventsToDraw = 5
)
{
  auto* file = TFile::Open(fileName.c_str(), "READ");
  auto* tree = dynamic_cast<TTree*>(file->Get("events"));

  TClonesArray* par1 = nullptr;
  TClonesArray* par2 = nullptr;
  TClonesArray* par3 = nullptr;

  tree->SetBranchAddress("par1_TGraph", &par1);
  tree->SetBranchAddress("par2_TGraph", &par2);
  tree->SetBranchAddress("par3_TGraph", &par3);

  auto* c = new TCanvas("c", "Response functions", 900, 700);

  const std::string outPdf = "response_functions.pdf";

  // Open multi-page PDF
  c->Print((outPdf + "[").c_str());

  int nEntries = tree->GetEntries();
  int nDraw = std::min(nEventsToDraw, nEntries);

  for(int iEvent = 0; iEvent < nDraw; ++iEvent){

    tree->GetEntry(iEvent);

    auto* g1 = dynamic_cast<TGraph*>(par1->At(0));
    auto* g2 = dynamic_cast<TGraph*>(par2->At(0));
    auto* g3 = dynamic_cast<TGraph*>(par3->At(0));

    if(!g1 || !g2 || !g3){
      std::cerr << "Missing TGraph for event " << iEvent << std::endl;
      continue;
    }

    c->Clear();

    g1->SetTitle(Form("Response functions, event %d;Parameter shift [#sigma];Response / weight", iEvent));

    g1->SetMarkerStyle(20);
    g2->SetMarkerStyle(21);
    g3->SetMarkerStyle(22);

    g1->SetLineWidth(2);
    g2->SetLineWidth(2);
    g3->SetLineWidth(2);

    g1->SetMarkerColor(kRed + 1);
    g2->SetMarkerColor(kBlue + 1);
    g3->SetMarkerColor(kGreen + 1);

    g1->SetLineColor(kRed + 1);
    g2->SetLineColor(kBlue + 1);
    g3->SetLineColor(kGreen + 1);

    g1->SetMinimum(0.0);
    g1->SetMaximum(3);

    g1->Draw("ALP");
    g2->Draw("LP SAME");
    g3->Draw("LP SAME");

    auto* leg = new TLegend(0.65, 0.72, 0.88, 0.88);
    leg->AddEntry(g1, "par1", "lp");
    leg->AddEntry(g2, "par2", "lp");
    leg->AddEntry(g3, "par3", "lp");
    leg->Draw();

    c->Print(outPdf.c_str());
  }

  // Close multi-page PDF
  c->Print((outPdf + "]").c_str());

  file->Close();

  std::cout << "Saved response functions to: " << outPdf << std::endl;
}
