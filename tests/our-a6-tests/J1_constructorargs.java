public class J1_constructorargs{
public int y;
public J1_constructorargs(int x) { y=x;}
public static int test(){
J1_constructorargs x = new J1_constructorargs(123);
return x.y;
}}
