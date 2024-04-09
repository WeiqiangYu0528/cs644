public class J1_staticMethod{
public static int x;
public J1_staticMethod(int _x){
	J1_staticMethod.x=_x;
}
public static int method(int x, boolean b) {
	if (b && x > 12) {return 123;}
	else {return -123;}
}
public static int test() {
	return J1_staticMethod.method(13, true);
}}
