# Advanced GUNDAM Example

Demonstrates the full modular GUNDAM configuration structure, where the fitter
configuration is divided across multiple YAML files organized by concern.
This package is intended for users who want to understand how a larger analysis
is structured and how configuration responsibilities are separated.

All commands are run from the **repository root**.

---

## Configuration structure

```
example/advanced/simpleFit/
├── mainConfig.yaml          Top-level fitter entry point
└── inputs/
    ├── datasets/            Dataset definition (ROOT file path, tree, selection cut, weights)
    ├── samples/             Sample definitions and 2D sample binning tables
    ├── parameters/          Parameter sets: flux (covariance-matrix) and free normalization
    └── plots/               Plot generator configuration
```

`mainConfig.yaml` is the single entry point. It references each sub-config by path
so that each concern (dataset loading, sample binning, parameter definitions,
plotting) can be read and modified independently.

---

## Setup

See [Building the input files](../../README.md#building-the-input-files) in the root README before running the fitter.

---

## Running

```
gundamFitter -c example/advanced/simpleFit/mainConfig.yaml
```

---

## Input organization

See [Input Organization: Flat vs. Modular](../../README.md#input-organization-flat-vs-modular) in the root README.
