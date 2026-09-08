// plotting/plot_interp_comparison.C
//
// Bar chart (log y-scale): post-fit uncertainty of the par3_TGraph systematic
// parameter across three GUNDAM interpolation dial types:
//   Spline (natural cubic), Spline (not-a-knot cubic), Graph (piecewise linear)
//
// Run from REPOSITORY ROOT after the three fits have been run:
//   gundamFitter -c example/extended/E05_spline_natural.yaml
//   gundamFitter -c example/extended/E06_spline_not_a_knot.yaml
//   gundamFitter -c example/extended/E07_graph_interpolation.yaml
//
//   root -l -q 'plotting/plot_interp_comparison.C'
//
// Output: example/extended/output/plots/interp_comparison.pdf

#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TStyle.h>
#include <TSystem.h>
#include <iostream>

// Post-fit sigma is stored as GetBinError(1) of postFitErrors_TH1D.
double readPostFitSigma(const char* path) {
    TFile* f = TFile::Open(path, "READ");
    if (!f || f->IsZombie()) { std::cerr << "ERROR: cannot open " << path << std::endl; return -1; }
    TH1D* h = (TH1D*)f->Get(
        "FitterEngine/postFit/Hesse/errors/Dummy Systematics/values/postFitErrors_TH1D");
    if (!h) { std::cerr << "ERROR: histogram not found in " << path << std::endl; f->Close(); return -1; }
    double sigma = h->GetBinError(1);
    f->Close();
    return sigma;
}

void plot_interp_comparison() {
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    // ── Read post-fit sigmas ─────────────────────────────────────────────────
    const int kN = 3;
    const char* kFiles[kN] = {
        "example/extended/output/gundamFitter_E05_spline_natural.root",
        "example/extended/output/gundamFitter_E06_spline_not_a_knot.root",
        "example/extended/output/gundamFitter_E07_graph_interpolation.root"
    };
    const char* kLabels[kN] = {
        "Spline (natural)",
        "Spline (not-a-knot)",
        "Graph (linear)"
    };
    const int kFillColor[kN] = { kRed-7,  kBlue-7, kGreen-7 };
    const int kLineColor[kN] = { kRed+1,  kBlue+1, kGreen+2 };

    double sigma[kN];
    for (int i = 0; i < kN; ++i) {
        sigma[i] = readPostFitSigma(kFiles[i]);
        std::cout << kLabels[i] << "  ->  sigma = " << sigma[i] << std::endl;
    }

    // ── Canvas (log y scale) ─────────────────────────────────────────────────
    TCanvas* c = new TCanvas("c_interp", "Interpolation comparison", 1100, 640);
    c->SetLeftMargin(0.14);
    c->SetRightMargin(0.27);
    c->SetTopMargin(0.12);
    c->SetBottomMargin(0.22);
    c->SetLogy();

    // ── Bar histogram ────────────────────────────────────────────────────────
    TH1D* h = new TH1D("h", "", kN, 0.5, kN + 0.5);
    for (int i = 0; i < kN; ++i) {
        h->GetXaxis()->SetBinLabel(i + 1, kLabels[i]);
        h->SetBinContent(i + 1, sigma[i]);
    }
    h->SetMinimum(0.003);
    h->SetMaximum(5.0);
    h->GetYaxis()->SetTitle("Post-fit uncertainty (#sigma)");
    h->GetYaxis()->SetTitleSize(0.053);
    h->GetYaxis()->SetLabelSize(0.046);
    h->GetYaxis()->SetTitleOffset(1.10);
    h->GetXaxis()->SetLabelSize(0.058);
    h->GetXaxis()->SetLabelOffset(0.015);
    h->GetXaxis()->SetTickLength(0.0);
    h->SetFillColor(kFillColor[0]);
    h->SetLineColor(kLineColor[0]);
    h->SetLineWidth(2);
    h->Draw("bar2");

    // Overlay bars 1 and 2 with individual colors
    for (int i = 1; i < kN; ++i) {
        TH1D* t = new TH1D(TString::Format("t%d", i), "", kN, 0.5, kN + 0.5);
        t->SetBinContent(i + 1, sigma[i]);
        t->SetFillColor(kFillColor[i]);
        t->SetLineColor(kLineColor[i]);
        t->SetLineWidth(2);
        t->Draw("bar2 same");
    }
    // Repaint bar 0 so it isn't hidden by bar 1's overlay
    TH1D* h0b = new TH1D("h0b", "", kN, 0.5, kN + 0.5);
    h0b->SetBinContent(1, sigma[0]);
    h0b->SetFillColor(kFillColor[0]);
    h0b->SetLineColor(kLineColor[0]);
    h0b->SetLineWidth(2);
    h0b->Draw("bar2 same");
    h->Draw("axis same");

    // ── Prior reference line ─────────────────────────────────────────────────
    TLine* priorLine = new TLine(0.5, 1.0, kN + 0.5, 1.0);
    priorLine->SetLineStyle(2);
    priorLine->SetLineWidth(2);
    priorLine->SetLineColor(kGray + 2);
    priorLine->Draw();

    // ── Value labels above each bar (multiplicative offset for log scale) ────
    TLatex tex;
    tex.SetTextAlign(21);
    tex.SetTextFont(42);
    tex.SetTextSize(0.050);
    for (int i = 0; i < kN; ++i) {
        TString val = TString::Format("#sigma = %.4f", sigma[i]);
        tex.DrawLatex(i + 1, sigma[i] * 1.35, val);
    }

    // ── Title ────────────────────────────────────────────────────────────────
    TLatex title;
    title.SetNDC();
    title.SetTextAlign(22);
    title.SetTextFont(62);
    title.SetTextSize(0.052);
    title.DrawLatex(0.415, 0.943,
        "par3_{TGraph} post-fit #sigma by interpolation method");

    // ── Legend ───────────────────────────────────────────────────────────────
    TLegend* leg = new TLegend(0.74, 0.52, 0.98, 0.87);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.043);
    leg->AddEntry(h0b,                                   kLabels[0], "F");
    leg->AddEntry((TObject*)gDirectory->FindObject("t1"), kLabels[1], "F");
    leg->AddEntry((TObject*)gDirectory->FindObject("t2"), kLabels[2], "F");
    leg->AddEntry(priorLine, "Prior #sigma = 1.0", "L");
    leg->Draw();

    // ── Save ─────────────────────────────────────────────────────────────────
    gSystem->mkdir("example/extended/output/plots", kTRUE);
    const char* outPDF = "example/extended/output/plots/interp_comparison.pdf";
    c->Print(outPDF);
    std::cout << "\nSaved => " << outPDF << std::endl;
}
