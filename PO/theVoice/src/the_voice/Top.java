package the_voice;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;

class WordCount implements Comparable<WordCount> {
	private Word word;
	private Integer count;
	
	WordCount(Word w, Integer c) {
		word = w;
		count = c;
	}
	
	@Override
	public String toString() {
		return word + "=" + count;
	}

	@Override
	public int compareTo(WordCount o) {
		Integer diff = o.count - this.count;
		if (diff == 0)
			return this.word.getContent().compareTo(o.word.getContent());
		else return diff;
	}
}

class ArtistTop {
	private Artist artist;
	private ArrayList<WordCount> top;
	
	
	ArtistTop(Artist a) {
		artist = a;
		top = new ArrayList<WordCount>(5);
	}
	
	void addEntry(WordCount wc) {
		top.add(wc);
	}
	
	@Override
	public String toString() {
		String s = new String();
		s += artist + "\n[";
		for (int i = 0; i < top.size(); i++) {
			if (i != 0) s += ", ";
			s += top.get(i);
		}
		s += "]\n";
		return s;
	}
}

class Top extends Processor {
	private ArrayList<ArtistTop> tops;

	Top(ArrayList<Artist> a, HashSet<Word> fw) {
		super(a, fw);
		tops = new ArrayList<ArtistTop>();
	}
	
	@Override
	void process() {
		for (Artist artist : artists) {
			HashMap<Word, Integer> map = new HashMap<Word, Integer>();
			for (Song song : artist.getSongs()) {
				for (Word word : song.getWords()) {
					if (map.containsKey(word))
						map.put(word, map.get(word) + 1);
					else
						map.put(word, 1);
				}
			}
			for (Word word : forbiddenWords)
				if (map.containsKey(word))
					map.remove(word);
			ArrayList<WordCount> words = new ArrayList<WordCount>();
			for (Word w : map.keySet())
				words.add(new WordCount(w, map.get(w)));

			Collections.sort(words);
			ArtistTop at = new ArtistTop(artist);
			for (int i = 0; i < Math.min(5, words.size()); i++)
				at.addEntry(words.get(i));
			tops.add(at);
		}
		processed = true;
	}

	@Override
	public String toString() {
		if (!processed) return "Not ready yet.";
		String s = new String();
		s += "top5:\n";
		for (ArtistTop ar : tops)
			s += ar;
		return s;
	}
}
