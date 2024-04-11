public class A{
    public A() {
    }

    public static int m0(int a) {
        int r = 0;
        if (a==0) r=r+1;
        if (a!=0) r=r+2;
        if (!(a==0)) r=r+4;
        if (!(a!=0)) r=r+8;
        return r;
    }

    public static int m1(int a) {
        int r = 0;
        if (a==1) r=r+1;
        if (a!=1) r=r+2;
        if (!(a==1)) r=r+4;
        if (!(a!=1)) r=r+8;
        return r;
    }

    public static int test() {
        return A.m0(0);
        // int r2 = A.m0(1);
        // int r3 = A.m0(100);
        // int r4 = A.m1(0);
        // int r5 = A.m1(1);
        // int r6 = A.m1(100);

        // int r = 0;
	    // if (r1==9) r=r+1;
        // return r1;
    }
}
