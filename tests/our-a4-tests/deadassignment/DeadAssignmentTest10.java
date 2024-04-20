public class DeadAssignmentTest10 {
    public DeadAssignmentTest10() {}

    public static int test() {
        boolean a = true;
        boolean b = a && false;
        boolean c = a || b;
        boolean d = a && c;
        boolean e = c || d;
        boolean f = d & e;
        boolean g = e | f;
        if (a && b && c && d && e && f && g) {
            g = true;
        } else {
            g = false;
        }
        return 123;
    }
}
