# Input Tutorial for GUNDAM



## Building the input files

```bash
root build/buildInputRootFile.C
```

The output should look like:

```
Processing build/buildInputRootFile.C...
File writen in ./inputs/fitter/likelihood/datasets/mydataset.root:/
```


## Running GUNDAM

### Running the fitter

```bash
gundamFitter -c inputs/myFit.yaml -t 4
```

