package gramatyka;

public class Main {

	private static void testGrammar(ContextFreeGrammar g) {
		g.ifRegular();
		g.ifChomsky();
		g.ifGreibach();
		System.out.println(g);
	}
	
	public static void main(String[] args) {
		
		String t, nt;
		String [][] p;

		System.out.println("Test 1: Niepoprawna gramatyka");
		try {
			@SuppressWarnings("unused")
			ContextFreeGrammar incorrectGrammar = new ContextFreeGrammar("lubie placki", "SĄ SMACZNE", new String [][] {{},{}});
		} catch (IllegalArgumentException e) {
			System.out.println("Błąd: " + e.getMessage());
		}
		
		System.out.println("\nTest 2: Gramatyka bezkontekstowa (przykład z treści zadania)");
		t = "ab";
		nt = "SPQR";
		p = new String [][] {{"P","Q","R"},{"a","aP","aPb"},{"b", "Qb", "aQb"},{""}};
		ContextFreeGrammar cfg = new ContextFreeGrammar(t, nt, p);
		testGrammar(cfg);
		
		System.out.println("\nTest 3: Gramatyka regularna");
		t = "abc";
		nt = "AS";
		p = new String [][] {{"","cA"},{"aS","bA"}};
		ContextFreeGrammar regular = new ContextFreeGrammar(t, nt, p);
		testGrammar(regular);
		
		System.out.println("\nTest 4: Gramatyka w formie Chomsky'ego i Greibach");
		t = "ab";
		nt = "ABC";
		p = new String [][] {{"BC"}, {"CA", "b"}, {"AB", "a"}};
		ContextFreeGrammar chomsky = new ContextFreeGrammar(t, nt, p);
		testGrammar(chomsky);
		NormalGreibachGrammar greibach = chomsky.toGreibach();
		testGrammar(greibach);
	}
}
