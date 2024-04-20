public class DeadAssignmentTest3 {
    public DeadAssignmentTest3() {}

    public static int test() {
        int x = 1;
        int y = 2;
        int z = 3;
        if (x > 0 && y > 0) {
            z = 123;
        }
        return x;
    }
}
