
public class Utility {

    public static void paddingBytesArray(byte[] dest, int newSize, byte[] src, int size) {
	    for (int i = 0; i < size; i++) {
		    dest[i] = src[i];
	    }
	    for (int i = size; i < newSize; i++) { // fill null character
		    dest[i] = 0;
	    }
    }
    
    /*public static String getReceivedString(byte[] src) {
 		int index;
 	    char[] tempCharData = new char[src.length];
 		for (index = 0; index < src.length; index++) {
 			if (src[index] != 0)
 				tempCharData[index] = (char) src[index];
 			else 
 				break;  // null character is hit			
 		}
 		String s = new String(tempCharData, 0, index);
 		return s;
 	}*/
    
    public static String getReceivedString(byte[] src) {
    	int index = 0;
 		while (index < src.length) {
 			if (src[index] == 0) break; // null character is hit
 			index++;
 		}
 		String s = new String(src, 0, index);
 		return s;
 	}
}
