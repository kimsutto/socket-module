import java.util.concurrent.ConcurrentHashMap;

public class WoeidManager {
		
	ConcurrentHashMap<String, Woeid> hashmap = new ConcurrentHashMap<String, Woeid>();
	
	public WoeidManager() {
		ConcurrentHashMap<String, Woeid> hashmap = new ConcurrentHashMap<String, Woeid>();
	}
	public void add(Woeid data) {
		String key = data.getCity();
		hashmap.put(key, data);	
	}
	public void find(String city) {
		Woeid i = hashmap.get(city);
		i.toString();
		return;
	}
	public void printAll() {
        for(String key : hashmap.keySet()){
            Woeid value = hashmap.get(key);
            System.out.println(value.toString());
        }
		
	}

}