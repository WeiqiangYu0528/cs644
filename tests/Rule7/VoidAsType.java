public class VoidAsType {
    
    public VoidAsType(){}
    // expecting an error because of rule 7: `The type void may only be used as the return type of a method.`
    public void voidType;

    public void foo() {
        // expecting an error because of rule 7: `The type void may only be used as the return type of a method.`
        void a = new Object();
    }
}
