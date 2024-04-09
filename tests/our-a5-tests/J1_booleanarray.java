public class J1_booleanarray{
public J1_booleanarray(){}
public static int test(){
	boolean[] a = new boolean[200];
	a[123] = true;
	int i = 0;
	for (boolean x = false; x == false; x=a[i]){
		i = i + 1;
		if (i != 123) {a[i] = false;}
	}
	return 123;
}
}
