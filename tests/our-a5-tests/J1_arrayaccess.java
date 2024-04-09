public class J1_arrayaccess {
	public J1_arrayaccess(){}
	public static int test() {
		int[] arr = new int[123];
		for (int i = 0; i < arr.length; i = i + 1) {
			arr[i] = i+J1_arrayaccess.justone();
		}
		return arr[arr.length-1];
	}
	public static int justone() {
		return 1;
	}
}
