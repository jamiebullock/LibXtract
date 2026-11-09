/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xtract/libxtract.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define TRADING_DAYS_PER_YEAR 252
#define SHORT_WINDOW 21
#define LONG_WINDOW 63
#define EWMA_NEW_WEIGHT 0.06
#define CHANGE_RATIO 1.5
#define REPORT_STRIDE 63

#define SYNTHETIC_DAYS 504
#define CALM_DAYS 252
#define CALM_DAILY_SIGMA 0.008
#define TURBULENT_DAILY_SIGMA 0.025
#define DAILY_DRIFT 0.0002
#define START_PRICE 100.0
#define SEED 20261109ULL

static unsigned long long generator_state = SEED;

static double uniform(void)
{
    generator_state = generator_state * 6364136223846793005ULL + 1442695040888963407ULL;
    return ((generator_state >> 11) + 1.0) / 9007199254740994.0;
}

static double standard_normal(void)
{
    double u = uniform();
    double v = uniform();
    return sqrt(-2.0 * log(u)) * cos(2.0 * M_PI * v);
}

static void synthetic_prices(double *prices, int N)
{
    int n;
    prices[0] = START_PRICE;
    for (n = 1; n < N; ++n)
    {
        double sigma = n < CALM_DAYS ? CALM_DAILY_SIGMA : TURBULENT_DAILY_SIGMA;
        prices[n] = prices[n - 1] * exp(DAILY_DRIFT + sigma * standard_normal());
    }
}

static int last_field(const char *line, double *value)
{
    const char *field = strrchr(line, ',');
    char *end;
    field = field ? field + 1 : line;
    *value = strtod(field, &end);
    return end != field && *value > 0.0;
}

static double *read_prices(const char *path, int *N)
{
    FILE *file = fopen(path, "r");
    double *prices = NULL;
    int capacity = 0;
    char line[256];
    *N = 0;
    if (!file)
    {
        fprintf(stderr, "cannot open %s\n", path);
        return NULL;
    }
    while (fgets(line, sizeof line, file))
    {
        double value;
        if (!last_field(line, &value))
            continue;
        if (*N == capacity)
        {
            double *grown;
            capacity = capacity ? 2 * capacity : 256;
            grown = realloc(prices, capacity * sizeof *prices);
            if (!grown)
            {
                free(prices);
                fclose(file);
                return NULL;
            }
            prices = grown;
        }
        prices[(*N)++] = value;
    }
    fclose(file);
    return prices;
}

static void log_returns(const double *prices, int N, double *returns)
{
    int n;
    for (n = 1; n < N; ++n)
        returns[n - 1] = log(prices[n] / prices[n - 1]);
}

static double annualised(double daily_sigma)
{
    return 100.0 * daily_sigma * sqrt(TRADING_DAYS_PER_YEAR);
}

static int day_of_return(int r)
{
    return r + 2;
}

static int check(int status, const char *function)
{
    if (status != XTRACT_SUCCESS)
        fprintf(stderr, "%s failed with status %d\n", function, status);
    return status;
}

static int rolling_volatility(const double *returns, int N, int window, double *volatility)
{
    int status = xtract_moving_standard_deviation(returns, N, &window, volatility);
    int n;
    for (n = 0; n <= N - window; ++n)
        volatility[n] = annualised(volatility[n]);
    return status;
}

static int ewma_volatility(const double *returns, int N, double *volatility)
{
    double weight = EWMA_NEW_WEIGHT;
    int status = xtract_ewma_variance(returns, N, &weight, volatility);
    int n;
    for (n = 0; n < N; ++n)
        volatility[n] = annualised(sqrt(volatility[n]));
    return status;
}

static int first_change(const double *short_volatility, const double *long_volatility, int N)
{
    int r;
    for (r = LONG_WINDOW - 1; r < N; ++r)
    {
        double ratio = short_volatility[r - SHORT_WINDOW + 1] / long_volatility[r - LONG_WINDOW + 1];
        if (ratio > CHANGE_RATIO)
            return r;
    }
    return -1;
}

