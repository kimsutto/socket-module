import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class WoeidImporter {
	
 	   public static List<Woeid> loadCSV(String filename) { 
    	   List<Woeid> list = Collections.synchronizedList(new ArrayList<Woeid>()); 
       	 String line = "";
        try (BufferedReader br = new BufferedReader(new FileReader(filename))) { 
            System.out.println("file import: " + filename);
            String delimiter = ",";

            while ((line = br.readLine()) != null) {
            	if (line.contains("#")) {
            		System.out.println("line contains #: " + line);
            		continue;
            	}
                // use comma as separator
                String[] items = line.split(delimiter); 
                Woeid d = Woeid.getData(items); 
               	list.add(d);
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
		System.out.println("load successfully");
		return list;
    }
  
    public static void saveCSV(String path, List<Woeid> list) {
    	FileWriter fw = null;
		try {
			fw = new FileWriter(path);
	        for (Woeid d : list) { 
	             fw.append(d.toString() + "\n"); 
	        }
	        fw.flush(); 
	        fw.close(); 
		} catch (IOException e) {
			e.printStackTrace();
		}
    }
}
