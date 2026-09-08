//  -------------------------------------------------------
//  makeCovFile.C
//
//  Worked example: how to build a covariance-matrix input file for a
//  GUNDAM "correlated normalization parameters" set, in the exact format
//  read by example/extended/E04_correlated_normparam.yaml (and E05-E07,
//  which reuse the same block):
//
//    parameterDefinitionFilePath: "./example/extended/covarianceFile.root"
//    covarianceMatrix:            "covarianceMatrix"
//    parametersBinningPath:       "./example/extended/correlated_parameters.txt"
//
//  IMPORTANT: this writes to a DIFFERENT file (covarianceFile_regenerated.root)
//  and never touches the tracked example/extended/covarianceFile.root, which
//  remains the ground truth GUNDAM actually reads.
//
//  What this script reproduces vs. adds (see README.md "Regenerating the
//  covariance file" section for the full explanation):
//    - The `covarianceMatrix` key/class/dimensions/values are a byte-for-byte
//      match of the tracked file (11x11 TMatrixT<double>, values extracted
//      directly from it) -- this IS what E04-E07 actually read.
//    - The `parameterNameList` (TObjArray, single key) and
//      `parameterPriorValueList` (TVectorT<double>) keys are NOT present in
//      the tracked covarianceFile.root at all. GUNDAM supports reading them
//      (ParameterSet.cpp fields `parameterNameList` /
//      `parameterPriorValueList`, aliased from the deprecated names
//      `parameterNameTObjArray` / `parameterPriorTVectorD`), but E04-E07's
//      yaml never asks for them, so
//      GUNDAM falls back to its defaults: parameters auto-numbered "#0".."#10"
//      by row/column index, and every prior defaulting to 1.0 (confirmed in
//      GUNDAM source, ParameterSet.cpp: `else{ par.setPriorValue(1); }`).
//      They're included here purely as an optional, fully-documented extra,
//      for reference if you want to name/re-prior parameters in your own file.
//
//  Run with: root example/extended/makeCovFile.C
//  -------------------------------------------------------

#include <TFile.h>
#include <TMath.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TMatrixT.h>
#include <TVectorT.h>
#include <iostream>

