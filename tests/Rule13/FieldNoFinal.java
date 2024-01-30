class FieldNoFinal {
    // expecting an error because of rule 13: `No field can be final.`
    final Object a;

    Object b;

    FieldNoFinal(){}

}