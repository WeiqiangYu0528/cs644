public class InterfaceMethodModifier {

    InterfaceMethodModifier(){}
    
    // expecting an error because of rule 10: An interface method cannot be static, final, or native
    native interface void foo() {}

    // expecting an error because of rule 10: An interface method cannot be static, final, or native
    final interface void boo() {}

    // expecting an error because of rule 10: An interface method cannot be static, final, or native
    static interface void koo() {}

    interface aoo() {}
}
