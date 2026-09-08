# Extended GUNDAM Example

Builds on the Basic tutorial. Demonstrates multiple concurrent selections,
multiple free normalization parameters, spline response dials, correlated
normalization parameters driven by a covariance matrix, and a comparison of
interpolation methods (Spline natural, Spline not-a-knot, Graph) on the same
non-smooth response function.

All commands are run from the **repository root**.

---

## Files in this package

| File | Purpose |
|---|---|
| `selection_1d.txt` | 1D binning in pmu (6 bins) — used by Extended01–Extended07 |
| `selection_2d.txt` | 2D binning in pmu × pp (33 bins) — used by Extended01–Extended07 |
| `selection_3d.txt` | 3D binning in enu × pmu × pp (8 bins) — used by Extended01–Extended07 |
| `parameters.txt` | Free normalization parameter binning (reaction × pmu, 10 bins) — used by Extended02–Extended03, Extended05–Extended07 |
| `correlated_parameters.txt` | Parameter binning for the correlated set (enu, 11 bins) — used by Extended04–Extended07 |
| `covarianceFile.root` | Covariance matrix ROOT file for the correlated parameter set — used by Extended04–Extended07 |

---

### Extended01 — Multiple selections

```
gundamFitter -d -c example/extended/E01_multiple_selections.yaml
```

Defines three concurrent samples, each with a different binning dimensionality:

- Selection 1 uses `selection_1d.txt` (1D: pmu)
- Selection 2 uses `selection_2d.txt` (2D: pmu × pp)
- Selection 3 uses `selection_3d.txt` (3D: enu × pmu × pp)

No parameters are defined. Run a dry run using the option `-d`: with zero
free parameters there is nothing for the minimizer to fit, so a full fit
would error out.

### Extended02 — Multiple free normalization parameters

```
gundamFitter -c example/extended/E02_multiple_normparam.yaml
```

Adds 10 free normalization parameters generated from `parameters.txt`.
Each row in `parameters.txt` defines one parameter bin in reaction × pmu space.

### Extended03 — Spline response example

```
gundamFitter -c example/extended/E03_response.yaml
```

Extends step 05 with a Spline dial (`dialType: Spline`, `options: not-a-knot`)
reading the per-event response function from the `par1_TGraph` branch.

### Extended04 — Correlated normalization parameters

```
gundamFitter -c example/extended/E04_correlated_normparam.yaml
```

Replaces free normalization parameters with a covariance-matrix-driven set.
The parameter binning is read from `correlated_parameters.txt` (enu, 11 bins)
and the covariance matrix is read from `covarianceFile.root`. See
[Regenerating the covariance file](#regenerating-the-covariance-file) below
for how that file is built and what format GUNDAM expects.

### Extended05 — Spline (natural) interpolation

```
gundamFitter -c example/extended/E05_spline_natural.yaml
```

Adds a `Dummy Systematics` parameter set with a Spline dial (`dialType: Spline`,
`options: natural`) reading the intentionally non-smooth response function
from the `par3_TGraph` branch, on top of the free and correlated normalization
parameters from Extended02–Extended04.

### Extended06 — Spline (not-a-knot) interpolation

```
gundamFitter -c example/extended/E06_spline_not_a_knot.yaml
```

Same as Extended05, but the `par3_TGraph` dial uses `options: not-a-knot`
instead of `natural`.

### Extended07 — Graph interpolation

```
gundamFitter -c example/extended/E07_graph_interpolation.yaml
```

Same as Extended05, but the `par3_TGraph` dial uses `dialType: Graph`
(piecewise-linear interpolation, no `options`) instead of a Spline.

Together, Extended05–Extended07 compare post-fit uncertainty across three
interpolation methods applied to the same intentionally non-smooth
systematic (`par3_TGraph`). See `mockDatasetGen/plotting/plot_interp_comparison.C`
for a script that plots the comparison after all three fits have been run.

---

## Regenerating the covariance file

`covarianceFile.root` (used by Extended04–Extended07) is a ROOT file holding
a single object:

| ROOT key | Class | Meaning |
|---|---|---|
| `covarianceMatrix` | `TMatrixT<double>` | The 11×11 covariance matrix for the 11 `enu`-binned correlated normalization parameters defined in `correlated_parameters.txt`. Row/column order matches that file's bin order (top to bottom = index 0 to 10). |

This is the only object GUNDAM reads for this parameter set — the yaml
(`covarianceMatrix: "covarianceMatrix"`) points straight at it. There
is no embedded parameter-name or prior-value object in this file. GUNDAM
falls back to its defaults when they're absent: parameters are auto-numbered
`#0`–`#10` by row/column index (you'll see this in fit output, e.g.
`Correlated normalization parameters/#0`), and every prior defaults to `1.0`.

GUNDAM does support two additional, fully optional objects in a covariance
file, if you want named parameters or non-default priors instead of relying
on those defaults:

| ROOT key (your choice of string) | Class | yaml field | Purpose |
|---|---|---|---|
| e.g. `parameterNameList` | `TObjArray` of `TObjString`, written with `TObject::kSingleKey` | `parameterNameList` | Human-readable name per parameter, instead of auto-numbered `#i` |
| e.g. `parameterPriorValueList` | `TVectorT<double>` | `parameterPriorValueList` | Prior value per parameter, instead of the `1.0` default |

`example/extended/makeCovFile.C` is a worked example showing how to build a
file like this from scratch — it reproduces `covarianceFile.root`'s
`covarianceMatrix` key exactly (same class, same 11×11 values, same order),
and additionally demonstrates the two optional name/prior objects above
(populated with synthesized bin-edge names and the `1.0` default, since
neither is actually present in the tracked file). It writes to a separate
file, `covarianceFile_regenerated.root`, and never touches the tracked
`covarianceFile.root`. Run it with:

```
root example/extended/makeCovFile.C
```

---

## Input organization

See [Input Organization: Flat vs. Modular](../../README.md#input-organization-flat-vs-modular) in the root README.
