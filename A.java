public class A{

    public A(){}

    public static int test(){
	
	boolean b = true;
	boolean e = false;
	Object a = new A();
	boolean c = e || a instanceof A;
	boolean d = b && a instanceof A;

	if (c && d){
	    return 123;
	}
	else {
	    return 12378;
	}
    }

}
