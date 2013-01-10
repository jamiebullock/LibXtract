
import xtract.core.*;
import java.util.Arrays;

public class test {
    public static void main(String argv[]) {

        try {
            System.loadLibrary("jxtract");
        }
        catch (UnsatisfiedLinkError e) {
            System.out.println("Failed to load the library \"jxtract\"");
            System.out.println(e.toString());
            System.exit(0);
        }

        System.out.println("\nRunning libxtract Java bindings test...\n");

        int len          = 5;
        int retval       = 0;
        double mean[]     = new double[1];
        double variance[] = new double[1];
        double data[]     = new double[len];

        for (int i = 0; i < len; i++){
            System.out.print(i * 3 + ", ");
            data[i] = i * 3;
        }

        retval = xtract.xtract_mean(data, len, null, mean);
        retval = xtract.xtract_variance(data, len, mean, variance);

        System.out.print("The mean of: " + Arrays.toString(data) + " is "
                + mean[0] + "\nThe variance is: " + variance[0] + "\n");
    }
}
