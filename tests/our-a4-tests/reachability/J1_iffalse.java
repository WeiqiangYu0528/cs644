public class J1_iffalse {
public J1_iffalse(){}
public static int test(){
int x;
if(true){
x = 3;
} else {
x = 4; //this is unreachable, but according to 14.20 "The else-statement is reachable iff the if-then-else statement is reaachable"
}
return x;
}}
