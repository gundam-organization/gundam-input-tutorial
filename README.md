# Input Tutorial for GUNDAM

## Tutorial structure

The tutorial is organized into three progressive packages under `example/`:

```text
example/
├── basic/      Steps 01–03: dataset loading, selection, one normalization parameter
├── extended/   Steps 04–10: multiple selections, spline dials, correlated parameters,
│               interpolation method comparison
└── advanced/   Full modular configuration (simpleFit/ sub-tree)

mockDatasetGen/ Mock dataset generator (buildInputRootFile.C)
plotting/       Response-function inspection macros
```

The entry point for the full modular run is `example/advanced/simpleFit/mainConfig.yaml`.

---

## Environment setup

For installing GUNDAM itself (cloning, building, directory layout), follow
the official [GUNDAM Getting Started guide](https://gundam-organization.github.io/gundam/GettingStarted.html) —
that's the source of truth, not this repo.

`env.sh` at the repository root loads ROOT and puts `gundamFitter` on `PATH`
for you. It follows the official guide's own directory convention
(`WORK_DIR` / `REPO_DIR` / `BUILD_DIR` / `INSTALL_DIR`), so **if you installed
GUNDAM by following that guide as-is, `env.sh` should work with no edits.**
It only needs editing if your install deviates from the guide's defaults
(different `WORK_DIR`, a custom `CMAKE_INSTALL_PREFIX`, etc.) — the lines to
change are marked `EDIT IF YOUR INSTALL DIFFERS` in the file. The `cd` at the
bottom needs no editing regardless of where you cloned this tutorial repo —
it resolves itself at runtime.

Source it before running any of the commands below:

```bash
source env.sh
```

---

## Building the input files

Generate the mock ROOT dataset before running any fitter step.
Run the build macro from the repository root, then move the output to the expected location:

```bash
root mockDatasetGen/buildInputRootFile.C
mv mydataset.root mockDatasetGen/mydataset.root
```

Every config across `basic/`, `extended/`, and `advanced/` reads the dataset from
`./mockDatasetGen/mydataset.root` (relative to the repository root), so this is
the one location it needs to end up in.

The expected output from the build macro:

```
Processing mockDatasetGen/buildInputRootFile.C...
File written to: /path/to/gundam-input-tutorial_new/mydataset.root
```

---

## Input Organization: Flat vs. Modular

The `basic/` and `extended/` packages keep their small `.txt` binning and selection
tables right next to each config file.
Every step is fully standalone and can be run in isolation without touching any
other package — the only shared resource is the ROOT dataset in
`mockDatasetGen/mydataset.root`.

The `advanced/` package mirrors how a real large-scale analysis is organized:
configuration is split across a dedicated `inputs/` sub-tree (datasets, samples,
parameters, plots), with a single `mainConfig.yaml` entry point that references each
sub-config by path.
This separation makes each concern independently readable and editable.

---

## Running GUNDAM

### Basic examples (steps 01–03)

```bash
gundamFitter -d -c example/basic/B01_load_dataset.yaml
gundamFitter -d -c example/basic/B02_selection.yaml
gundamFitter -c example/basic/B03_normparam.yaml
gundamFitter -c example/basic/B04_datafit.yaml

```

### Extended examples (steps 04–10)

```bash
gundamFitter -d -c example/extended/E01_multiple_selections.yaml
gundamFitter -c example/extended/E02_multiple_normparam.yaml
gundamFitter -c example/extended/E03_response.yaml
gundamFitter -c example/extended/E04_correlated_normparam.yaml
gundamFitter -c example/extended/E05_spline_natural.yaml
gundamFitter -c example/extended/E06_spline_not_a_knot.yaml
gundamFitter -c example/extended/E07_graph_interpolation.yaml
```

### Full modular run

```bash
gundamFitter -a -c example/advanced/simpleFit/mainConfig.yaml -t 4
```

---

## Dial types reference

### Common options (all dial types)

| Option | Description |
|---|---|
| `dialType` | One of: `Normalization`, `Spline`, `Graph`, `Surface`, `Tabulated`, `Kriged`, `Formula` |
| `options` | Variant within a type (e.g. `MonotonicSpline`, `Bilinear`) |
| `isEnabled` | `true` / `false` — skip this dial set without removing it |
| `applyOnDataSets` | List of dataset names; `["*"]` matches all |
| `applyCondition` | Single ROOT formula string; dial only applies when true |
| `applyConditions` | List of ROOT formula strings (all must be true) |
| `minDialResponse` | Hard floor on the computed weight |
| `maxDialResponse` | Hard ceiling on the computed weight |
| `binningFilePath` | Apply dial only within specific bins |
| `useMirrorDial` | Reflect the dial shape across an edge |
| `mirrorLowEdge` | Lower mirror boundary |
| `mirrorHighEdge` | Upper mirror boundary |
| `allowDialExtrapolation` | Extrapolate beyond the defined parameter range |
| `printDialsSummary` | Print dial summary table at startup |
| `definitionRange` | Parameter range over which the dial is defined |
| `mirrorDefinitionRange` | Definition range used for the mirrored side |
| `dialInputList` | Explicit list of dial input objects (alternative to `treeExpression`) |
| `dialsTreePath` | Path inside a ROOT file to the tree holding dial objects |

### Type-specific options

#### Normalization
| Option | Description |
|---|---|
| `parametersBinningPath` | Optional `.txt` binning file for bin-by-bin normalisation |

#### Spline
| Option | Description |
|---|---|
| `treeExpression` | Branch name of `TClonesArray<TGraph>` in the event tree |
| `dialsFilePath` | External ROOT file holding spline objects |
| `dialsList` | Explicit list of spline object names inside `dialsFilePath` |
| `options` | `Uniform` \| `General` \| `Monotonic` \| `Akima` |

#### Graph
| Option | Description |
|---|---|
| `treeExpression` | Branch name of `TClonesArray<TGraph>` in the event tree |
| `dialsFilePath` | External ROOT file holding graph objects |
| `dialsList` | Explicit list of graph object names inside `dialsFilePath` |

#### Surface
| Option | Description |
|---|---|
| `treeExpression` | Branch name identifying which surface bin each event falls in |
| `dialsFilePath` | External ROOT file holding the 2-D surface |
| `dialsList` | Explicit list of surface object names |
| `options` | `Bilinear` \| `Bicubic` |

#### Tabulated
| Option | Description |
|---|---|
| `tableConfig` | Config block pointing to the pre-computed weight table |

#### Kriged
| Option | Description |
|---|---|
| `tableConfig` | Config block pointing to the kriging data source |

#### Formula
| Option | Description |
|---|---|
| *(formula string)* | ROOT `TFormula` expression evaluated per event; set via the `formula` key. Not GPU-accelerated — avoid in large-scale fits. |
