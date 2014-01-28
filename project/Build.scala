import sbt._
import java.io._
import Keys._
import com.typesafe.sbteclipse.plugin.EclipsePlugin.EclipseKeys
import org.seacourt.build._
import sbt.Process._

object RootConfig {
    def libraries: Seq[String] = {
        val output: String = (Process("root-config" :: "--libs" :: "--noldflags" :: Nil) !!)
        output.trim.split(" ").filter(_.startsWith("-l")).map(_.substring(2))
    }
    
    def linkFlags: Seq[String] = {
        val output: String = (Process("root-config" :: "--libs" :: Nil) !!)
        val lib_flags = output.trim.split(" ").filter(!_.startsWith("-l"))
        lib_flags ++ Seq("-Wl,--no-as-needed")
    }
}


object RawRootPlugin extends NativeDefaultBuild("RawRootPlugin") {
    lazy val compileFlags = nativeCXXCompileFlags in Compile ++= {
        Seq()
    }
  
    // RootWalker and handlers
    lazy val walkerSettings = NativeProject.sharedLibrarySettings ++ Seq(
        nativeCXXCompileFlags in Compile    += "-std=c++11",
        nativeIncludeDirectories in Compile += file("/usr/include/root"),
        nativeIncludeDirectories in Compile += file("./RootWalker/source"),
        nativeIncludeDirectories in Compile += file("/usr/lib/jvm/java-7-openjdk-amd64/include"),
        nativeLibraries in Compile ++= RootConfig.libraries,
        nativeDynamicLibraryLinkFlags in Compile ++= RootConfig.linkFlags,
        EclipseKeys.skipProject := true
   )
    
    lazy val rootWalker = NativeProject(
        "RootWalker", file("RootWalker/"),
        walkerSettings
    )
    
    def makeNativeProject(f: File): Project =
        NativeProject("Handler_" + f.getName, f, walkerSettings) nativeDependsOn rootWalker
    
    lazy val availableHandlers: Array[Project] = 
        new File("./RootWalker/handlers").listFiles().filter(_.isDirectory).map(makeNativeProject(_))
  
    // Native part of the wrapper
    lazy val pluginNative = NativeProject(
        "libRawRootPluginNative", file("RawRootPlugin/src/main/native/"),
        walkerSettings
    ) nativeDependsOn rootWalker
    
    // RAW    
    lazy val raw = uri("git://github.com/ayllon/raw.git")   
    
    // Java/Scala wrapper
    lazy val plugin = Project(
        id = "RawRootPluginJVM",
        base = file("RawRootPlugin")
    ) dependsOn (pluginNative, raw)
    
    // Overload projects
    override def projects: Seq[Project] = {
        List(super.projects: _*) ::: List(availableHandlers: _*)
    }
}
