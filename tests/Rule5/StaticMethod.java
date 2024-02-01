public class StaticMethod {

    public StaticMethod(){}
    
    // expected an error because of rule 5: `A static method cannot be final.`
    public static final void coo() {

    }

    public static void doo() {}
}
