Archiwum theVoice.jar ma spakowaną bibliotekę jsoup.
Przy samodzielnej kompilacji plików z katalogu src należy dołączyć w.w. bibliotekę (jej jar też jest w src).

Rozwiązanie zawiera część dodatkową (azlyrics.com, teksty.org).

Przykładowe polecenia:

java -jar theVoice.jar --source-type=file --source=data/texts --processors=count,top5 --filters=data/wordlist.txt:data/prepositions.txt "The Beatles" "Justin Bieber"

java -jar theVoice.jar --source-type=teksty.org --source=http://www.teksty.org --processors=count,top5 --filters=data/wordlist.txt:data/prepositions.txt "The Beatles" "Justin Bieber"

java -jar theVoice.jar --source-type=azlyrics.com --source=http://www.azlyrics.com --processors=count,top5 --filters=data/wordlist.txt:data/prepositions.txt "The Beatles" "Justin Bieber"

W razie blokady azlyrics.com, można wykorzystać kopię serwisu:

java -jar theVoice.jar --source-type=azlyrics.com --source=http://students.mimuw.edu.pl/~ap360585/azlyrics --processors=count,top5 --filters=data/wordlist.txt:data/prepositions.txt "The Beatles" "Justin Bieber"
