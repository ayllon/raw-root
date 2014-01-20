import sbt._
import java.io._
import Keys._
import com.typesafe.sbteclipse.plugin.EclipsePlugin.EclipseKeys
import org.seacourt.build._


object RawRootPlugin extends NativeDefaultBuild("RawRootPlugin") {
    lazy val compileFlags = nativeCXXCompileFlags in Compile ++= {
        Seq("-std=c++11")
    }
  
    // RootWalker and handlers
    lazy val walkerSettings = NativeProject.sharedLibrarySettings ++ Seq(
        compileFlags,
        nativeIncludeDirectories in Compile += file("/usr/include/root"),
        nativeIncludeDirectories in Compile += file("./RootWalker/source"),
        nativeIncludeDirectories in Compile += file("/usr/lib/jvm/java-7-openjdk-amd64/include"),
        EclipseKeys.skipProject := true
   )
    
    lazy val rootWalker = NativeProject(
        "RootWalker", file("RootWalker/"), walkerSettings
    )
    
    def makeNativeProject(f: File): Project =
        NativeProject(f.getName, f, walkerSettings) nativeDependsOn rootWalker
    
    lazy val availableHandlers: Array[Project] = 
        new File("./RootWalker/handlers").listFiles().filter(_.isDirectory).map(makeNativeProject(_))
  
    // Native part of the wrapper
    lazy val pluginNative = NativeProject(
        "RawRootPluginNative", file("RawRootPlugin/src/main/native/"),
        walkerSettings
    ) nativeDependsOn rootWalker
    
    // RAW    
    lazy val raw = uri("git://github.com/MiguelBranco/raw.git")   
    
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
