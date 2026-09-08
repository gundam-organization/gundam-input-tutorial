# Basic GUNDAM Example

Introductory tutorial. Three progressive steps, each building on the previous one.

All commands are run from the **repository root**.

---

## Files in this package

| File | Purpose |
|---|---|
| `selection_1d.txt` | Local 1D binning definition (pmu, 6 bins) used by Basic01–Basic04 |

---

### Basic01 — Load dataset

```
gundamFitter -d -c example/basic/B01_load_dataset.yaml
```

Loads the mock dataset and verifies that the event tree can be read.
Applies the dataset-level cut (`selection != -1`) with no further selection —
all topologies are kept together in a single sample, binned in 1D (pmu) using
the local `selection_1d.txt` file. No parameters are defined.
Run a dry run using the option `-d`: with zero free parameters there is
nothing for the minimizer to fit, so a full fit would error out.

### Basic02 — Add event selection

```
gundamFitter -d -c example/basic/B02_selection.yaml
```

Adds one event selection (`selection == 1`) with 1D binning in muon momentum.
The binning is read directly from the local file `selection_1d.txt`.
No parameters are defined.
Run a dry run using the option `-d`

### Basic03 — Add normalization parameter

```
gundamFitter -c example/basic/B03_normparam.yaml
```

Adds one free normalization parameter defined directly in the yaml under `parameterDefinitions`.
The parameter is named `Free normalization`; it applies a uniform weight multiplier
(`dialType: Normalization`) to every event matched by the selection, regardless of kinematics —
a single scale factor for the whole sample.
Its prior is flat at nominal value 1.0 with step size 0.1, meaning the fitter starts at 1
(no change to event counts) and is free to float in either direction with no Gaussian pull.
This is the simplest possible systematic parameter: one number, no external binning file,
and it is introduced here as the foundation for all dial studies that follow. The Asimov fit is performed in this example.

### Basic04 — Real data fit

```
gundamFitter -c example/basic/B04_datafit.yaml
```

Extends B03 by enabling a real data-vs-MC fit: `selectedDataEntry: "data"` tells GUNDAM
to load the event tree as observed data instead of using the Asimov approximation.
The normalization parameter floats freely to minimize the likelihood between the data
histogram and the MC prediction, so its post-fit value will differ from the prior whenever
the raw event counts disagree with the `asimovWeight`-normalized MC.

---

## Input organization

See [Input Organization: Flat vs. Modular](../../README.md#input-organization-flat-vs-modular) in the root README.
