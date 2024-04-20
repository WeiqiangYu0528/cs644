public class DeadAssignmentTest8 {
    public DeadAssignmentTest8() {}

    public static int test() {
        int x = 1;
        int y = 2;
        int z;
        if (x > 0 && y > 0) {
            z = 123;
            if (z > 0) {
                x = 123;
                z = 0;
            } else {
                y = 123;
                z = 1;
            }
        }
        return z;
    }
}
