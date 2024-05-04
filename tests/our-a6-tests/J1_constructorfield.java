public class J1_constructorfield{
public int x;
public J1_constructorfield(){x = 123;}
public static int test(){
J1_constructorfield y = new J1_constructorfield();
return y.x;
}}
