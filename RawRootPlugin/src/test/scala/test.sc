import java.io.File
import raw.root.RootLoader

object Test {
  def getWorksheetPath = {
      val wsClassPath = Test.getClass.getResource("Test.class").getFile()
      new File(wsClassPath + "../../../..").getCanonicalPath
  }                                               //> getWorksheetPath: => String
  
  def getHandlerPath = {
    new File(getWorksheetPath + "/stage/walker/handlers/").getCanonicalPath
  }                                               //> getHandlerPath: => String

  val loader = new RootLoader(getHandlerPath)     //> loader  : raw.root.RootLoader = raw.root.RootLoader@2f215fab
  
  val f = loader("/home/aalvarez/Sources/raw-root/TestRootFile/TestFile.root")
                                                  //> f  : raw.calculus.MonoidType = null
}