public class StaticMethod {

    StaticMethod(){}
    
    // expected an error because of rule 5: `A static method cannot be final.`
    static final void coo() {

    }

    static void doo() {}
}
