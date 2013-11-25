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

	public native boolean isArray();

	public native String getContainedType();

	public boolean isNull() {
		return ptr == 0;
	}
	
	public boolean isValid() {
		return ptr != 0;
	}

	protected native void initialize();

	protected native void initialize(String rootPath);

	protected native void finalize();

	protected long ptr;
	protected long tFilePtr;
};
