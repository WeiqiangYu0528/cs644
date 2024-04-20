public class DeadAssignmentTest7 {
    public DeadAssignmentTest7() {}

    public static int test() {
        int a = 1;
        int b = 2;
        int c = 3;
        if (a == 1) {
            b = 123;
        }
        if (b == 2) {
            c = 123;
        }
        a = b;
        return a; 
    }

}
