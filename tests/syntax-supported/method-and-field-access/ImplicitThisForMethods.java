public class ImplicitThisForMethods {
    public ImplicitThisForMethods() {}
    public int m1() {
      return 42;
    }
    public int m2() {
      return m1();
    }
  }