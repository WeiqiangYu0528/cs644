public class DeadAssignmentTest4 {
    public DeadAssignmentTest4() {}

    public static int test() {
        int x = 0;
        while (x < 10) {
            int y = 3;
            y = y + 1;
            x = x + 1;
            if (x == 5) {
                return x;
            }
        }
        return x;
    }
}
