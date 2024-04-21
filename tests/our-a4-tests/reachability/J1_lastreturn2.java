public class J1_lastreturn2 {
    public J1_lastreturn2() {}
    public int test(int a) {
        int c = 0;
        if(0 < 0)
            return c;
        if (a > 0) {
            return 1;
        } else if (a < 0) {
            return -1;
        } else {
            return 0;
        }
    }
}