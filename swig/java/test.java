
import xtract.core.*;

public class test {
    public static void main(String argv[]) {
//        System.loadLibrary("xtract");

        int len = 5;
        int retval = 0;
        float result[];
        floatArray a = new floatArray(len);
        SWIGTYPE_p_void myvoid = null;

        result = new float[1];

        for (int i = 0; i < len; i++)
            a.setitem(i, i * 2);

        retval = xtract.xtract_mean(a.cast(), len, myvoid, result);

        System.out.println(result);
    }
}
