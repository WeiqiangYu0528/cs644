public class MethodBody {

    public MethodBody(){}

    public abstract void foo() {
        // expected an error because of rule 3: `A method has a body if and only if it is neither abstract nor native.`
        Object obj = new Object();
    }

    public native void koo() {
        // expected an error because of rule 3: `A method has a body if and only if it is neither abstract nor native.`
        Object obj = new Object();
    }

    public void joo() {
        Object obj = new Object();
    }
}
