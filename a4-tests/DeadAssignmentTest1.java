public class DeadAssignmentTest1 {
    public DeadAssignmentTest1() {}

    public static int test() {
        String str1 = "Hello";
        String str2 = "World";
        String str = "";
        str = str1 + str2;
        return 123;
    }
}
