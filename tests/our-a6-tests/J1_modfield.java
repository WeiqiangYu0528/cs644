public class J1_modfield{
public J1_modfield(){}
protected static int x;
public static int test(){
	return new J1_modfield().temp(3);
}
public int temp(int y){
J1_modfield.x = y;
return 123;
}
}
