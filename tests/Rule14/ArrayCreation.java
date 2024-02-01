public class ArrayCreation {
    public Object[] a;
    // expecting an error because of rule 14: No multidimensional array types or multidimensional array creation expressions are allowed.
    public Object[][] b;
    // expecting an error because of rule 14: No multidimensional array types or multidimensional array creation expressions are allowed.
    public Object c = new String[1][1];
}
