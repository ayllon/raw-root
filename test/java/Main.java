import raw.root.*;

class MyVisitor implements IVisitor
{
	@Override
	public boolean pre(Node node) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void post(Node node) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void unknown(Node node) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void leaf(String name, Data data) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void leaf(long index, Data data) {
		// TODO Auto-generated method stub
		
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
		
		if (fNode.isValid()) {
			MyVisitor visitor = new MyVisitor();
			walker.walk(fNode, visitor);
		}
	}
}
