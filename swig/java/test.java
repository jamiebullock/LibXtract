
import xtract.core.*;

public class test {
    public static void main(String argv[]) {

	try {
	    System.loadLibrary("jxtract");
	}
	catch (UnsatisfiedLinkError e) {
	    System.out.println("Failed to load the library \"jxtract\"");
	    System.out.println(e.toString());
	}

	System.out.println("\nRunning libxtract Java bindings test...\n");

        int len = 5;
        int retval = 0;
        float result[];
        floatArray a = new floatArray(len);
        SWIGTYPE_p_void myvoid = null;

        result = new float[1];

	System.out.print("The mean of: ");

        for (int i = 0; i < len; i++){
	    System.out.print(i * 2 + ", ");
            a.setitem(i, i * 2);
	}

	System.out.print("is: ");

        retval = xtract.xtract_mean(a.cast(), len, myvoid, result);

        System.out.print(result[0] + "\n");

	System.out.println("\nFinished!\n\n");

    }
}
