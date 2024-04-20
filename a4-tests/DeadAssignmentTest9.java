public class DeadAssignmentTest9 {
    public DeadAssignmentTest9() {}

    public static int test() {
        int x = 0;
        int y = 0;
        if (x == 0) {
            for (y = 0; y <= 10; y = y + 1) {
                x = y;
            }
        } else {
            y = 123;
        }
        return y;
    }
}
