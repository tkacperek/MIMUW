package gramatyka;

import java.util.ArrayList;
import java.util.HashMap;

abstract class Symbol {
	protected Character symbol;
	// I know it's bad design, but it's faster than frequent usage of validChar
	protected Boolean isTerminal;
	
	protected Character getSymbol() {
		return symbol;
	}
	
	@Override
	public String toString() {
		return symbol.toString();
	}
}

class Terminal extends Symbol {
	// hm and get save memory when many Symbols with the same symbol are used 
	private static HashMap<Character, Terminal> hm = new HashMap<Character, Terminal>(5);
	
	private Terminal(Character c) throws IllegalArgumentException {
		if (! Terminal.validChar(c))
			throw new IllegalArgumentException("Invalid terminal character");
		symbol = c;
		isTerminal = true;
	}
	
	static Terminal get(Character c) throws IllegalArgumentException {
		if (c == null)
			throw new IllegalArgumentException("Null character");
		if (! hm.containsKey(c))
			hm.put(c, new Terminal(c));
		return hm.get(c);
	}
	
	static Boolean validChar(Character c) throws IllegalArgumentException {
		if (c == null)
			throw new IllegalArgumentException("Null character");
		return c.toString().matches("[a-z]+");
	}
}

class NonTerminal extends Symbol {
	// hm and get save memory when many Symbols with the same symbol are used
	private static HashMap<Character, NonTerminal> hm = new HashMap<Character, NonTerminal>();
	// validCharacters is used in getFree. 
	// storing the alphabet in a field is better than calculating it every time
	private static String validCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	
	private NonTerminal(Character c) throws IllegalArgumentException {
		if (! NonTerminal.validChar(c))
			throw new IllegalArgumentException("Invalid nonterminal character");
		symbol = c;
		isTerminal = false;
	}
	
	static NonTerminal get(Character c) throws IllegalArgumentException {
		if (c == null)
			throw new IllegalArgumentException("Null character");
		if (! hm.containsKey(c))
			hm.put(c, new NonTerminal(c));
		return hm.get(c);
	}
	
	// returns a NonTerminal that is not present in the ArrayList argument
	static NonTerminal getFree(ArrayList<NonTerminal> nt) throws IllegalStateException {
		String nonTerminals = "";
		for (NonTerminal x : nt) nonTerminals += x.toString();
		ArrayList<Character> freeCharacters = new ArrayList<Character>();
		for (Character x : validCharacters.toCharArray()) freeCharacters.add(x);
		for (Character x : nonTerminals.toCharArray()) freeCharacters.remove(x);
		
		if (freeCharacters.isEmpty())
			throw new IllegalStateException("Out of characters");
		return NonTerminal.get((Character)freeCharacters.toArray()[0]);
	}
	
	static Boolean validChar(Character c) throws IllegalArgumentException {
		if (c == null)
			throw new IllegalArgumentException("Null character");
		return c.toString().matches("[A-Z]+");
	}
}