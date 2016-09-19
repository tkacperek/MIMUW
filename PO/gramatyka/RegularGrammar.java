package gramatyka;

public class RegularGrammar extends ContextFreeGrammar{

	public RegularGrammar(ContextFreeGrammar g) throws IllegalArgumentException {
		super(g);
		if (! this.ifRegular())
			throw new IllegalArgumentException("Referenced grammar is not regular");
	}
}
