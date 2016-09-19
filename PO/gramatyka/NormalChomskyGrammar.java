package gramatyka;

public class NormalChomskyGrammar extends ContextFreeGrammar{

	public NormalChomskyGrammar(ContextFreeGrammar g) throws IllegalArgumentException {
		super(g);
		if (! this.ifChomsky())
			throw new IllegalArgumentException("Referenced grammar is not in Chomsky form");
	}
}