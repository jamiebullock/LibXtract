/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

import xtract.*;

public class test {
    public static void main(String[] args) {

        try {
            System.loadLibrary("jxtract");
        }
        catch (UnsatisfiedLinkError e) {
            System.out.println("Failed to load the library \"jxtract\"");
            System.out.println(e.toString());
            System.exit(1);
        }

        System.out.println("\nRunning libxtract Java bindings test...\n");

        int len = 8;

        doubleArray a = new doubleArray(len);
        StringBuilder temp = new StringBuilder();

        for (int i = 0; i < len; i++) {
            a.setitem(i, 2.0 * i);
            if (i > 0) temp.append(", ");
            temp.append(2.0 * i);
        }

        double[] meanResult = new double[1];
        xtract.xtract_mean(null, a.cast(), len, null, meanResult);
        double mean = meanResult[0];
        System.out.printf("The mean of %s is: %.2f%n", temp, mean);

        doubleArray argv = new doubleArray(1);
        argv.setitem(0, mean);

        double[] varianceResult = new double[1];
        xtract.xtract_variance(null, a.cast(), len,
                xtract.doublea_to_voidp(argv.cast()), varianceResult);
        System.out.printf("The variance is %.2f%n", varianceResult[0]);

        System.out.println("Computing spectrum...");

        doubleArray specArgv = new doubleArray(4);
        specArgv.setitem(0, 44100.0 / len);
        specArgv.setitem(1, xtract_spectrum_.XTRACT_MAGNITUDE_SPECTRUM.swigValue());
        specArgv.setitem(2, 0.0);
        specArgv.setitem(3, 0.0);

        SWIGTYPE_p_xtract_context_ ctx = xtract.xtract_context_new(len);

        doubleArray result = new doubleArray(len);
        xtract.xtract_spectrum(ctx, a.cast(), len,
                xtract.doublea_to_voidp(specArgv.cast()), result.cast());
        xtract.xtract_context_delete(ctx);

        for (int i = 0; i < len; i++) {
            System.out.println(result.getitem(i));
        }

        System.out.println("Computing the log energy a filter passes...");

        double[] gainValues = {0.5, 0.25, 2.0, 1.0};
        double[] coefficientValues = {1.0, 2.0, 3.0, 4.0};
        doubleArray gains = new doubleArray(4);
        doubleArray coefficients = new doubleArray(4);
        for (int i = 0; i < 4; i++) {
            gains.setitem(i, gainValues[i]);
            coefficients.setitem(i, coefficientValues[i]);
        }

        SWIGTYPE_p_xtract_filterbank_ filterbank = xtract.xtract_init_filterbank(8, 1, gains.cast());
        doubleArray energy = new doubleArray(1);
        xtract.xtract_mel_spectrogram(null, coefficients.cast(), 4,
                xtract.filterbank_to_voidp(filterbank), energy.cast());
        xtract.xtract_free_filterbank(filterbank);

        System.out.printf("The log energy is %.4f%n", energy.getitem(0));

        if (!(Math.abs(energy.getitem(0) - Math.log(11.0)) <= 1e-12)) {
            System.exit(1);
        }

        gains.delete();
        coefficients.delete();
        energy.delete();

        System.out.println("Testing stateful functions...");

        int N = 4;
        SWIGTYPE_p_xtract_last_n_state_ state = xtract.xtract_last_n_state_new(N);
        doubleArray lastN = new doubleArray(N);
        for (int i = 0; i < N; i++) lastN.setitem(i, 0.0);

        doubleArray data = new doubleArray(1);
        data.setitem(0, 42.0);
        /* N.B. data is a single sample; N is the window size, not the size of data */
        xtract.xtract_last_n(null, data.cast(), N, xtract.last_n_state_to_voidp(state), lastN.cast());
        System.out.printf("last_n after one value: %.1f%n", lastN.getitem(N - 1));

        data.setitem(0, 99.0);
        xtract.xtract_last_n(null, data.cast(), N, xtract.last_n_state_to_voidp(state), lastN.cast());
        System.out.printf("last_n after two values: %.1f, %.1f%n",
                lastN.getitem(N - 2), lastN.getitem(N - 1));

        xtract.xtract_last_n_state_delete(state);
        data.delete();
        lastN.delete();

        System.out.println("\nFinished!\n");

        a.delete();
        argv.delete();
        specArgv.delete();
        result.delete();
    }
}
