package the_voice;

import java.util.ArrayList;
import java.util.HashSet;

abstract class Processor {	
	protected ArrayList<Artist> artists;
	protected HashSet<Word> forbiddenWords;
	protected Boolean processed;
	
	Processor(ArrayList<Artist> a, HashSet<Word> fw) {
		artists = new ArrayList<Artist>(a);
		forbiddenWords = new HashSet<Word>(fw);
		processed = false;
	}
	
	abstract void process();
	
	@Override
	abstract public String toString();
}