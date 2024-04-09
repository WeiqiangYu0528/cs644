public class A{
    public static int myStaticMember = 0;
    public A() {
    }
    public static int test() {
        int a = 0;
        return A.add(2, 6);
    }

    public static int add(int a, int b) {
        return a + b;
    }
}
