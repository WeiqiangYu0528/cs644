public class DeadAssignmentTest6 {
    public DeadAssignmentTest6() {}

    public static int test() {
        boolean val = true;
        while (val) {
            val = false;
        }
        val = true;
        return 123;
    }
}
