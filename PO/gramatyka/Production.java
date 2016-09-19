package gramatyka;

import java.util.ArrayList;
import java.util.Comparator;

class ProductionComparator implements Comparator<Production> {

	@Override
	public int compare(Production p1, Production p2) {
		return Character.compare(p1.getLhs().getSymbol(), p2.getLhs().getSymbol());
	}
}

class Production {
	// left-hand side of production
	private NonTerminal lhs;
	// right-hand side of production
	private ArrayList<Symbol> rhs;
	
	Production(Character left, String right) throws IllegalArgumentException {
		this(right);
		lhs = NonTerminal.get(left);
	}
	
	Production(NonTerminal left, String right) throws IllegalArgumentException {
		this(right);
		if (left == null)
			throw new IllegalArgumentException("Null lhs");
		lhs = left;
	}
	
	private Production(String right) throws IllegalArgumentException {
		if (right == null)
			throw new IllegalArgumentException("Null string");
		if (right.isEmpty())
			rhs = new ArrayList<Symbol>(0);
		else {
			rhs = new ArrayList<Symbol>(right.length());
			for (Character c : right.toCharArray())
				rhs.add((Terminal.validChar(c)) ? Terminal.get(c) : NonTerminal.get(c));
		}		
	}
	
	Production(NonTerminal left, ArrayList<Symbol> right) {
		lhs = left;
		rhs = new ArrayList<Symbol>(right);
	}
	
	// substitutes first rhs symbol from beingExtended with rhs from helper
	Production(Production beingExtended, Production helper) throws IllegalArgumentException {
		if (beingExtended.rhs.size() == 0)
			throw new IllegalArgumentException("First production is empty");
		if (beingExtended.rhs.get(0) != helper.lhs)
			throw new IllegalArgumentException("Cannot substitute");
		lhs = beingExtended.lhs;
		rhs = new ArrayList<Symbol>(beingExtended.rhs.size() - 1 + helper.rhs.size());
		for (Symbol x : helper.rhs)
			rhs.add(x);
		for (int i = 1; i < beingExtended.rhs.size(); ++i)
			rhs.add(beingExtended.rhs.get(i));
	}
	
	NonTerminal getLhs() {
		return lhs;
	}
	
	Integer getRhsSize() {
		return rhs.size();
	}
	
	Symbol getRhs(Integer i) {
		if (i < 0 || rhs.size() <= i)
			throw new IllegalArgumentException("Index out of bounds");
		return rhs.get(i);
	}
	
	ArrayList<Symbol> getRhsCopy() {
		return new ArrayList<Symbol>(rhs); 
	}
	
	ArrayList<Symbol> getRhsTail() {
		ArrayList<Symbol> tmp = new ArrayList<Symbol>(rhs.size() - 1);
		for (int i = 1; i < rhs.size(); ++i)
			tmp.add(rhs.get(i));
		return tmp;
	}
	
	Boolean isRecursive() {
		return lhs == rhs.get(0);
	}
	
	@Override
	public String toString() {
		String s = lhs.toString() + " -> ";
		if (rhs.size() == 0)
			s += "&";
		else
			for (Symbol x : rhs)
				s += x;
		return s;
	}
}
