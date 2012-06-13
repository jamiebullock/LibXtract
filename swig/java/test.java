
import xtract.core.*;

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

        int len = 5;
        int retval = 0;
        float mean[] = new float[1];
        float variance[] = new float[1];
        float[] a = new float[len];
        floatArray av = new floatArray(1);
        variance = new float[1];

	System.out.print("The mean of: ");

        for (int i = 0; i < len; i++){
	    System.out.print(i * 3 + ", ");
            a[i] = i * 3;
	}

        retval = xtract.xtract_mean(a, len, av.cast().getVoidPointer(), mean);

        System.out.print("is " + mean[0] + "\n");

        av.setitem(0, mean[0]);
        retval = xtract.xtract_variance(a, len, av.cast().getVoidPointer(), variance);
        System.out.print("The variance is: " + variance[0] + "\n");

    }
}
