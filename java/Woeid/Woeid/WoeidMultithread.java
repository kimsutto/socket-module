import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

public class WoeidMultithread extends Thread{
	
	List<Woeid> list;
	WoeidManager mm;
	String filename;
	
	public WoeidMultithread(String filename,WoeidManager wm) {
		list = WoeidImporter.loadCSV(filename);
		this.filename = filename;
		mm = wm;
	}
	
	@Override
	public void run() { // Runnable run() implement
		int i = 0; // local counter
		while (i<list.size()) {
			mm.add(list.get(i));
			System.out.println(filename + " add " + i + " complete");
			i++;		
			try {
				Thread.sleep(500);
			} catch (InterruptedException e) {
				return;
			}
		}
	}
}
	
	


