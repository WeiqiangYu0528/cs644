public interface InterfaceMethodModifier {
    
    // expecting an error because of rule 10: An interface method cannot be static, final, or native
    public native void foo()

    // expecting an error because of rule 10: An interface method cannot be static, final, or native
    public final void boo()

    // expecting an error because of rule 10: An interface method cannot be static, final, or native
    public static void koo()

    public aoo()
}
