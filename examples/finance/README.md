# Volatility of a price series

`volatility.c` takes a daily price series, turns it into log returns and reports three volatility estimates and the first day on which the volatility regime changes. It is the same computation a risk desk runs on a close-price history, written against the C API with no audio concepts involved.

## Input

Run without arguments, the program generates a 504-day random walk: 252 days with a daily volatility of 0.8%, then 252 days at 2.5%, with a small positive drift. The generator is a fixed-seed linear congruential generator with a Box-Muller transform, so the series is identical on every platform and the result can be checked.

Run with `--input <file>`, it reads one price per line and uses the last comma-separated field of each line, so a plain column of closes works and so does a `date,open,high,low,close` export; lines whose last field is not a positive number, such as a header, are skipped. It needs more than 63 prices.

## Pipeline

1. **Log returns.** `r[n] = ln(p[n] / p[n - 1])`, written in the example: the library works on whatever series it is given and takes no view on what the numbers are.
2. **Return distribution.** `xtract_mean`, `xtract_variance` (which takes the mean in `argv`), `xtract_standard_deviation` (which takes the variance), `xtract_skewness` and `xtract_kurtosis` (which take mean and standard deviation) and `xtract_median`. The kurtosis is the excess kurtosis, 0 for a normal distribution. The feature functions share one signature, `(data, N, argv, result)`, and return a status code; the example stops at the first failure and names the call.
3. **Rolling volatility.** `xtract_moving_standard_deviation` with a window of 21 trading days and again with 63, giving `N - window + 1` sample standard deviations, one per window position. Each is annualised as `sigma * sqrt(252)` and printed as a percentage.
4. **Exponentially weighted volatility.** `xtract_ewma_variance` with a smoothing factor of 0.06, which is the RiskMetrics decay of 0.94 expressed as the weight of the newest return. Its first value is 0 by definition and the estimate settles over the first few weeks.
5. **Regime change.** The first day on which the 21-day volatility exceeds 1.5 times the 63-day volatility. A short window reacts to a change within days while a long one lags, so their ratio spikes at the onset and returns towards 1 once both windows lie in the new regime. On the synthetic series the program checks that this day falls within 21 days after the planted change and exits non-zero otherwise, which is what the CTest entry relies on.

## Reading the output

The table prints one line every 63 trading days: the day number, the price and the three volatility estimates. The two rolling estimates agree in a stable regime and disagree after a change for as long as the longer window still straddles it; the exponentially weighted estimate sits between them, reacting faster than the 63-day window and more smoothly than the 21-day one.

The change detection is a threshold on a ratio, so with independent Gaussian returns it can fire by chance; a 21-sample standard deviation has a relative standard error near 16%, so a ratio of 1.5 in a stable regime is a three-sigma event per window and rare over a two-year series, but not impossible on real data with fat tails. Raise `CHANGE_RATIO` to trade sensitivity for false alarms.

A threshold on the level of volatility against the median of the whole series does not work for this task: with two regimes of similar length the median lands inside one of them and the threshold ends up either never or always exceeded. The ratio of two horizons avoids that because it compares each day with its own recent past.

## Running on your own data

```sh
cmake --preset default
cmake --build --preset default
build/default/examples/finance/volatility --input closes.csv
```

Window lengths, the smoothing factor and the change ratio are the `#define` constants at the top of the file, named for what they are.
