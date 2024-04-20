public class DeadAssignmentTest2 {
    public DeadAssignmentTest2() {}

    public static int test() {
        int a = 0;
        if (a > 0) {
            a = 2;
        } else {
            a = 3;
            return a;
        }
        return 123;
    }

}
