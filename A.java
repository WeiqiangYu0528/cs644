

public class A{
    public A() {
    }
    public void test(Object o) {
      // A b = (Object) this;
      // String s = (String) o;
      char[] ch = s.chars;
      char[] ch2 = (s).chars;
      char[] ch3 = ((String) o).chars;
              // return Arrays.equals(chars, ((String)o).chars);
    }
}
