package the_voice;

import java.util.ArrayList;

class Song {
	private String title;
	private Artist artist;
	private ArrayList<Word> words;
	private String source;
	
	Song(String t, Artist a, String s) {
		title = new String(t);
		artist = a;
		source = new String(s);
		String[] splittedSource = source.split("[\\s,.:;?!()-]+");
		words = new ArrayList<Word>();
		for (String x : splittedSource) {
			try {
				words.add(new Word(x));
			}
			catch (Exception e) {}
		}
	}
	
	String getTitle() {
		return title;
	}
	
	Artist getArtist() {
		return artist;
	}
	
	ArrayList<Word> getWords() {
		return words;
	}
}
