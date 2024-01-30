public class AbstractMethod {

    AbstractMethod(){}
    
    // expected an error because of rule 4: `An abstract method cannot be static or final`.
    static abstract void aoo() {
    }

    // expected an error because of rule 4: `An abstract method cannot be static or final`.
    final abstract void boo() {
    }

    abstract void coo() {}
}
