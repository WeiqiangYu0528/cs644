public class AsciiTest {
    public String validAscii = "Hello, World! 123";
    // expected an error because of rule 1: `All characters in the input program must be in the range of 7-bit ASCII (0 to 127).`
    public String invalidAscii = "Hello, World! 123 é ü";

    public AsciiTest() {}
}
