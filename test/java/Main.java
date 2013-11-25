import java.util.Arrays;

import raw.root.*;

class MyVisitor implements IVisitor
{
	protected int tabLevel;
	
	public MyVisitor() {
		tabLevel = 0;
	}
	
	String getIndent() {
		char[] chars = new char[tabLevel];
		Arrays.fill(chars,  '\t');
		return new String(chars);
	}
	
	@Override
	public boolean pre(Node node) {
		System.out.format("%s+%s %s", this.getIndent(), node.getTypeName(), node.getName());

        if (node.isArray())
            System.out.print("[");
        else
        	System.out.print("{");
        System.out.print("\n");        
		
		++tabLevel;
		return true;
	}

	@Override
	public void post(Node node) {
        --tabLevel;
        if (node.isArray())
            System.out.format("\n%s]\n", getIndent());
        else
        	System.out.format("%s}\n", getIndent());
	}

	@Override
	public void unknown(Node node) {
        System.out.format("%s?%s %s", getIndent(), node.getTypeName(), node.getName());
	}

	@Override
	public void leaf(String name, Data data) {
		System.out.format("%s %s %s = %s", getIndent(), data.getTypeName(), name, data.toString());
	}

	@Override
	public void leaf(long index, Data data) {
        if (index == 0) {
        	System.out.print(getIndent());
        }
        System.out.format("%s,", data.toString());
	}
	
}



class Main {
	public static void main(String[] args) {
		if (args.length < 1) {
			System.err.println("Expecting a file name as parameter");
			System.exit(-1);
		}
		
		Walker walker = new Walker();
		
		Node fNode = new Node(args[0]);
		if (args.length > 1)
			fNode = walker.getChildNode(fNode, args[1]);
		
		System.out.format("Using as root node %s (%s)\n", fNode.getName(), fNode.getTypeName());
		
		if (fNode.isValid()) {
			MyVisitor visitor = new MyVisitor();
			walker.walk(fNode, visitor);
		}
	}
}
