public class J1_arrays {
	public J1_arrays(){}
	public static int test() {
		int[] x = new int[128];
		x[123] = 123;
		return x[x[-1+1+x[123]]];
	}
}
