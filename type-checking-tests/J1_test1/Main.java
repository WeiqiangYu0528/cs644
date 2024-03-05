import foo.*;
import foo.test.A;
public class Main {
    public Main() {}
    public static int v() { return 1; }
    public static int test() {
        int q = v();
        int a;
        int b;
        a = b;
        short c;
        a = c;
        c = a;  // narrowing conversion
        char d;
        c = c;
        c = d;
        d = a; // narrowing conversion      
        
        A aa;
        A bb;
        aa = bb;
        aa.a = 1;
        aa.bar = aa; // assignment between different object types
        aa.bar = 1; // assignment between object and basic type
        aa.bar = bb.bar;

        boolean bool;
        bool = true;
        bool = 1;   // assignment between int/char/short and boolean
        bool = a;   // assignment between int/char/short and boolean
        a = bool;   // assignment between int/char/short and boolean

        A[] aaa;
        A[] bbb;
        char[] ccc;
        aaa = bbb;
        aaa = ccc;  // assignment between different array types
        aaa = a;    // assignment between array and basic type
        aaa = aa;   // assignment between array and object type
        String aaaa;
        String bbbb;
        aaaa = bbbb;
        aaaa = "123";
        aaaa = 123;   // assignment between string and non-string type
        aaaa.chars = ccc;
        return 0;
    }
}