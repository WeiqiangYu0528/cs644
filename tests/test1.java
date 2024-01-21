public final class hello {
    protected int x;
    public A() {}
    protected A() {}
    public B(int x) {}
    public int m();
    public int m1(int c) {}
    public int y;
    protected int m() {}
}

public abstract class A {
  public A() {}
  public abstract int m();
}

public final class A {
    public A() {}
    public final int m() {
    }
}

public class A {
    public static native int m(int i);
}