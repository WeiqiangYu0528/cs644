
public class ThisSuperCall {
    ThisSuperCall(){
        // expecting an error because of rule 15: `A method or constructor must not contain explicit this() or super() calls.`
        this();
        // expecting an error because of rule 15: `A method or constructor must not contain explicit this() or super() calls.`
        super();
    }

    void foo() {
        // expecting an error because of rule 15: `A method or constructor must not contain explicit this() or super() calls.`
        this();
        // expecting an error because of rule 15: `A method or constructor must not contain explicit this() or super() calls.`
        super();
    }
}