void makeCovFile(const char* outfile = "example/extended/covarianceFile_regenerated.root")
{
    // ------------------------------------------------------------------ //
    // 1. Parameter identity
    // ------------------------------------------------------------------ //
    // correlated_parameters.txt defines 11 parameters as bins in enu (GeV),
    // read top-to-bottom as row/column 0..10 of the covariance matrix below.
    // The bin edges themselves carry no "name" field in that file -- GUNDAM
    // just auto-labels them "#0".."#10" -- so the names here are synthesized
    // from the bin edges purely for readability in this optional array.
    const char* names[] = {
        "enu_0.0_0.5",
        "enu_0.5_1.0",
        "enu_1.0_1.5",
        "enu_1.5_2.0",
        "enu_2.0_2.5",
        "enu_2.5_3.0",
        "enu_3.0_3.5",
        "enu_3.5_4.0",
        "enu_4.0_5.0",
        "enu_5.0_7.0",
        "enu_7.0_30.0",
    };
    constexpr int N = sizeof(names) / sizeof(names[0]);

    auto* nameArray = new TObjArray(N);
    nameArray->SetName("parameterNameList");
    for (int i = 0; i < N; ++i) {
        nameArray->Add(new TObjString(names[i]));
    }

    // ------------------------------------------------------------------ //
    // 2. Prior values
    // ------------------------------------------------------------------ //
    // E04-E07 don't supply a prior vector, so GUNDAM defaults every one of
    // these parameters to a prior of 1.0. Reproduced explicitly here.
    double parVals[N];
    for (int i = 0; i < N; ++i) parVals[i] = 1.0;
    auto* prior = new TVectorT<double>(N, parVals);

    // ------------------------------------------------------------------ //
    // 3. Covariance matrix
    // ------------------------------------------------------------------ //
    // These are the exact values extracted from the tracked
    // example/extended/covarianceFile.root (row/column order matches
    // correlated_parameters.txt). There is no known formula/generator for
    // this matrix in this repo -- unlike a toy diagonal example, it encodes
    // real bin-to-bin correlations (correlation generally falls off with
    // enu distance, and the last/widest bin [7, 30] carries the largest
    // variance) -- so it is copied rather than recomputed from a model.
    static const double covValues[N][N] = {
        {0.0035120471828324813, 0.0025823185234341169, 0.0023903856840913310, 0.0020689644022447930, 0.0018500766808639162, 0.0014853443380161686, 0.0011299359150295256, 0.0014285066032853186, 0.0014659892116579932, 0.0011289555328770117, 0.0010271262097969692},
        {0.0025823185234341169, 0.0027664970036088386, 0.0026752632895570652, 0.0020556080131491541, 0.0013734109661508401, 0.0008852866252664585, 0.0010863191953816272, 0.0016547745981062444, 0.0016914174169580514, 0.0013653185653072881, 0.0009831027497849505},
        {0.0023903856840913310, 0.0026752632895570652, 0.0028020039389212150, 0.0023577116093222044, 0.0018144578569801674, 0.0010525172551344721, 0.0010379536845824713, 0.0016755931504746605, 0.0018895033759127851, 0.0014139607747457722, 0.0007189646826735039},
        {0.0020689644022447930, 0.0020556080131491541, 0.0023577116093222044, 0.0026558810401004010, 0.0031062166338426851, 0.0025061429043926058, 0.0015455762397199369, 0.0016006340237652566, 0.0020166170150302059, 0.0014452590070056345, 0.0007217954986117287},
        {0.0018500766808639162, 0.0013734109661508401, 0.0018144578569801674, 0.0031062166338426851, 0.0055309061638916879, 0.0056657775353967533, 0.0031689945399444304, 0.0019360150354758630, 0.0025434934639304617, 0.0022566499817226610, 0.0019139063328987879},
        {0.0014853443380161686, 0.0008852866252664585, 0.0010525172551344721, 0.0025061429043926058, 0.0056657775353967533, 0.0077614587664819107, 0.0052336101164785608, 0.0024524064376432782, 0.0022454521449832895, 0.0025313236925562348, 0.0035940923877783417},
        {0.0011299359150295256, 0.0010863191953816272, 0.0010379536845824713, 0.0015455762397199369, 0.0031689945399444304, 0.0052336101164785608, 0.0049264263864918847, 0.0027383302448502284, 0.0019902159019248972, 0.0029675701079667129, 0.0034932045971352997},
        {0.0014285066032853188, 0.0016547745981062444, 0.0016755931504746605, 0.0016006340237652566, 0.0019360150354758626, 0.0024524064376432782, 0.0027383302448502284, 0.0024950024985861473, 0.0021323966542206034, 0.0024065641805413964, 0.0023399448082501908},
        {0.0014659892116579932, 0.0016914174169580514, 0.0018895033759127851, 0.0020166170150302059, 0.0025434934639304617, 0.0022454521449832895, 0.0019902159019248972, 0.0021323966542206034, 0.0025256079305523618, 0.0026269817242228089, 0.0021889812798401033},
        {0.0011289555328770117, 0.0013653185653072881, 0.0014139607747457722, 0.0014452590070056345, 0.0022566499817226610, 0.0025313236925562348, 0.0029675701079667129, 0.0024065641805413964, 0.0026269817242228089, 0.0049365587644154508, 0.0050852178874933665},
        {0.0010271262097969692, 0.0009831027497849505, 0.0007189646826735039, 0.0007217954986117287, 0.0019139063328987879, 0.0035940923877783417, 0.0034932045971353002, 0.0023399448082501908, 0.0021889812798401033, 0.0050852178874933665, 0.0130691784526693190},
    };

    auto* cov = new TMatrixT<double>(N, N);
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            (*cov)(i, j) = covValues[i][j];

    // ------------------------------------------------------------------ //
    // 4. Write to file
    // ------------------------------------------------------------------ //
    TFile f(outfile, "RECREATE");

    // Write the names array as ONE key only:
    //    kSingleKey = do NOT write the eleven sub-objects as individual keys
    nameArray->Write("parameterNameList", TObject::kSingleKey);

    // Write the priors
    f.WriteObject(prior, "parameterPriorValueList");

    // Write the matrix -- this is the key GUNDAM's E04-E07 configs actually
    // read (covarianceMatrix: "covarianceMatrix")
    f.WriteObject(cov, "covarianceMatrix");

    f.Close();

    std::cout << "Wrote " << outfile << " with:" << std::endl
              << "  - covarianceMatrix       (TMatrixT<double>, " << N << "x" << N
              << ") <- what E04-E07's yaml actually reads" << std::endl
              << "  - parameterNameList      (TObjArray, single key)   [optional extra, not in the tracked file]" << std::endl
              << "  - parameterPriorValueList (TVectorT<double>)       [optional extra, not in the tracked file]" << std::endl;
}
