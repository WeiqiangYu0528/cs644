public class MethodBody {

    MethodBody(){}

    abstract void foo() {
        // expected an error because of rule 3: `A method has a body if and only if it is neither abstract nor native.`
        Object obj = new Object();
    }

    native void koo() {
        // expected an error because of rule 3: `A method has a body if and only if it is neither abstract nor native.`
        Object obj = new Object();
    }

    void joo() {
        Object obj = new Object();
    }
}
