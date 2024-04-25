public class J1_nestedcalls{
public J1_nestedcalls(){}
public static int c2(int x){
return x;
}
public int cowabunga(int x){
return 122+J1_nestedcalls.c2(x);
}
public static int test(){
J1_nestedcalls x = new J1_nestedcalls();
return x.cowabunga(1);
}

}

