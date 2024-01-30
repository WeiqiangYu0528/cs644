public class NativeMethod {

    NativeMethod(){}
    
    native static void doo() {

    }

    // expected an error because of rule 6: `A native method must be static.`
    native void eoo() {

    }
}
