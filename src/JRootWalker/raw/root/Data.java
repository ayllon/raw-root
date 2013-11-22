package raw.root;

public class Data {
	public enum DataType {
		kUnknown, kInt8, kInt32, kInt64, kUInt8, kUInt32, kUInt64, kFloat, kDouble, kString
	};

	public Data() {
		initialize();
	}

	public native String getTypeName();

	public native DataType getType();

	public native byte getByte();

	public native int getInt32();

	public native long getLong();

	public native float getFloat();

	public native double getDouble();

	public native String getString();

	protected native void initialize();

	protected native void finalize();

	protected long ptr;
};
