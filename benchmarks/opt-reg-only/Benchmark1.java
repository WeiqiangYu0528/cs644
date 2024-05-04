public class Benchmark1 {
    public Benchmark1() {}

    public static int test() {
        int x = 0; 
        int i = 0; 
        while (i < 2147483647) {
            x = x + 1;
            i = i + 1; 
        }
        return x;
    }
}