package raw.root.types;

public final class UnknownType implements Type {
	protected String unknown;
	
	public UnknownType(String u) {
		unknown = u;
	}
	
	@Override
	public String toString() {
		return "UnknownType(" + unknown + ")";
	}
}
