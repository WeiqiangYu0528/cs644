public class AbstractMethod {

    public AbstractMethod(){}
    
    // expected an error because of rule 4: `An abstract method cannot be static or final`.
    public static abstract void aoo() {
    }

    // expected an error because of rule 4: `An abstract method cannot be static or final`.
    public final abstract void boo() {
    }

    public abstract void coo() {}
}
