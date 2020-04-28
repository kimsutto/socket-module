import java.util.InputMismatchException; 
import java.util.UnknownFormatConversionException; 

public class Woeid implements Comparable<Woeid> { 
	private long id; 
	private String city;
	private String country;
	private double latitude;
	private double longitude;		

	public Woeid() { 
		this(-1, "", "", 0.0, 0.0);
	}

	public Woeid(long id, String city, String country, double latitude, double longitude) {
		this.id = id;
		this.city = city;
		this.country = country;
		this.latitude = latitude;
		this.longitude = longitude;
	}

    
	public Woeid(Woeid other) { 
		this(other.id, other.city, other.country, other.latitude, other.longitude);
	}
	

	public long getID() {
		return this.id;
	}

	public void setID(long id) {
		this.id = id;
	}
	
	public String getCity() {
		return this.city;
	}

	public void setCity(String city) {
		this.city = city;
	}

	public String getCountry() {
		return this.country;
	}

	public void setCountry(String country) {
		this.country = country;
	}
	
	public double getLatitude() {
		return this.latitude;
	}

	public void setLatitude(double latitude) {
		this.latitude = latitude;
	}
	
	public double getLongitude() {
		return this.longitude;
	}

	public void setLongitude(double longitude) {
		this.longitude = longitude;
	}

	@Override
	public String toString() {
		return String.format("%d,%s,%s,%f,%f", this.id, this.city, this.country, this.latitude, this.longitude);
	}
	
	
	@Override
	public int compareTo(Woeid other) { 
    	return (int) (this.id - other.id);
	}
	    
  
	public static Woeid getData(String[] items) {
		if (items.length != 5) 
			return null;
		try {
			//System.out.println("items : " + Arrays.toString(items));
			long id = Long.parseLong(items[0]);
			String city = items[1];
			String country = items[2];
			double latitude = Double.parseDouble(items[3]);
			double longitude = Double.parseDouble(items[4]);
			return new Woeid(id, city, country, latitude, longitude);
		} catch (InputMismatchException e) {
			System.out.println("InputMismatchException!");
		} catch (UnknownFormatConversionException e) {
			System.out.println("UnknownFormatConversionException!");
		}
		return null;
	}
    // csv String -> Woeid, line
	public static Woeid getData(String line) {
		String[] items = line.split(",",5); 
		return Woeid.getData(items); // String -> Woeid
	}
}