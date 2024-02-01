public class NativeMethod {

    public NativeMethod(){}
    
    public native static void doo()

    // expected an error because of rule 6: `A native method must be static.`
    public native void eoo()
}
