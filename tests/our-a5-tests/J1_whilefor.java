public class J1_whilefor {
	public J1_whilefor(){}
	public static boolean exiting(int counter) {
                return !(counter < 120);
	}
	public static int test() {
		boolean x = true;
		int counter = 0;
		while (x!=!x&&J1_whilefor.exiting(counter)){
			x=!x;
			counter = counter + 1;
			int z = 0;
			for (int i = counter; z < i; z = z + 1) {
				counter = counter + 1;
			}
		}
		return 123;
	}
}
