public class Local10 {
	public Local10() {}
	
	public static int test() {
		int a = 1;
        while (a == 1) {
            {
                boolean b = true;
                {
                    int a = 2;
                    int c = 3;
                }
                int c = 4;
            }
            boolean b = false;
        }
		return a;
	}
}
