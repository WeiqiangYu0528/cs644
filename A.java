public class A{
    public int a = 5;
    public int b;
    public A() {
    }

    public int temp() {
        return this.joy();
    }

    public int joy() {
        return a;
    }

    public static int test() {
        A obj = new A();
        return obj.temp();
    }
}
