// No two single-type-import declarations clash with each other.
import a.foo;
import b.foo;
public class Main {

    public Main() {}

    public static int test() {
        return new foo();
    }

}
