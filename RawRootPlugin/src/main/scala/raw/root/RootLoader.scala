package raw.root

import java.io.File
import raw.calculus._


class RootLoader(handlerPath: String) {
    val inferrer = new Inferrer(handlerPath)
  
    def apply(file: String, node: String): MonoidType = {
        val root = inferrer.infer(file, node)
        null
    }
    
    def apply(file: String): MonoidType = apply(file, null)
}
