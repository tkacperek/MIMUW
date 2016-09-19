package the_voice;

import java.util.ArrayList;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.select.Elements;

class TekstyReader extends Reader {

	TekstyReader(String src) throws IllegalArgumentException {
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
		// TODO teksty.readArtists
		for (String name : names) {
			Artist artist = new Artist(name);
			artists.add(artist);
			
			String queryName = name.toLowerCase();
			queryName = queryName.replace(" ", "-");
			
			Document artistDoc;
			try {
				artistDoc =
						Jsoup.connect(source + "/" + queryName + ",teksty-piosenek").get();
			}
			catch (Exception e) {
				System.err.println("Connection problem: " + e.getMessage());
				continue;
			}

			Document tmp = null;
			Elements next = new Elements();
			next.add(artistDoc);
			try {
				// for every page
				while (next.size() != 0) {
					if (tmp == null)
						tmp = artistDoc;
					else
						tmp = Jsoup.connect(source + next.get(0).attr("href")).get();
					Elements els = tmp.select("div.artistSongs").get(0).select("a.artist");
					// for every song
					for (int i = 0; i < els.size(); ++i) {
						try {
							Document songDoc = Jsoup.connect(els.get(i).attr("href")).get();
							String title = songDoc.select("strong.crumb").get(0).text();
							String text = songDoc.select("div.originalText").get(0).toString().replaceAll("<[^>]*>", "");
							artist.addSong(new Song(title, artist, text));
						} catch (Exception e) {
							System.err.println("Couldn't read the song.");
						}
					}
					next = tmp.select("div.next").get(0).select("a");
				}
			} catch (Exception e) {
				System.err.println("Connection problem: " + e.getMessage());
				continue;
			}
		}
	}

}
