public class J1_for {
	public static int z = 1;
	public J1_for(){}
	public static int test() {
		int ret = 0;
		for (int i = 0; i < 123; i=i+1) {
			ret = ret - ret;
			ret = ret + i;
		}
		return ret+J1_for.z;
	}
}
