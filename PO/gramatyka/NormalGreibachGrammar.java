package gramatyka;

public class NormalGreibachGrammar extends ContextFreeGrammar{

	public NormalGreibachGrammar(ContextFreeGrammar g) throws IllegalArgumentException {
		super(g);
		if (! this.ifGreibach())
			throw new IllegalArgumentException("Referenced grammar is not in Greibach form");
	}
}