public class J1_factorial {
	public J1_factorial(){}
	public static int test() {
		return J1_factorial.factorial(5) + J1_factorial.factorial(3) / 2;
	}
	public static int factorial(int x) {
		if (x == 1) return 1;
		else return x * J1_factorial.factorial(x-1);
	}
}
