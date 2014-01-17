package raw.root;

import raw.root.types.Type;


public class Inferrer {
	
	private String handlerPath;
	
	public Inferrer() {
		handlerPath = null;
	}
	
	public Inferrer(String handlerPath) {
		this.handlerPath = handlerPath;
	}
	
	public Type infer(String path, String node) {
		return this.infer(path, node, handlerPath);
	}
	
	private native Type infer(String path, String node, String handlerPath);
	
	static {
        System.loadLibrary("RootInferrerNative");
	}   
}
