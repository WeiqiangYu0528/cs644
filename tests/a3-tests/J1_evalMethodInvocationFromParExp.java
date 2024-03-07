// PARSER_WEEDER,TYPE_CHECKING
public class J1_evalMethodInvocationFromParExp {
    
    public J1_evalMethodInvocationFromParExp() {}

    public static void test() {
	("12"+"3").toString();
    //return new Integer(("12"+"3").toString()).intValue();
    }
}
