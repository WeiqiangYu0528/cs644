public class InterfaceMethodModifier {

    public InterfaceMethodModifier(){}
    
    // expecting an error because of rule 10: An interface method cannot be static, final, or native
    public native interface void foo() {}

    // expecting an error because of rule 10: An interface method cannot be static, final, or native
    public final interface void boo() {}

    // expecting an error because of rule 10: An interface method cannot be static, final, or native
    public static interface void koo() {}

    public interface aoo() {}
}
