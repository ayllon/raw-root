package raw.root;

public class Walker {
	public Walker() {
		initialize();
	}

	public native void walk(Node node, IVisitor visitor);

	public native Node getChildNode(Node node, String path);

	protected native void initialize();

	protected native void finalize();

	protected long ptr;
	
	static {
		System.loadLibrary("JRootWalkerNative");
	}	
}
