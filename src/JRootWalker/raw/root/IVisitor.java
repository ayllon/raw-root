package raw.root;

public interface IVisitor {
	public boolean pre(Node node);

	public void post(Node node);

	public void unknown(Node node);

	public void leaf(String name, Data data);

	public void leaf(long index, Data data);
};
