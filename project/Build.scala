import sbt._
import Keys._
import org.seacourt.build._

object RawRootPlugin extends NativeDefaultBuild("RawRootPlugin") {
    lazy val compileFlags = nativeCXXCompileFlags in Compile ++= {
        Seq("-std=c++11")
    }
  
    lazy val rootWalker = NativeProject(
        "RootWalker", file("RootWalker/"),
        NativeProject.sharedLibrarySettings ++ Seq(
            compileFlags,
            nativeIncludeDirectories in Compile += file("/usr/include/root")
        )
    )
  
    lazy val pluginNative = NativeProject(
        "RawRootPluginNative", file("RawRootPlugin/src/main/native"),
        NativeProject.sharedLibrarySettings ++ Seq(
            compileFlags,
            nativeIncludeDirectories in Compile += file("/usr/include/root"),
            nativeIncludeDirectories in Compile += file("./RootWalker/source"),
            nativeIncludeDirectories in Compile += file("/usr/lib/jvm/java-7-openjdk-amd64/include")
        )
    ) nativeDependsOn rootWalker
    
    lazy val plugin = Project(
        id = "RawRootPluginJVM",
        base = file("RawRootPlugin")
    ) dependsOn (pluginNative, raw)
    
    lazy val raw = uri("git://github.com/MiguelBranco/raw.git")
}
