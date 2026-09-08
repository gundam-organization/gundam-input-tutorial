# Response Function Plotting

This folder contains ROOT C++ macros for inspecting the response functions stored in the mock input dataset.

## drawResponseFunctions.C

`drawResponseFunctions.C` reads event-by-event systematic response functions from the `events` TTree in:

```text
example/advanced/simpleFit/inputs/datasets/mydataset.root
```

The response functions are stored in the following branches:

- `par1_TGraph`
- `par2_TGraph`
- `par3_TGraph`

From the repository root, run:

```bash
root -l 'plotting/drawResponseFunctions.C("example/advanced/simpleFit/inputs/datasets/mydataset.root",5)'
```

The second argument specifies the number of events to plot. In this example, the macro draws the response functions for the first five events.

The macro creates the multi-page PDF:

```text
response_functions.pdf
```

The response functions for `par1` and `par2` are smooth. The response function for `par3` is intentionally non-smooth so that different interpolation methods can be tested and compared.

---

## plot_interp_comparison.C

Compares post-fit uncertainty (σ) on the `par3_TGraph` systematic parameter across three
GUNDAM interpolation dial types, using the actual Hesse errors extracted from GUNDAM fit outputs:

| Dial type | Sub-type |
|---|---|
| `Spline` | `natural` |
| `Spline` | `not-a-knot` |
| `Graph` | *(piecewise linear)* |

The three fit outputs must already exist (run the three fits first):

```bash
gundamFitter -c example/extended/E05_spline_natural.yaml
gundamFitter -c example/extended/E06_spline_not_a_knot.yaml
gundamFitter -c example/extended/E07_graph_interpolation.yaml
```

Then, from the repository root:

```bash
root -l -q 'plotting/plot_interp_comparison.C'
```

Produces a bar chart on a log y-scale showing the post-fit σ for each method,
with the prior σ = 1.0 drawn as a reference line.

Output:

```text
example/extended/output/plots/interp_comparison.pdf
```