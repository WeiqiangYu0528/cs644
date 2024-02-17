public class Local8 {
	public Local8() {}
	
	public static int test() {
		int a = 1;
        {
            {
                {
                    int a = 2;
                }
            }
        }
		return a;
	}
}
