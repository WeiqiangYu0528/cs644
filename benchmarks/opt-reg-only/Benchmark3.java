public class Benchmark3 {
    public Benchmark3() {}
    public static int test() {
        int[] array = new int[10000000];
        int sum = 0;
        for (int i = 0; i < array.length; i = i + 1) {
            for(int j = 0; j < 10; j = j + 1) {
                array[i] = i;
                i = array[i];
                array[i] = i;
                sum = sum + array[i];
            }

        }
        return sum;
    }
}