import foo.bar;
import bar.foo;

public class Main {
    public Main() {}
    public static void main(String[] args) {
        bar a = new bar();
        a.printMessage();
    }
}