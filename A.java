public class A{
    public A() {
    }

    public static int test() {
        int[] arr = new int[1];
        arr[0] = 1;
        if (1 < 3 && arr[0] > 5) {
            return 123;
        }
        return 200;
    }
}