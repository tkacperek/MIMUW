package the_voice;

import java.util.ArrayList;

class Artist {
	private String name;
	private ArrayList<Song> songs;
	
	Artist(String n) {
		name = n;
		songs = new ArrayList<Song>();
	}
	
	String getName() {
		return name;
	}
	
	ArrayList<Song> getSongs() {
		return songs;
	}

	void addSong(Song s) {
		songs.add(s);
	}
	
	@Override
	public String toString() {
		return new String(name);
	}
}
