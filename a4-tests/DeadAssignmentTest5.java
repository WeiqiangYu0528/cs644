public class DeadAssignmentTest5 {
    public DeadAssignmentTest5() {}

    public static int test() {
        int x = 3;
        int y = 5;
        int z = 6;
        {
            x = y + z;
            y = z + 123;
        }
        return z;
    }
}
