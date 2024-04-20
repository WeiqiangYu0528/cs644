public class J1_ignorevalues {
public J1_ignorevalues(){}
public static int test() {
	int x = 5;
	if (x != 5) x = 10;
	while (x != 5) x = 10;
	return 123;
}
}
