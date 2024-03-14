public class A{
    public char[] chars;
    public int length() {
        return chars.length;
    }
    public A() {
    }    
    public String concat(String s2) {
        int i = 0;
        char[] newchars = new char[length() + s2.length()];
        for(i = 0; i < length(); i = i + 1)
            newchars[i] = chars[i];
        for(i = 0; i < s2.length(); i = i + 1)
            newchars[i+length()] = s2.chars[i];
        return new String(newchars);
    }
}
