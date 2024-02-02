public class cast_test {
  public cast_test() {
    a = (int)123213;
    b = (int)!a;
    a = (Object)(int)(Object)(Integer)a;
    a = (Object)(int)(Object)(Integer[])a;
  }

}
