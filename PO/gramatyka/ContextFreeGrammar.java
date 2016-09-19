package gramatyka;

import java.util.ArrayList;
import java.util.HashMap;

enum Type {
	CONTEXTFREE ("bezkontekstowa"),
	REGULAR ("regularna");
	private String str;
	
	private Type(String s) {
		str = s;
	}
	
	@Override
	public String toString() {
		return str;
	}
}

enum Form {
	NONE (""),
	CHOMSKY ("Chomsky"),
	GREIBACH ("Greibach");
	private String str;
	
	private Form(String s) {
		str = s;
	}
	
	@Override
	public String toString() {
		return str;
	}
}

public class ContextFreeGrammar {
	protected Type type;
	protected Form form;
	protected ArrayList<Terminal> terminals;
	protected ArrayList<NonTerminal> nonTerminals;
	protected ArrayList<Production> productions;
	protected NonTerminal startSymbol;
	// order of symbols; used in toGreibach
	protected HashMap<Symbol, Integer> order;
	
	// terms - terminals, nonTerms - nonTerminals, prods - productions
	public ContextFreeGrammar(String terms, String nonTerms, String[][] prods) throws IllegalArgumentException {
		type = Type.CONTEXTFREE;
		form = Form.NONE;
		
		// processing terminals
		if (terms == null)
			throw new IllegalArgumentException("Null terminals set");
		if (terms.isEmpty())
			throw new IllegalArgumentException("Empty terminals set");
		terminals = new ArrayList<Terminal>(terms.length());
		for (Character x : terms.toCharArray())
			terminals.add(Terminal.get(x));
		
		// processing nonterminals
		if (nonTerms == null)
			throw new IllegalArgumentException("Null nonterminals set");
		if (nonTerms.isEmpty())
			throw new IllegalArgumentException("Empty nonterminals set");
		nonTerminals = new ArrayList<NonTerminal>(nonTerms.length());
		for (Character x : nonTerms.toCharArray())
			nonTerminals.add(NonTerminal.get(x));
		startSymbol = nonTerminals.get(0);
		
		// processing productions
		if (prods == null)
			throw new IllegalArgumentException("Null productions set");
		if (prods.length == 0)
			throw new IllegalArgumentException("Empty productions set");
		if (nonTerms.length() != prods.length)
			throw new IllegalArgumentException("Number of nonterminals and productions differs");
		productions = new ArrayList<Production>();
		for (int i = 0; i < prods.length; i++) {
			if (prods[i] == null)
				throw new IllegalArgumentException("Null productions set for "+ i + "th nonterminal");
			if (prods[i].length == 0)
				throw new IllegalArgumentException("Empty productions set for "+ i + "th nonterminal");
			for (String x : prods[i])
				productions.add(new Production(nonTerminals.get(i), x));
		}
	}
	
	public ContextFreeGrammar(ContextFreeGrammar g) {
		type = g.type;
		form = g.form;
		terminals = new ArrayList<Terminal>(g.terminals);
		nonTerminals = new ArrayList<NonTerminal>(g.nonTerminals);
		productions = new ArrayList<Production>(g.productions);
		startSymbol = g.startSymbol;
	}
	
	@Override
	public String toString() {
		String s = "Gramatyka: " + type + "/" + form + "\n";
		s += "Terminale: ";
		for (Terminal x : terminals)
			s += x;
		s += "\nNieterminale: ";
		for (NonTerminal x : nonTerminals)
			s += x;
		s += "\nProdukcje:";
		for (Production x : productions)
			s += "\n" + x;
		return s;
	}
	
	public Boolean ifRegular() {
		// t,nt determine if the grammar is left- or right- regular 
		Integer t = null, nt = null;
		// looking for irregular productions
		try {
			for (Production x : productions) {
				if (x.getRhsSize() > 2)
					// too long
					throw new Exception();
				// if size = 0 then it's fine
				else if (x.getRhsSize() == 1) {
					if (! x.getRhs(0).isTerminal)
						// only terminals in 1-symbol productions
						throw new Exception();
				}
				else if (x.getRhsSize() == 2) {
					// handling left- / right- regularity
					if (t == null) {
						if (x.getRhs(0).isTerminal) {
							t = 0;
							nt = 1;
						}
						else {
							t = 1;
							nt = 0;
						}
					}
					if (! x.getRhs(t).isTerminal 
						|| x.getRhs(nt).isTerminal)
						// 1 terminal and 1 nonterminal required
						throw new Exception();
				}
			}
		}
		catch (Exception e) {
			// irregular production found
			return false;
		}
		// everything is ok
		type = Type.REGULAR;
		return true;
	}
	
	public Boolean ifChomsky() {
		// looking for irregular productions
		try {
			for (Production x : productions) {
				if (0 == x.getRhsSize() || 2 < x.getRhsSize())
					// wrong length
					throw new Exception();
				else if (x.getRhsSize() == 1) {
					if (! x.getRhs(0).isTerminal)
						// only terminals in 1-symbol productions
						throw new Exception();
				}
				else {
					if (x.getRhs(0).isTerminal
						|| x.getRhs(1).isTerminal)
						// 2 nonterminals required
						throw new Exception();
				}
			}
		} catch (Exception e) {
			// irregular production found
			return false;
		}
		// everything is ok
		form = Form.CHOMSKY;
		return true;
	}
	
	public Boolean ifGreibach() {
		int i;
		// looking for irregular productions
		try {
			for (Production x : productions) {
				if (x.getRhsSize() == 0)
					// no empty productions allowed
					throw new Exception();
				if (! x.getRhs(0).isTerminal)
					// first symbol has to be terminal
					throw new Exception();
				if (x.getRhsSize() > 1)
					for (i = 1; i < x.getRhsSize(); ++i)
						if (x.getRhs(i).isTerminal)
							// every other symbol has to be nonterminal
							throw new Exception();							
			}
		} catch (Exception e) {
			// irregular production found
			return false;
		}
		// everything is ok
		form = Form.GREIBACH;
		return true;
	}
	
