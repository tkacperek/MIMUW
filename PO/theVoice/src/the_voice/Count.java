package the_voice;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;

class ArtistCount implements Comparable<ArtistCount> {
	private Artist artist;
	private Integer count;
	
	ArtistCount(Artist a, Integer c) {
		artist = a;
		count = c;
	}
	
	Artist getArtist() {
		return artist;
	}
	
	Integer getCount() {
		return count;
	}

	@Override
	public int compareTo(ArtistCount o) {
		return o.count - this.count;
	}
	
	@Override
	public String toString() {
		return artist + " " + count;
	}
}

class Count extends Processor {
	private ArrayList<ArtistCount> records;
	
	Count(ArrayList<Artist> a) {
		super(a, new HashSet<Word>());
		records = new ArrayList<ArtistCount>();
	}

	@Override
	void process() {
		for (Artist artist : artists) {
			HashSet<Word> words = new HashSet<Word>();
			for (Song song : artist.getSongs())
				for (Word word : song.getWords())
					words.add(word);
			records.add(new ArtistCount(artist, words.size()));
		}
		Collections.sort(records);
		processed = true;
	}

	@Override
	public String toString() {
		if (!processed) return "Not ready yet.";
		String s = new String();
		s += "count:\n";
		for (ArtistCount ac : records)
			s += ac + "\n";
		return s;
	}
}
