package raw.root.types;

import java.util.HashMap;
import java.util.Map;


public class RecordType extends HashMap<String, Type> implements Type {
	@Override
	public String toString() {
		String repr = new String("RecordType(");
		
		for (Map.Entry<String, Type> record: this.entrySet()) {
			repr += record.getKey() + ": " + record.getValue().toString() + ", ";
		}
		
		if (repr.endsWith(", "))
			repr = repr.substring(0, repr.length() - 2);
		
		return repr + ")";
	}
}