	// sets order of all symbols in grammar
	// nonterminals < terminals
	private void setOrder() {
		Integer i, n;
		if (this.order == null) {
			n = nonTerminals.size();
			this.order = new HashMap<Symbol, Integer>();
			for (i = 0; i < n; ++i) order.put(nonTerminals.get(i), i + n);
			i = 2 * n;
			for (Symbol x : terminals) {
				order.put(x, i);
				++i;
			}
		}
	}
	
	// returns exactly what the method name says :-)
	// groups are addressed with integer values given by the symbol order
	private HashMap<Integer, ArrayList<Production>> getProductionsGroupedByLhs() {
		Integer i;
		ArrayList<Production> productionList;
		HashMap<Integer, ArrayList<Production>>  groupedProductions = new HashMap<Integer, ArrayList<Production>>();
		this.setOrder();
		i = 0;
		for (Symbol x : nonTerminals) {
			productionList = new ArrayList<Production>();
			try {
				while (i < productions.size())
					if (productions.get(i).getLhs() == x) {
						productionList.add(productions.get(i));
						++i;
					}
					else throw new Exception();
			} catch (Exception e) {};
			groupedProductions.put(order.get(x), productionList);
		}
		return groupedProductions;
	}
	
	// return Greibach form representation of the grammar, provided that it's in Chomsky form
	public NormalGreibachGrammar toGreibach() throws IllegalArgumentException {
		if (! ifChomsky())
			throw new IllegalArgumentException("This grammar is not in Chomsky form");
		
		ContextFreeGrammar newGrammar = new ContextFreeGrammar(this);
		ArrayList<NonTerminal> newNonTerminals = new ArrayList<NonTerminal>(nonTerminals);
		ArrayList<Production> newProductions;
		
		NonTerminal newNonTerminal;
		Boolean stepCompleted;
		ArrayList<Production> currentProductions, tmpProductions, recursiveProductions, nonRecursiveProductions;
		ArrayList<Symbol> tmpSymbols;
		Integer [] keyArray;
		Integer i;
		
		this.setOrder();
		HashMap<Integer, ArrayList<Production>>  groupedProductions = this.getProductionsGroupedByLhs();
		
		// for every group of productions
		for (Integer x : groupedProductions.keySet()) {
			// make sure that every group does not depend on the previous ones
			currentProductions = groupedProductions.get(x);
			stepCompleted = false;
			while (! stepCompleted) {
				stepCompleted = true;
				tmpProductions = new ArrayList<Production>();
				for (Production y : currentProductions) {
					if (order.get(y.getLhs()) > order.get(y.getRhs(0))) {
						stepCompleted = false;
						for (Production z : groupedProductions.get(order.get(y.getRhs(0))))
							tmpProductions.add(new Production(y, z));
					}
					else
						tmpProductions.add(y);
				}
				currentProductions = tmpProductions;
			}
			groupedProductions.put(x, currentProductions);
			
			// get rid of recursive productions
			
			recursiveProductions = new ArrayList<Production>();
			nonRecursiveProductions = new ArrayList<Production>();
			for (Production y : groupedProductions.get(x))
				if (order.get(y.getLhs()) < order.get(y.getRhs(0)))
					nonRecursiveProductions.add(y);
				else 
					recursiveProductions.add(y);
			
			if (recursiveProductions.size() != 0) {
				newNonTerminal = NonTerminal.getFree(nonTerminals);
				newNonTerminals.add(newNonTerminal);
				order.put(newNonTerminal, 2 * nonTerminals.size() - 1 - x);
				
				tmpProductions = new ArrayList<Production>();
				for (Production y : recursiveProductions) {
					tmpSymbols = y.getRhsTail();
					tmpProductions.add(new Production(newNonTerminal, tmpSymbols));
					tmpSymbols.add(newNonTerminal);
					tmpProductions.add(new Production(newNonTerminal, tmpSymbols));
				}
				groupedProductions.put(order.get(newNonTerminal), tmpProductions);
				
				tmpProductions = new ArrayList<Production>();
				for (Production y : nonRecursiveProductions) {
					tmpSymbols = y.getRhsCopy();
					tmpProductions.add(new Production(y.getLhs(), tmpSymbols));
					tmpSymbols.add(newNonTerminal);
					tmpProductions.add(new Production(y.getLhs(), tmpSymbols));
				}
				groupedProductions.put(x, tmpProductions);
			}
		}
		
		// go through every productions bottom-up
		// to make sure that every group does not depend on the subsequent ones
		// and thus does not depend on any other production
		
		keyArray = groupedProductions.keySet().toArray(new Integer[] {});
		for (i = keyArray.length - 2; i >= 0; --i) {
			tmpProductions = new ArrayList<Production>();
			for (Production x : groupedProductions.get(keyArray[i]))
				if (! x.getRhs(0).isTerminal)
					for (Production y : groupedProductions.get(order.get(x.getRhs(0))))
						tmpProductions.add(new Production(x, y));
				else
					tmpProductions.add(x);
			groupedProductions.put(keyArray[i], tmpProductions);
		}
		
		// clean up and finish
		
		newProductions = new ArrayList<Production>();
		for (Integer x : groupedProductions.keySet())
			for (Production y : groupedProductions.get(x))
				newProductions.add(y);
		newProductions.sort(new ProductionComparator());
		
		newGrammar.nonTerminals = newNonTerminals;
		newGrammar.productions = newProductions;
		
		return new NormalGreibachGrammar(newGrammar);
	}
}