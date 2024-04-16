public class A{
    public int a = 5;
    public int b;
    public A() {
    }

    public int joy() {
        return 123;
    }

    public static int test() {
        A obj = new A();
        return obj.joy();
    }
}
