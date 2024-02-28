import foo.*;
public class Main {
    public Main() {}
    public static int test() {
        foo.test.A a = new foo.test.A();
        foo.test.A b = new foo.test.A();
        b = a;
        return 0;
    }
}