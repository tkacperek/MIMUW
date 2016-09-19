package the_voice;

class Word {
	private String content;
	
	Word(String c) throws IllegalArgumentException {
		if (c.length() == 0)
			throw new IllegalArgumentException("Empty word.");
		content = new String(c);
		content = content.toLowerCase();
	}
	
	String getContent() {
		return content;
	}
	
	@Override
	public int hashCode() {
		return content.hashCode();
	}
	
	@Override
	public boolean equals(Object obj) {
		if (this == obj) return true;
		Word w = (Word)obj;
		return content.equals(w.content);
	}
	
	@Override
	public String toString() {
		return new String(content);
	}
}
