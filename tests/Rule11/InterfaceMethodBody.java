public class InterfaceMethodBody {

    public InterfaceMethodBody(){}
    
    // expecting an error because of rule 11: An interface method cannot have a body.
    public interface joo {
        Object a = new Object();
    }

    public interface loo {}
}
