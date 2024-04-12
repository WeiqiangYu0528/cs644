public class J1_swaparray{
public J1_swaparray(){}
public static int test(){
	byte[] arr = new byte[1000];
	arr[123] = (byte)123;
	arr[125] = (byte)12;
	J1_swaparray.swap(arr, 123, 125);
	J1_swaparray.swap(arr, 125, 123);
	J1_swaparray.swap(arr, 125, 123);
	return (int)(byte)(int)(int)(int)(byte)(byte)arr[125];
}
public static void swap(byte[] a, int i, int j){
	byte x = a[i];
	a[i] = a[j];
	a[j] = x;
	return;
}

}
