public class J1_and {
public J1_and(){}
public static int test() {
	int x = 12;
	int y = 12;
	boolean a = true;
	boolean b = false;
	if (x == 13 && y == 12) {return -1;}
	else if (b && a) {return -2;}
	else if (a && b || b && a || b && !a || a && !b) {
		return 123;
	}
	return -1;
}
}
