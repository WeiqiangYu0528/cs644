public class InterfaceMethodBody {

    InterfaceMethodBody(){}
    
    // expecting an error because of rule 11: An interface method cannot have a body.
    interface joo {
        Object a = new Object();
    }

    interface loo {}
}
