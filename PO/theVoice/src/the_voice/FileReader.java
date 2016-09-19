package the_voice;

import java.io.File;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.HashSet;

class FileReader extends Reader {
	private File file;
	private HashSet<Word> filter;
	
	FileReader() {
		super("");
		filter = new HashSet<Word>();
	}
	
	FileReader(String src) throws IllegalArgumentException {
		super(src);
		file = new File(src);
			if (!file.exists() || !file.isDirectory())
				throw new IllegalArgumentException("Wrong source path.");
	}
	
	@Override
	void readArtists(ArrayList<String> names) {
		for (String name : names) {
			Artist artist = new Artist(name);
			artists.add(artist);
			
			File artistFile = new File(file.getPath() + "/" + name);
			if (artistFile.exists() && artistFile.isDirectory())
				for (File songFile : artistFile.listFiles())
					try {
						if (!songFile.isDirectory() && songFile.getName().endsWith(".txt")) {
							byte bytes[] = Files.readAllBytes(songFile.toPath());
							String songText = new String(bytes, StandardCharsets.UTF_8);
							artist.addSong(new Song(songFile.getName(), artist, songText));
						}
					}
					catch (Exception e) {
						System.err.println("Couldn't read the song file: " + songFile.getName());
					}	
		}
	}
	
	HashSet<Word> getFilter() {
		return filter;
	}
	
	void readFilter(String srcs) {
		for (String str : srcs.split(File.pathSeparator)) {
			File file = new File(str);
			if (!file.exists() || file.isDirectory()) {
				System.err.println("Wrong filter path.");
				continue;
			}
			try {
				for (String s : Files.readAllLines(file.toPath(), StandardCharsets.UTF_8))
					filter.add(new Word(s));
			}
			catch (Exception e) {
				System.err.println("Couldn't read filters from: " + str);
			}
		}
	}
}
