public class Benchmark2 {
    public Benchmark2() {}
    public static int test() {
        int sum = 0;
        for (int i = 0; i < 2147483647; i = i + 1) {
            if (i % 2 == 0) {
                sum = sum + i;
            } else {
                sum = sum - i;
            }
        }
        return sum;
    }
}