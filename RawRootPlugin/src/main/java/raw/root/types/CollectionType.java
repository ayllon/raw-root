package raw.root.types;


public class CollectionType implements Type {
	protected Type contained;
	
	public CollectionType(Type contained) {
		this.contained = contained;
	}
	
	@Override
	public String toString() {
		return "CollectionType(" + this.contained.toString() + ")";
	}
}