static int summarise_returns(const double *returns, int N)
{
    double moments[2];
    double variance;
    double skewness;
    double kurtosis;
    double median;
    if (check(xtract_mean(returns, N, NULL, &moments[0]), "xtract_mean") ||
        check(xtract_variance(returns, N, &moments[0], &variance), "xtract_variance") ||
        check(xtract_standard_deviation(returns, N, &variance, &moments[1]), "xtract_standard_deviation") ||
        check(xtract_skewness(returns, N, moments, &skewness), "xtract_skewness") ||
        check(xtract_kurtosis(returns, N, moments, &kurtosis), "xtract_kurtosis") ||
        check(xtract_median(returns, N, NULL, &median), "xtract_median"))
        return EXIT_FAILURE;
    printf("Daily log returns over %d days\n", N);
    printf("  mean %.5f  median %.5f  standard deviation %.5f (%.1f%% annualised)\n",
           moments[0], median, moments[1], annualised(moments[1]));
    printf("  skewness %.3f  excess kurtosis %.3f\n\n", skewness, kurtosis);
    return EXIT_SUCCESS;
}

static void report(const double *prices, const double *short_volatility, const double *long_volatility,
                   const double *ewma, int N)
{
    int r;
    printf("%6s %10s %12s %12s %12s\n", "day", "price", "vol 21d %", "vol 63d %", "ewma %");
    for (r = LONG_WINDOW - 1; r < N; r += REPORT_STRIDE)
        printf("%6d %10.2f %12.1f %12.1f %12.1f\n", day_of_return(r), prices[r + 1],
               short_volatility[r - SHORT_WINDOW + 1], long_volatility[r - LONG_WINDOW + 1], ewma[r]);
    printf("\n");
}

int main(int argc, char **argv)
{
    const char *input = NULL;
    double *prices;
    double *returns;
    double *short_volatility;
    double *long_volatility;
    double *ewma;
    int N;
    int returns_count;
    int change;
    int status = EXIT_SUCCESS;

    if (argc == 3 && strcmp(argv[1], "--input") == 0)
        input = argv[2];
    else if (argc != 1)
    {
        fprintf(stderr, "usage: %s [--input prices.csv]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (input)
    {
        prices = read_prices(input, &N);
        if (!prices || N <= LONG_WINDOW)
        {
            fprintf(stderr, "need more than %d prices, one per line, price in the last comma-separated field\n",
                    LONG_WINDOW);
            free(prices);
            return EXIT_FAILURE;
        }
        printf("%d prices read from %s\n\n", N, input);
    }
    else
    {
        N = SYNTHETIC_DAYS;
        prices = malloc(N * sizeof *prices);
        if (!prices)
            return EXIT_FAILURE;
        synthetic_prices(prices, N);
        printf("Synthetic random walk of %d days: daily volatility %.1f%% for %d days, then %.1f%%\n\n", N,
               100.0 * CALM_DAILY_SIGMA, CALM_DAYS, 100.0 * TURBULENT_DAILY_SIGMA);
    }

    returns_count = N - 1;
    returns = malloc(returns_count * sizeof *returns);
    short_volatility = malloc(returns_count * sizeof *short_volatility);
    long_volatility = malloc(returns_count * sizeof *long_volatility);
    ewma = malloc(returns_count * sizeof *ewma);
    if (!returns || !short_volatility || !long_volatility || !ewma)
    {
        status = EXIT_FAILURE;
        goto done;
    }

    log_returns(prices, N, returns);
    if (summarise_returns(returns, returns_count) ||
        check(rolling_volatility(returns, returns_count, SHORT_WINDOW, short_volatility),
              "xtract_moving_standard_deviation") ||
        check(rolling_volatility(returns, returns_count, LONG_WINDOW, long_volatility),
              "xtract_moving_standard_deviation") ||
        check(ewma_volatility(returns, returns_count, ewma), "xtract_ewma_variance"))
    {
        status = EXIT_FAILURE;
        goto done;
    }

    report(prices, short_volatility, long_volatility, ewma, returns_count);

    change = first_change(short_volatility, long_volatility, returns_count);
    if (change < 0)
        printf("No day where the 21-day volatility exceeds %.1f times the 63-day volatility\n", CHANGE_RATIO);
    else
        printf("Volatility regime change on day %d: 21-day volatility %.1f%% against 63-day %.1f%%\n",
               day_of_return(change),
               short_volatility[change - SHORT_WINDOW + 1], long_volatility[change - LONG_WINDOW + 1]);

    if (!input)
    {
        int earliest = CALM_DAYS - 1;
        int latest = CALM_DAYS - 1 + SHORT_WINDOW;
        if (change < earliest || change > latest)
        {
            fprintf(stderr, "expected the change between days %d and %d\n", day_of_return(earliest),
                    day_of_return(latest));
            status = EXIT_FAILURE;
        }
        else
            printf("The planted change on day %d was found within %d days\n", CALM_DAYS + 1, SHORT_WINDOW);
    }

done:
    free(ewma);
    free(long_volatility);
    free(short_volatility);
    free(returns);
    free(prices);
    return status;
}
