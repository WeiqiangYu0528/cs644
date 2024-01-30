public class ArrayCreation {
    Object[] a;
    // expecting an error because of rule 14: No multidimensional array types or multidimensional array creation expressions are allowed.
    Object[][] b;
    // expecting an error because of rule 14: No multidimensional array types or multidimensional array creation expressions are allowed.
    Object c = new String[1][1];
}
