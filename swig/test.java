import libxtract.*;

public class test {
    public static void main(String[] args) {

        try {
            System.loadLibrary("xtract_java");
        }
        catch (UnsatisfiedLinkError e) {
            System.out.println("Failed to load the library \"xtract_java\"");
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
        xtract.xtract_mean(a.cast(), len, null, meanResult);
        double mean = meanResult[0];
        System.out.printf("The mean of %s is: %.2f%n", temp, mean);

        doubleArray argv = new doubleArray(1);
        argv.setitem(0, mean);

        double[] varianceResult = new double[1];
        xtract.xtract_variance(a.cast(), len,
                xtract.doublea_to_voidp(argv.cast()), varianceResult);
        System.out.printf("The variance is %.2f%n", varianceResult[0]);

        System.out.println("Computing spectrum...");

        doubleArray specArgv = new doubleArray(4);
        specArgv.setitem(0, 44100.0 / len);
        specArgv.setitem(1, xtract_spectrum_.XTRACT_MAGNITUDE_SPECTRUM.swigValue());
        specArgv.setitem(2, 0.0);
        specArgv.setitem(3, 0.0);

        xtract.xtract_init_fft(len, xtract_features_.XTRACT_SPECTRUM.swigValue());

        doubleArray result = new doubleArray(len);
        xtract.xtract_spectrum(a.cast(), len,
                xtract.doublea_to_voidp(specArgv.cast()), result.cast());

        for (int i = 0; i < len; i++) {
            System.out.println(result.getitem(i));
        }

        System.out.println("\nFinished!\n");

        a.delete();
        argv.delete();
        specArgv.delete();
        result.delete();
    }
}
