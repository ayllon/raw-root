import raw.root.Inferrer;
import raw.root.types.Type;

class Main {
	public static void main(String[] args) {
		if (args.length < 1) {
			System.err.println("Expecting a file name as parameter");
			System.exit(-1);
		}
		
		String filePath = args[0];
		String objPath = new String();
		if (args.length > 1)
			objPath = args[1];
		
		System.out.format("Using as root node %s:%s\n", filePath, objPath);
		
		Type schema = new Inferrer().infer(filePath, objPath);
		System.out.println(schema.toString());
	}
}
