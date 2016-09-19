package the_voice;

import java.util.ArrayList;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;

class AzReader extends Reader {

	AzReader(String src) throws IllegalArgumentException {
		super(src);
		try {
			Jsoup.connect(source).get();
		}
		catch (Exception e)
		{
			throw new IllegalArgumentException("Connection problem: " + e.getMessage());
		}
	}
	
	@Override
	void readArtists(ArrayList<String> names) {
		for (String name : names) {
			Artist artist = new Artist(name);
			artists.add(artist);
			
			String queryName = name.toLowerCase();
			if (queryName.startsWith("the"))
				queryName = queryName.substring(4);
			queryName = queryName.replace(" ", "");
			
			Document artistDoc;
			try {
				artistDoc =
						Jsoup.connect(source + "/" + queryName.charAt(0) + "/" + queryName + ".html").get();
			}
			catch (Exception e) {
				System.err.println("Connection problem: " + e.getMessage());
				continue;
			}
			
			for (Element songLink : artistDoc.select("a[href^=\"../lyrics\"")) {
				try {
					Document songDoc = Jsoup.connect(source + songLink.attr("href").substring(2)).get();
					Element tmp = songDoc.select("div.main-page").get(0).select("div.row").get(0).select("div").get(3);
					String title = tmp.select("b").get(1).text();
					title = title.substring(1, title.length() - 1);
					String text = tmp.select("div").get(8).toString().replaceAll("<[^>]*>", "");
					artist.addSong(new Song(title, artist, text));
				}
				catch (Exception e) {
					System.err.println("Couldn't read the song.");
				}
			}
		}
	}

}
