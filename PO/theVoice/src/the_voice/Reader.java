package the_voice;

import java.util.ArrayList;

abstract class Reader {
	protected ArrayList<Artist> artists;
	protected String source;
	
	Reader(String src) {
		artists = new ArrayList<Artist>();
		source = src;
	}
	
	ArrayList<Artist> getArtists() {
		return artists;
	}
	
	abstract void readArtists(ArrayList<String> names);
}
