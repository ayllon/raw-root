package raw.root;

public class Node {
	public Node() {
		initialize();
	}

	public Node(String rootPath) {
		initialize(rootPath);
	}

	public native String getTypeName();

	public native String getName();

	public native boolean isBasic();

	public native Data getData();

	public native boolean isArray();

	public native String getContainedType();

	public native boolean isNull();
	
	public boolean isValid() {
		return !isNull();
	}

	protected native void initialize();

	protected native void initialize(String rootPath);

	protected native void finalize();

	protected long ptr;
};
