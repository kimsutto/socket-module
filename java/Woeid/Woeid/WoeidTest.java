import java.util.concurrent.ConcurrentHashMap;

public class WoeidTest {

	public static void main(String[] args) {
		System.out.println("Woeid start");
		
			WoeidManager wm = new WoeidManager();
			
			WoeidMultithread thread1 = new WoeidMultithread("WOEIDLIST1.csv", wm);
			WoeidMultithread thread2 = new WoeidMultithread("WOEIDLIST2.csv", wm);
			WoeidMultithread thread3 = new WoeidMultithread("WOEIDLIST3.csv", wm);
		
			thread1.start();
			thread2.start();
			thread3.start();

			try {
				thread1.join();
				thread2.join();
				thread3.join();
			}catch (InterruptedException e) {
                e.printStackTrace();
            }
			wm.printAll();
	}

}
