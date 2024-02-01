public class FieldNoFinal {
    // expecting an error because of rule 13: `No field can be final.`
    public final Object a;

    public Object b;

    public FieldNoFinal(){}

}