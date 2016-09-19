package the_voice;

import java.util.ArrayList;

public class TheVoice {
	private String[] arguments;
	private String sourceType;
	private String source;
	private String processors;
	private String filters;
	private ArrayList<String> artistNames;
	
	public TheVoice(String[] args) {
		arguments = args;
		artistNames = new ArrayList<String>();
	}
	
	void readArguments() throws IllegalArgumentException {
		for (String str : arguments) {
			if (str.startsWith("--source-type=")) {
				sourceType = str.substring(14);
			}
			else if (str.startsWith("--source=")) {
				source = str.substring(9);
			}
			else if (str.startsWith("--processors=")) {
				processors = str.substring(13);
			}
			else if (str.startsWith("--filters=")) {
				filters = str.substring(10);
			}
			else {
				artistNames.add(str);
			}
		}
		
		if (sourceType == null)
			throw new IllegalArgumentException("No source type.");
		
		if (source == null)
			throw new IllegalArgumentException("No source.");
		
		if (processors == null)
			throw new IllegalArgumentException("No processors.");
		
		if (artistNames.size() == 0)
			throw new IllegalArgumentException("No artists.");
	}
	
	void run() throws IllegalArgumentException {
		if (processors.length() == 0)
			return;
		
		Reader reader;
		if (sourceType.equals("file"))
			reader = new FileReader(source);
		else if (sourceType.equals("azlyrics.com"))
			reader = new AzReader(source);
		else if (sourceType.equals("teksty.org"))
			reader = new TekstyReader(source);
		else
			throw new IllegalArgumentException("Unsupported source type.");
		reader.readArtists(artistNames);
		
		FileReader filterReader = new FileReader();
		if (filters != null)
			filterReader.readFilter(filters);
		
		for (String str : processors.split(",")) {
			if (str.equals("count")) {
				Count count = new Count(reader.getArtists());
				count.process();
				System.out.println(count + "***");
			}
			else if (str.equals("top5")) {
				Top top = new Top(reader.getArtists(), filterReader.getFilter());
				top.process();
				System.out.println(top + "***");
			}
			else
				System.err.println("Invalid processor.");
		}
		
	}
	
	public static void main(String[] args) {
		TheVoice voice = new TheVoice(args);
		try {
			voice.readArguments();
			voice.run();
		}
		catch (Exception e) {
			System.err.println("Something's wrong: " + e.getMessage());
		}
	}
}
