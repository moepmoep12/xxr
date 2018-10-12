# XCS &amp; XCSR
XCS &amp; XCSR (CS & LU representation) implementation in C++14

```
$ git clone https://github.com/m4saka/xcs-xcsr.git && cd xcs-xcsr
$ git submodule update -i
$ make
```

# Examples

## 6-Multiplexer Problem
```
$ ./xcs --mux=6
```

## 6-Real Multiplexer Problem (CS representation)
```
$ ./xcsr --mux=6 --repr=cs
```

## 6-Real Multiplexer Problem (LU representation)
```
$ ./xcsr --mux=6 --repr=lu
```

## From CSV
```
$ ./xcs --csv=dataset.csv --action=0,1
```

## From CSV (with another test dataset)
```
$ ./xcs --csv=dataset.csv --csv-eval=dataset_test.csv --action=0,1
```

## For more details:
```
$ ./xcs --help
```
```
$ ./xcsr --help
```
