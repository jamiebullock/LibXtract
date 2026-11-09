# Examples

Each directory holds one runnable program that uses the library on a kind of data, with a README that explains the task, the input the program expects and which functions it calls. The examples build with the library when `XTRACT_BUILD_EXAMPLES` is on (the default for a top-level build) and run as CTest entries, so `make check` runs them.

| Directory | Data | What it shows |
| --- | --- | --- |
| [`simpletest`](simpletest) | audio (`test.wav`) | pitch tracking, spectrum, MFCCs, spectral flux and onset detection over overlapping frames |
| [`finance`](finance) | a daily price series | rolling and exponentially weighted volatility, detection of a volatility regime change |

Every non-audio example generates its own data when run without arguments, so it needs no files and its result is checked by CTest. Give it `--input <file>` to run the same analysis on your own series; the format each program reads is in its README.

Build and run from the repository root:

```sh
cmake --preset default
cmake --build --preset default
build/default/examples/finance/volatility
build/default/examples/finance/volatility --input prices.csv
```
